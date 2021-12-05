#pragma once

#include "usb_ir_dongle.h"

void usb_ir_dongle_input_callback(InputEvent* input_event, void* ctx);
void usb_ir_dongle_tick_callback(void* ctx);
void usb_ir_dongle_render_callback(Canvas* canvas, void* ctx);
UIDState* usb_ir_dongle_init(ValueMutex* state_mutex);
void usb_ir_dongle_free(UIDState* state);
void usb_ir_dongle_loop(UIDState* state, ValueMutex* state_mutex);
