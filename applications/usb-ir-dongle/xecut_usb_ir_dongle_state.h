#pragma once

#include "xecut_usb_ir_dongle.h"


void xecut_usb_ir_dongle_input_callback(InputEvent* input_event, void* ctx);
void xecut_usb_ir_dongle_tick_callback(void* ctx);
void xecut_usb_ir_dongle_render_callback(Canvas* canvas, void* ctx);
XecutUIDState* xecut_usb_ir_dongle_init(ValueMutex* state_mutex);
void xecut_usb_ir_dongle_free(XecutUIDState* state);
void xecut_usb_ir_dongle_loop(XecutUIDState* state, ValueMutex* state_mutex);
