#pragma once

#include "irusb.h"

void irusb_input_callback(InputEvent* input_event, void* ctx);
void irusb_tick_callback(void* ctx);
void irusb_render_callback(Canvas* canvas, void* ctx);
IrusbState* irusb_init(ValueMutex* state_mutex);
void irusb_free(IrusbState* state);
void irusb_loop(IrusbState* state, ValueMutex* state_mutex);
