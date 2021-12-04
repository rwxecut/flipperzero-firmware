#pragma once

#include <furi.h>
#include <gui/gui.h>
#include <input/input.h>

#include "xecut_usb_ir_dongle.h"


void xecut_usb_ir_dongle_input_callback(InputEvent* input_event, void* ctx);
void xecut_usb_ir_dongle_tick_callback(void* ctx);
void xecut_usb_ir_dongle_render_callback(Canvas* canvas, void* ctx);
XecutUIDState* xecut_usb_ir_dongle_init(ValueMutex* state_mutex);
void xecut_usb_ir_dongle_free(XecutUIDState* state);
