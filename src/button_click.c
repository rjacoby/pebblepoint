#include <pebble.h>

static Window *window;
static TextLayer *prompt_text_layer, *up_label_text_layer, *down_label_text_layer;

static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
  text_layer_set_text(prompt_text_layer, "Selected");
}

static void up_click_handler(ClickRecognizerRef recognizer, void *context) {
  text_layer_set_text(prompt_text_layer, "Previous");
}

static void down_click_handler(ClickRecognizerRef recognizer, void *context) {
  text_layer_set_text(prompt_text_layer, "Next");
}

static void click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
  window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);
}

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  prompt_text_layer = text_layer_create((GRect) { .origin = { 0, 62 }, .size = { bounds.size.w, 60 } });
  text_layer_set_text(prompt_text_layer, "Use Up/Down buttons to control your slides");
  text_layer_set_text_alignment(prompt_text_layer, GTextAlignmentCenter);
  // text_layer_set_background_color(prompt_text_layer, GColorBlack);
  // text_layer_set_text_color(prompt_text_layer, GColorWhite);
  layer_add_child(window_layer, text_layer_get_layer(prompt_text_layer));

  up_label_text_layer = text_layer_create((GRect) { .origin = { 0, 0}, .size = { bounds.size.w - 10, 20} });
  text_layer_set_text(up_label_text_layer, "PREV");
  text_layer_set_text_alignment(up_label_text_layer, GTextAlignmentRight);
  layer_add_child(window_layer, text_layer_get_layer(up_label_text_layer));

  down_label_text_layer = text_layer_create((GRect) { .origin = { 0, bounds.size.h - 20}, .size = { bounds.size.w - 10, 20} });
  text_layer_set_text(down_label_text_layer, "NEXT");
  text_layer_set_text_alignment(down_label_text_layer, GTextAlignmentRight);
  layer_add_child(window_layer, text_layer_get_layer(down_label_text_layer));
}

static void window_unload(Window *window) {
  text_layer_destroy(prompt_text_layer);
}

static void init(void) {
  window = window_create();
  window_set_click_config_provider(window, click_config_provider);
  window_set_window_handlers(window, (WindowHandlers) {
	  .load = window_load,
    .unload = window_unload,
  });
  const bool animated = true;
  window_stack_push(window, animated);
}

static void deinit(void) {
  window_destroy(window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
