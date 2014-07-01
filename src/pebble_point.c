#include <pebble.h>

static Window *window;
static Layer *prompt_layer, *status_layer, *controls_layer;
static TextLayer *prompt_text_layer, *status_text_layer, *slide_index_text_layer, *up_label_text_layer, *down_label_text_layer;

static void send_message_to_phone(char* command) {
  DictionaryIterator *iter;
  app_message_outbox_begin(&iter);
  Tuplet value = TupletCString(0, command);
  dict_write_tuplet(iter, &value);
  app_message_outbox_send();
}

static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
  // text_layer_set_text(prompt_text_layer, "Selected");
}

static void up_click_handler(ClickRecognizerRef recognizer, void *context) {
  // text_layer_set_text(prompt_text_layer, "Previous");
  send_message_to_phone("previous");
  vibes_short_pulse();
}

static void down_click_handler(ClickRecognizerRef recognizer, void *context) {
  // text_layer_set_text(prompt_text_layer, "Next");
  send_message_to_phone("next");
  vibes_short_pulse();
}

static void long_up_click_handler(ClickRecognizerRef recognizer, void *context) {
  // text_layer_set_text(prompt_text_layer, "First");
  send_message_to_phone("first");
  vibes_double_pulse();
}

static void long_down_click_handler(ClickRecognizerRef recognizer, void *context) {
  // text_layer_set_text(prompt_text_layer, "Last");
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
  //*** Set up to level container
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);
  window_set_background_color(window, GColorBlack);

  //*** Controls container
  controls_layer = layer_create(bounds);
  // Top control
  up_label_text_layer = text_layer_create((GRect) { .origin = { 0, 0}, .size = { bounds.size.w - 10, 20} });
  text_layer_set_text(up_label_text_layer, "PREV");
  text_layer_set_text_alignment(up_label_text_layer, GTextAlignmentRight);
  text_layer_set_background_color(up_label_text_layer, GColorBlack);
  text_layer_set_text_color(up_label_text_layer, GColorWhite);
  layer_add_child(controls_layer, text_layer_get_layer(up_label_text_layer));
  // Bottom control
  down_label_text_layer = text_layer_create((GRect) { .origin = { 0, bounds.size.h - 20}, .size = { bounds.size.w - 10, 20} });
  text_layer_set_text(down_label_text_layer, "NEXT");
  text_layer_set_text_alignment(down_label_text_layer, GTextAlignmentRight);
  text_layer_set_background_color(down_label_text_layer, GColorBlack);
  text_layer_set_text_color(down_label_text_layer, GColorWhite);
  layer_add_child(controls_layer, text_layer_get_layer(down_label_text_layer));
  // Add it!
  layer_add_child(window_layer, controls_layer);

  //*** GRect for the middle of screen
  GRect mid_rect = (GRect) { .origin = {0, 30}, .size = { bounds.size.w, bounds.size.h - 50}};

  //*** Prompt; can be replaced with status
  int prompt_height_offset = 50;
  prompt_layer = layer_create(((GRect) { .origin = {0, prompt_height_offset}, .size = {mid_rect.size.w, mid_rect.size.h - prompt_height_offset}}));
  prompt_text_layer = text_layer_create(layer_get_bounds(prompt_layer));
  text_layer_set_text(prompt_text_layer, "Press for Prev/Next\n\nHold for First/Last");
  text_layer_set_text_alignment(prompt_text_layer, GTextAlignmentCenter);
  text_layer_set_background_color(prompt_text_layer, GColorBlack);
  text_layer_set_text_color(prompt_text_layer, GColorWhite);
  layer_add_child(prompt_layer, text_layer_get_layer(prompt_text_layer));
  layer_add_child(window_layer, prompt_layer);

  //*** Status; starts out hidden
  status_layer = layer_create(mid_rect);

  GRect slide_index_rect = (GRect) {.origin = {0, 0}, .size = {mid_rect.size.w, mid_rect.size.h - 40}};
  slide_index_text_layer = text_layer_create(slide_index_rect);
  text_layer_set_text_alignment(slide_index_text_layer, GTextAlignmentCenter);
  text_layer_set_font(slide_index_text_layer, fonts_get_system_font(FONT_KEY_ROBOTO_BOLD_SUBSET_49));
  text_layer_set_background_color(slide_index_text_layer, GColorBlack);
  text_layer_set_text_color(slide_index_text_layer, GColorWhite);

  status_text_layer = text_layer_create((GRect) {.origin = {0, (slide_index_rect.origin.y + slide_index_rect.size.h)}, .size = {mid_rect.size.w, (mid_rect.size.h - slide_index_rect.size.h)}});
  text_layer_set_text_alignment(status_text_layer, GTextAlignmentCenter);
  text_layer_set_font(status_text_layer, fonts_get_system_font(FONT_KEY_ROBOTO_CONDENSED_21));
  text_layer_set_background_color(status_text_layer, GColorBlack);
  text_layer_set_text_color(status_text_layer, GColorWhite);

  layer_add_child(status_layer, text_layer_get_layer(status_text_layer));
  layer_add_child(status_layer, text_layer_get_layer(slide_index_text_layer));
  layer_add_child(window_layer, status_layer);
  layer_set_hidden(status_layer, true);
}

static void window_unload(Window *window) {
  text_layer_destroy(prompt_text_layer);
  layer_destroy(prompt_layer);

  layer_destroy(status_layer);

  text_layer_destroy(up_label_text_layer);
  text_layer_destroy(down_label_text_layer);
  layer_destroy(controls_layer);
}

// Handle ACK from phone
void out_sent_handler(DictionaryIterator *sent, void *context) {
  // outgoing message was delivered
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Message delivered to phone");
}

// Handle NACK from phone
void out_failed_handler(DictionaryIterator *failed, AppMessageResult reason, void *context) {
  // outgoing message failed
  APP_LOG(APP_LOG_LEVEL_DEBUG, "Message NOT delivered to phone");
  text_layer_set_text(prompt_text_layer, "Cannot connect to phone");
  text_layer_set_text_alignment(prompt_text_layer, GTextAlignmentLeft);
  layer_set_hidden(prompt_layer, false);
  layer_set_hidden(status_layer, true);
}

enum {
  AKEY_COMMAND,
  AKEY_SUCCESS,
  AKEY_MESSAGE,
  AKEY_SLIDE_INDEX,
  AKEY_SLIDE_TOTAL
};

// Give ACK to phone
void in_received_handler(DictionaryIterator *received, void *context) {
  Tuple *success_tuple = dict_find(received, AKEY_SUCCESS);
  if((unsigned int)success_tuple->value->uint32 == 1){
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Success");
    Tuple *index_tuple = dict_find(received, AKEY_SLIDE_INDEX);
    Tuple *total_tuple = dict_find(received, AKEY_SLIDE_TOTAL);
    if(index_tuple && total_tuple){
      static char index_message[4] = "";
      static char total_message[16] = "";
      snprintf(index_message, sizeof(index_message), "%u", (unsigned int)index_tuple->value->uint32);
      snprintf(total_message, sizeof(total_message), "of %u", (unsigned int)total_tuple->value->uint32);
      // Set the response
      APP_LOG(APP_LOG_LEVEL_DEBUG, "Message: %s %s", index_message, total_message);
      text_layer_set_text(slide_index_text_layer, index_message);
      text_layer_set_text(status_text_layer, total_message);
      layer_set_hidden(status_layer, false);
      layer_set_hidden(prompt_layer, true);
    }
  } else {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "Failure");
    Tuple *error_message_tuple = dict_find(received, AKEY_MESSAGE);
    APP_LOG(APP_LOG_LEVEL_DEBUG, error_message_tuple->value->cstring);
    text_layer_set_text(prompt_text_layer, error_message_tuple->value->cstring);
    text_layer_set_text_alignment(prompt_text_layer, GTextAlignmentLeft);
    layer_set_hidden(prompt_layer, false);
    layer_set_hidden(status_layer, true);
  }
}

// Give NACK to phone
void in_dropped_handler(AppMessageResult reason, void *context) {
  // When can this happen?
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
