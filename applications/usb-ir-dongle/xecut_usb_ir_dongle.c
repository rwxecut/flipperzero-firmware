#include <furi.h>
#include <gui/gui.h>
#include <input/input.h>

#define TAG "XECUT_USB_IR_DONGLE"


typedef struct {
	bool press[5];
	uint16_t up;
	uint16_t down;
	uint16_t left;
	uint16_t right;
	uint16_t ok;
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


static void xecut_usb_ir_dongle_input_callback(InputEvent* input_event, osMessageQueueId_t event_queue) {
	furi_assert(event_queue);
	XecutUIDEvent event = {.type = EventTypeInput, .input = *input_event};
	osMessageQueuePut(event_queue, &event, 0, osWaitForever);
}


static void xecut_usb_ir_dongle_tick_callback(osMessageQueueId_t event_queue) {
	furi_assert(event_queue);
	XecutUIDEvent event = {.type = EventTypeTick};
	osMessageQueuePut(event_queue, &event, 0, 0);
}


static void xecut_usb_ir_dongle_render_callback(Canvas* canvas, void* ctx) {
	XecutUIDState* state = (XecutUIDState*)acquire_mutex((ValueMutex*)ctx, 25);
	if (!state) return;

	canvas_set_font(canvas, FontPrimary);
	canvas_draw_str(canvas, 0, 10, "XECUT: AIRWAVES");
	canvas_set_font(canvas, FontSecondary);
	canvas_draw_str(canvas, 0, 19, "DON'T");
	canvas_set_font(canvas, FontPrimary);
	canvas_draw_str(canvas, 0, 28, "LIE");

	release_mutex((ValueMutex*)ctx, state);
}


int32_t xecut_usb_ir_dongle_app(void* p) {
	osMessageQueueId_t event_queue = osMessageQueueNew(8, sizeof(XecutUIDState), NULL);
	furi_check(event_queue);

	XecutUIDState _state = {0};

	ValueMutex state_mutex;
	if(!init_mutex(&state_mutex, &_state, sizeof(XecutUIDState))) {
		FURI_LOG_E(TAG, "cannot create mutex");
		return 256;
	}

	ViewPort* view_port = view_port_alloc();
	view_port_draw_callback_set(view_port, xecut_usb_ir_dongle_render_callback, &state_mutex);
	view_port_input_callback_set(view_port, xecut_usb_ir_dongle_input_callback, event_queue);

	Gui* gui = furi_record_open("gui");
	gui_add_view_port(gui, view_port, GuiLayerFullscreen);

	osTimerId_t timer = osTimerNew(xecut_usb_ir_dongle_tick_callback, osTimerPeriodic, event_queue, NULL);
	osTimerStart(timer, osKernelGetTickFreq() / 4);

	XecutUIDEvent event;
	for (bool running = true; running;) {
		osStatus_t event_status = osMessageQueueGet(event_queue, &event, NULL, 100);
		XecutUIDState* state = (XecutUIDState*)acquire_mutex_block(&state_mutex);
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
		view_port_update(view_port);
		release_mutex(&state_mutex, state);
	}

	osTimerDelete(timer);
	view_port_enabled_set(view_port, false);
	gui_remove_view_port(gui, view_port);
	view_port_free(view_port);
	furi_record_close("gui");
	osMessageQueueDelete(event_queue);
	delete_mutex(&state_mutex);
	return 0;
}
