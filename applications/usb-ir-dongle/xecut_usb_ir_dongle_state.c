#include "xecut_usb_ir_dongle.h"
#include "xecut_usb_ir_dongle_state.h"

void xecut_usb_ir_dongle_input_callback(InputEvent* input_event, void* ctx) {
	furi_assert(ctx);
	XecutUIDState *state = (XecutUIDState*)ctx;
	XecutUIDEvent event = {.type = EventTypeInput, .input = *input_event};
	osMessageQueuePut(state->event_queue, &event, 0, osWaitForever);
}


void xecut_usb_ir_dongle_tick_callback(void* ctx) {
	furi_assert(ctx);
	XecutUIDState *state = (XecutUIDState*)ctx;
	XecutUIDEvent event = {.type = EventTypeTick};
	osMessageQueuePut(state->event_queue, &event, 0, 0);
}


void xecut_usb_ir_dongle_render_callback(Canvas* canvas, void* ctx) {
	XecutUIDState *state = (XecutUIDState*)acquire_mutex((ValueMutex*)ctx, 25);
	if (!state) return;

	canvas_set_font(canvas, FontPrimary);
	canvas_draw_str(canvas, 0, 10, "XECUT: AIRWAVES");
	canvas_set_font(canvas, FontSecondary);
	canvas_draw_str(canvas, 0, 19, "DON'T");
	canvas_set_font(canvas, FontPrimary);
	canvas_draw_str(canvas, 0, 28, "LIE");

	release_mutex((ValueMutex*)ctx, state);
}


XecutUIDState* xecut_usb_ir_dongle_init(ValueMutex* state_mutex) {
	XecutUIDState* state = furi_alloc(sizeof(XecutUIDState));
	state->event_queue = osMessageQueueNew(8, sizeof(XecutUIDEvent), NULL);
	furi_check(state->event_queue);
	state->view_port = view_port_alloc();
	view_port_draw_callback_set(state->view_port, xecut_usb_ir_dongle_render_callback, state_mutex);
	view_port_input_callback_set(state->view_port, xecut_usb_ir_dongle_input_callback, state);
	state->gui = furi_record_open("gui");
	gui_add_view_port(state->gui, state->view_port, GuiLayerFullscreen);
	state->timer = osTimerNew(xecut_usb_ir_dongle_tick_callback, osTimerPeriodic, state, NULL);
	osTimerStart(state->timer, osKernelGetTickFreq() / 4);
	return state;
}


void xecut_usb_ir_dongle_free(XecutUIDState* state) {
	view_port_enabled_set(state->view_port, false);
	gui_remove_view_port(state->gui, state->view_port);
	view_port_free(state->view_port);
	furi_record_close("gui");
	osTimerDelete(state->timer);
	osMessageQueueDelete(state->event_queue);
	free(state);
}
