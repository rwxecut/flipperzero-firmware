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


static void xecut_usb_ir_dongle_keystroke(InputType input_type, uint16_t button)
{
	if (input_type == InputTypePress) furi_hal_hid_kb_press(button);
	else if (input_type == InputTypeRelease) furi_hal_hid_kb_release(button);
}


void xecut_usb_ir_dongle_loop(XecutUIDState* state, ValueMutex* state_mutex) {
	XecutUIDEvent event;
	for (bool running = true; running;) {
		osStatus_t event_status = osMessageQueueGet(state->event_queue, &event, NULL, 100);
		XecutUIDState *_state = (XecutUIDState*)acquire_mutex_block(state_mutex);
		if (event_status == osOK) {
			if (event.type == EventTypeInput) {
				switch (event.input.key) {
				case InputKeyLeft:
					FURI_LOG_I(TAG, "Pressed Left");
					xecut_usb_ir_dongle_keystroke(event.input.type, KEY_LEFT_ARROW);
					break;
				case InputKeyRight:
					FURI_LOG_I(TAG, "Pressed Right");
					xecut_usb_ir_dongle_keystroke(event.input.type, KEY_RIGHT_ARROW);
					break;
				case InputKeyUp:
					FURI_LOG_I(TAG, "Pressed Up");
					xecut_usb_ir_dongle_keystroke(event.input.type, KEY_UP_ARROW);
					break;
				case InputKeyDown:
					FURI_LOG_I(TAG, "Pressed Down");
					xecut_usb_ir_dongle_keystroke(event.input.type, KEY_DOWN_ARROW);
					break;
				case InputKeyOk:
					FURI_LOG_I(TAG, "Pressed OK");
					xecut_usb_ir_dongle_keystroke(event.input.type, KEY_MOD_LEFT_CTRL);
					break;
				case InputKeyBack:
					if (event.input.type == InputTypePress)
						running = false;
				default:
					break;
				}
			} else if (event.type == EventTypeTick) {
				// do smth
			}
		}
		view_port_update(_state->view_port);
		release_mutex(state_mutex, _state);
	}
}
