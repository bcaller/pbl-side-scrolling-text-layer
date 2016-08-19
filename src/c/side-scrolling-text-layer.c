#include <pebble.h>
#include "side-scrolling-text-layer.h"

// Side Scrolling Text Layer by Ben Caller
// Copyright Ben Caller 2016
// Licensed under GPLv3

typedef struct {
	GTextAlignment alignment;
	GColor text_color;
	GFont font;
	char* text;
	// Current animation position
	int16_t offset;
	// Maximum offset which still leaves the layer filled
	int16_t max_offset;
	Animation* scroll_anim;
	int delay_at_start;
	int delay_at_end;
	int reverse_scroll_duration;
	int forward_scroll_duration;
	bool max_offset_calculated;
	AnimationCurve curve;
	AnimationCurveFunction curve_function;
} SideScrollingTextData;

static int16_t ZERO = 0;

SideScrollingTextData* side_scrolling_text_layer_get_data(SideScrollingTextLayer* layer) {
	return layer_get_data((Layer*)layer);
}

void draw(Layer *layer, GContext *ctx) {
	SideScrollingTextData* data = side_scrolling_text_layer_get_data((SideScrollingTextLayer*)layer);
	GRect bounds = layer_get_bounds((Layer*)layer);
	graphics_context_set_text_color(ctx, data->text_color);
	
	graphics_draw_text(ctx, data->text, data->font, 
		(GRect){
			.origin={bounds.origin.x - data->offset,bounds.origin.y},
			.size={bounds.size.w + 200, bounds.size.h}
		}, GTextOverflowModeFill, data->alignment, NULL);
	
	graphics_context_set_text_color(ctx, data->text_color);
}

static void set_animated_offset(SideScrollingTextLayer *layer, int16_t offset) {
	SideScrollingTextData* data = side_scrolling_text_layer_get_data((SideScrollingTextLayer*)layer);
	data->offset = offset;
	layer_mark_dirty((Layer*)layer);
}

static const PropertyAnimationImplementation s_animation_implementation = {
	.base = {
		.update = (AnimationUpdateImplementation) property_animation_update_int16
	},
	.accessors = {
		.setter = { .int16 = (const Int16Setter) set_animated_offset }
	},
};

static void side_scrolling_text_layer_stop(SideScrollingTextLayer *layer) {
	SideScrollingTextData* data = side_scrolling_text_layer_get_data((SideScrollingTextLayer*)layer);
	if(animation_is_scheduled(data->scroll_anim)) {
		animation_unschedule(data->scroll_anim);
		data->offset = 0;
		layer_mark_dirty((Layer*)layer);
	}
}

void side_scrolling_text_layer_animate(SideScrollingTextLayer *layer) {
	side_scrolling_text_layer_stop(layer);
	side_scrolling_text_layer_get_max_offset(layer);
	SideScrollingTextData* data = side_scrolling_text_layer_get_data((SideScrollingTextLayer*)layer);
	if(data->max_offset <= 0) return;
	
	PropertyAnimation* scroll_anim = property_animation_create(&s_animation_implementation, layer, NULL, NULL);
	
	property_animation_set_from_int16(scroll_anim, &ZERO);
	property_animation_set_to_int16(scroll_anim, &data->max_offset);
	
	Animation* forward = property_animation_get_animation(scroll_anim);
	
	Animation* reversed = animation_clone(forward);
	animation_set_reverse(reversed, true);
	
	animation_set_duration(forward, data->forward_scroll_duration);
	animation_set_duration(reversed, data->reverse_scroll_duration);
	animation_set_delay(forward, data->delay_at_start);
	animation_set_delay(reversed, data->delay_at_end);
	animation_set_curve(forward, data->curve);
	if(data->curve == AnimationCurveCustomFunction) {
		animation_set_custom_curve(forward, data->curve_function);
	}
	animation_set_curve(reversed, AnimationCurveEaseIn);
	
	data->scroll_anim = animation_sequence_create(forward, reversed, NULL);
		
	animation_set_play_count(data->scroll_anim, ANIMATION_PLAY_COUNT_INFINITE);
	animation_schedule(data->scroll_anim);
}

SideScrollingTextLayer *side_scrolling_text_layer_create(GRect frame) {
	Layer *layer = layer_create_with_data(frame, sizeof(SideScrollingTextData));
	SideScrollingTextData* data = layer_get_data(layer);
	data->alignment = GTextAlignmentLeft;
	layer_set_update_proc(layer, draw);
	data->reverse_scroll_duration = 250;
	data->forward_scroll_duration = 3000;
	data->delay_at_end = 400;
	data->delay_at_start = 2500;
	data->curve = AnimationCurveEaseInOut;
	
	return (SideScrollingTextLayer*) layer;
}

void side_scrolling_text_layer_destroy(SideScrollingTextLayer *layer) {
	side_scrolling_text_layer_stop(layer);
	layer_destroy((Layer*)layer);
}

Layer* side_scrolling_text_layer_get_layer(SideScrollingTextLayer* layer) {
	return (Layer*)layer;
}

int16_t side_scrolling_text_layer_get_max_offset(SideScrollingTextLayer* layer) {
	SideScrollingTextData* data = side_scrolling_text_layer_get_data((SideScrollingTextLayer*)layer);
	if(!data->max_offset_calculated) {
		GRect bounds = layer_get_bounds((Layer*)layer);
		int16_t width_of_text = graphics_text_layout_get_content_size(
			data->text, data->font,
			(GRect){.origin=bounds.origin,.size={bounds.size.w * 15, bounds.size.h}},
			GTextOverflowModeWordWrap, GTextAlignmentLeft
		).w;
		data->max_offset = width_of_text - bounds.size.w;
		data->max_offset_calculated = true;
		
		if(data->max_offset < 0) data->max_offset = 0;
	}
	return data->max_offset;
}

void side_scrolling_text_layer_set_text(SideScrollingTextLayer* layer, char* text) {
	SideScrollingTextData* data = side_scrolling_text_layer_get_data((SideScrollingTextLayer*)layer);
	data->text = text;
	data->max_offset_calculated = false;
	side_scrolling_text_layer_stop(layer);
}

void side_scrolling_text_layer_set_font(SideScrollingTextLayer* layer, GFont font) {
	SideScrollingTextData* data = side_scrolling_text_layer_get_data((SideScrollingTextLayer*)layer);
	data->font = font;
	data->max_offset_calculated = false;
	side_scrolling_text_layer_stop(layer);
}

void side_scrolling_text_layer_set_forward_duration(SideScrollingTextLayer* layer, int dur) {
	SideScrollingTextData* data = side_scrolling_text_layer_get_data((SideScrollingTextLayer*)layer);
	data->forward_scroll_duration = dur;
}

void side_scrolling_text_layer_set_reverse_duration(SideScrollingTextLayer* layer, int dur) {
	SideScrollingTextData* data = side_scrolling_text_layer_get_data((SideScrollingTextLayer*)layer);
	data->reverse_scroll_duration = dur;
}

void side_scrolling_text_layer_set_delays(SideScrollingTextLayer* layer, int start_delay, int end_delay) {
	SideScrollingTextData* data = side_scrolling_text_layer_get_data((SideScrollingTextLayer*)layer);
	data->delay_at_start = start_delay;
	data->delay_at_end = end_delay;
}

void side_scrolling_text_layer_set_text_alignment(SideScrollingTextLayer * layer, GTextAlignment text_alignment){
	side_scrolling_text_layer_get_data((SideScrollingTextLayer*)layer)->alignment = text_alignment;
}

void side_scrolling_text_layer_set_text_color(SideScrollingTextLayer * layer, GColor text_color) {
	SideScrollingTextData* data = side_scrolling_text_layer_get_data((SideScrollingTextLayer*)layer);
	data->text_color = text_color;
}

void side_scrolling_text_layer_set_curve(SideScrollingTextLayer* layer, AnimationCurve curve, AnimationCurveFunction curve_function) {
	SideScrollingTextData* data = side_scrolling_text_layer_get_data((SideScrollingTextLayer*)layer);
	data->curve = curve;
	data->curve_function = curve_function;
}
