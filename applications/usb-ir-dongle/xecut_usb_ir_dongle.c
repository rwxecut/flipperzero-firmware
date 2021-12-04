#include "xecut_usb_ir_dongle.h"
#include "xecut_usb_ir_dongle_state.h"


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
