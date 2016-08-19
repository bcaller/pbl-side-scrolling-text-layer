# Pebble side-scrolling-text-layer

Animated single-line TextLayer, scrolling to show the whole string.

The text scrolls back and forth to display a long line of text on the small watch screen.

## Functions

See [include/side-scrolling-text-layer.h](https://github.com/bcaller/pbl-side-scrolling-text-layer/blob/master/include/side-scrolling-text-layer.h)

```C
int16_t side_scrolling_text_layer_get_max_displacement(SideScrollingTextLayer* layer);
```
Gets the number of animation steps - the maximum offset in characters which the text needs to scroll. Only calculate this after setting the text and font.

## Usage

Treat it like a `TextLayer`. Create with a `GRect` frame. You must set text and font. Optionally set animation parameters such as delays at each end of the animation. Then start the animation by calling `side_scrolling_text_layer_animate`.

```C
#include <side-scrolling-text-layer/side-scrolling-text-layer.h>

SideScrollingTextLayer* sstl = side_scrolling_text_layer_create(bounds);

side_scrolling_text_layer_set_text_color(sstl, GColorRed);
side_scrolling_text_layer_set_text(sstl, "This text is too long to be displayed");
side_scrolling_text_layer_set_font(sstl, fonts_get_system_font(FONT_KEY_GOTHIC_24_BOLD));
side_scrolling_text_layer_set_curve(sstl, AnimationCurveLinear, NULL);
side_scrolling_text_layer_set_forward_duration(sstl, 180*side_scrolling_text_layer_get_max_displacement(side));

//Kick off
side_scrolling_text_layer_animate(sstl);
```
