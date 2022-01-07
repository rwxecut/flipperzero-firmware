#include <furi.h>
#include <furi_hal.h>
#include "irusb_config.h"
#include "irusb_state.h"

int32_t irusb_app(void* p) {
    ValueMutex state_mutex;
    IrusbState* state = irusb_init(&state_mutex);
    if(!init_mutex(&state_mutex, state, sizeof(IrusbState))) {
        FURI_LOG_E(TAG, "failed to create mutex");
        return 256;
    }
    UsbInterface* usb_mode_save = furi_hal_usb_get_config();
    furi_hal_usb_set_config(&usb_hid);

    irusb_loop(state, &state_mutex);

    furi_hal_hid_kb_release_all();
    delete_mutex(&state_mutex);
    irusb_free(state);
    furi_hal_usb_set_config(usb_mode_save);
    return 0;
}
