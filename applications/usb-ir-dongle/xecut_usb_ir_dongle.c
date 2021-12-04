#include <furi.h>
#include <gui/gui.h>
#include <input/input.h>

#define TAG "XECUT_USB_IR_DONGLE"


typedef struct {
	osMessageQueueId_t event_queue;
	ViewPort* view_port;
	Gui* gui;
	osTimerId_t timer;
} XecutUIDState;

typedef enum {
	EventTypeTick,
	EventTypeInput
} EventType;

typedef struct {
	union {
		InputEvent input;
	};
	EventType type;
} XecutUIDEvent;


static void xecut_usb_ir_dongle_input_callback(InputEvent* input_event, void* ctx) {
	furi_assert(ctx);
	XecutUIDState *state = (XecutUIDState*)ctx;
	XecutUIDEvent event = {.type = EventTypeInput, .input = *input_event};
	osMessageQueuePut(state->event_queue, &event, 0, osWaitForever);
}


static void xecut_usb_ir_dongle_tick_callback(void* ctx) {
	furi_assert(ctx);
	XecutUIDState *state = (XecutUIDState*)ctx;
	XecutUIDEvent event = {.type = EventTypeTick};
	osMessageQueuePut(state->event_queue, &event, 0, 0);
}


static void xecut_usb_ir_dongle_render_callback(Canvas* canvas, void* ctx) {
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


static XecutUIDState* xecut_usb_ir_dongle_init(ValueMutex* state_mutex) {
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


static void xecut_usb_ir_dongle_free(XecutUIDState* state) {
	view_port_enabled_set(state->view_port, false);
	gui_remove_view_port(state->gui, state->view_port);
	view_port_free(state->view_port);
	furi_record_close("gui");
	osTimerDelete(state->timer);
	osMessageQueueDelete(state->event_queue);
	free(state);
}


int32_t xecut_usb_ir_dongle_app(void* p) {
	ValueMutex state_mutex;
	XecutUIDState *state = xecut_usb_ir_dongle_init(&state_mutex);
	if (!init_mutex(&state_mutex, state, sizeof(XecutUIDState))) {
		FURI_LOG_E(TAG, "failed to create mutex");
		return 256;
	}

	XecutUIDEvent event;
	for (bool running = true; running;) {
		osStatus_t event_status = osMessageQueueGet(state->event_queue, &event, NULL, 100);
		XecutUIDState *state = (XecutUIDState*)acquire_mutex_block(&state_mutex);
		if (event_status == osOK) {
			if (event.type == EventTypeInput && event.input.type == InputTypePress) {
				switch (event.input.key) {
				case InputKeyBack:
					running = false;
				default:
					break;
				}
			} else if (event.type == EventTypeTick) {
				// do smth
			}
		}
		view_port_update(state->view_port);
		release_mutex(&state_mutex, state);
	}

	delete_mutex(&state_mutex);
	xecut_usb_ir_dongle_free(state);
	return 0;
}
