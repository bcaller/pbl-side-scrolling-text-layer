#pragma once
#include <pebble.h>

typedef struct SideScrollingTextLayer SideScrollingTextLayer;

SideScrollingTextLayer *side_scrolling_text_layer_create(GRect frame);
void side_scrolling_text_layer_destroy(SideScrollingTextLayer *layer);
Layer* side_scrolling_text_layer_get_layer(SideScrollingTextLayer* layer);

void side_scrolling_text_layer_set_text(SideScrollingTextLayer* layer, char* text);

void side_scrolling_text_layer_set_font(SideScrollingTextLayer* layer, GFont font);

void side_scrolling_text_layer_set_text_color(SideScrollingTextLayer * layer, GColor text_color);

void side_scrolling_text_layer_set_text_alignment(SideScrollingTextLayer * text_layer, GTextAlignment text_alignment);

void side_scrolling_text_layer_animate(SideScrollingTextLayer *layer);

// Maximum displacement of text
int16_t side_scrolling_text_layer_get_max_offset(SideScrollingTextLayer* layer);

void side_scrolling_text_layer_set_forward_duration(SideScrollingTextLayer* layer, int dur);

void side_scrolling_text_layer_set_reverse_duration(SideScrollingTextLayer* layer, int dur);

// Note: an end_delay < ~150 may not work
void side_scrolling_text_layer_set_delays(SideScrollingTextLayer* layer, int start_delay, int end_delay);

void side_scrolling_text_layer_set_curve(SideScrollingTextLayer* layer, AnimationCurve curve, AnimationCurveFunction curve_function);