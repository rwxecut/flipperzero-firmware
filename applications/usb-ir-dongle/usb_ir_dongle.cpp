#include "usb_ir_dongle.h"
#include "usb_ir_dongle_state.h"


extern "C" int32_t usb_ir_dongle_app(void* p) {
	ValueMutex state_mutex;
	UIDState *state = usb_ir_dongle_init(&state_mutex);
	if (!init_mutex(&state_mutex, state, sizeof(UIDState))) {
		FURI_LOG_E(TAG, "failed to create mutex");
		return 256;
	}
	UsbInterface* usb_mode_save = furi_hal_usb_get_config();
	furi_hal_usb_set_config(&usb_hid);

	usb_ir_dongle_loop(state, &state_mutex);

	furi_hal_hid_kb_release_all();
	delete_mutex(&state_mutex);
	usb_ir_dongle_free(state);
	furi_hal_usb_set_config(usb_mode_save);
	return 0;
}