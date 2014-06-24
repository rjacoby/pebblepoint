#include <pebble.h>

static Window *window;
static TextLayer *prompt_text_layer, *up_label_text_layer, *down_label_text_layer;

static void send_message_to_phone(char* command) {
  DictionaryIterator *iter;
  app_message_outbox_begin(&iter);
  Tuplet value = TupletCString(0, command);
  dict_write_tuplet(iter, &value);
  app_message_outbox_send();
}

static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
  text_layer_set_text(prompt_text_layer, "Selected");
}

static void up_click_handler(ClickRecognizerRef recognizer, void *context) {
  text_layer_set_text(prompt_text_layer, "Previous");
  send_message_to_phone("previous");
  vibes_short_pulse();
}

static void down_click_handler(ClickRecognizerRef recognizer, void *context) {
  text_layer_set_text(prompt_text_layer, "Next");
  send_message_to_phone("next");
  vibes_short_pulse();
}

static void long_up_click_handler(ClickRecognizerRef recognizer, void *context) {
  text_layer_set_text(prompt_text_layer, "First");
  send_message_to_phone("first");
  vibes_double_pulse();
}

static void long_down_click_handler(ClickRecognizerRef recognizer, void *context) {
  text_layer_set_text(prompt_text_layer, "Last");
  send_message_to_phone("last");
  vibes_double_pulse();
}

static void click_config_provider(void *context) {
  window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
  window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
  window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);

  window_long_click_subscribe(BUTTON_ID_UP, 1500, long_up_click_handler, NULL);
  window_long_click_subscribe(BUTTON_ID_DOWN, 1500, long_down_click_handler, NULL);
}

static void window_load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  prompt_text_layer = text_layer_create((GRect) { .origin = { 0, 40 }, .size = { bounds.size.w, 70 } });
  text_layer_set_text(prompt_text_layer, "Use Up/Down buttons for Prev/Next\n\nHold to jump to First/Last");
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

// Handle ACK from phone
void out_sent_handler(DictionaryIterator *sent, void *context) {
  // outgoing message was delivered
}

// Handle NACK from phone
void out_failed_handler(DictionaryIterator *failed, AppMessageResult reason, void *context) {
  // outgoing message failed
}

// Give ACK to phone
void in_received_handler(DictionaryIterator *received, void *context) {
  text_layer_set_text(prompt_text_layer, "Received a SUCCESS");
}

// Give NACK to phone
void in_dropped_handler(AppMessageResult reason, void *context) {
  text_layer_set_text(prompt_text_layer, "Received a FAIL");
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

  app_message_register_inbox_received(in_received_handler);
  app_message_register_inbox_dropped(in_dropped_handler);
  app_message_register_outbox_sent(out_sent_handler);
  app_message_register_outbox_failed(out_failed_handler);

  const uint32_t inbound_size = 64;
  const uint32_t outbound_size = 64;
  app_message_open(inbound_size, outbound_size);
}

static void deinit(void) {
  window_destroy(window);
}

int main(void) {
  init();
  app_event_loop();
  deinit();
}
