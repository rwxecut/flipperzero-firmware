#include <furi-hal.h>
#include <irda.h>
#include "irusb_dispatch.h"

IrusbDispatchTable* irusb_dispatch_init(void) {
    auto dispatch_table = new IrusbDispatchTable {
        {{IrdaProtocolNEC, 0x00, 0x15}, KEY_UP_ARROW},
        {{IrdaProtocolNEC, 0x00, 0x11}, KEY_UP_ARROW},
        {{IrdaProtocolNEC, 0x00, 0x7}, KEY_DOWN_ARROW},
        {{IrdaProtocolNEC, 0x00, 0x10}, KEY_DOWN_ARROW},
        {{IrdaProtocolNEC, 0x00, 0x44}, KEY_LEFT_ARROW},
        {{IrdaProtocolNEC, 0x00, 0x40}, KEY_RIGHT_ARROW},
        {{IrdaProtocolNEC, 0x00, 0x43}, KEY_SPACE},

        {{IrdaProtocolSamsung32, 0x07, 0x07}, KEY_UP_ARROW},
        {{IrdaProtocolSamsung32, 0x07, 0x0B}, KEY_DOWN_ARROW},
        {{IrdaProtocolSamsung32, 0x07, 0x45}, KEY_LEFT_ARROW},
        {{IrdaProtocolSamsung32, 0x07, 0x48}, KEY_RIGHT_ARROW},
        {{IrdaProtocolSamsung32, 0x07, 0x4A}, KEY_SPACE},
        {{IrdaProtocolSamsung32, 0x07, 0x47}, KEY_SPACE},
        {{IrdaProtocolSamsung32, 0x07, 0x10}, KEY_P | KEY_MOD_LEFT_SHIFT},
        {{IrdaProtocolSamsung32, 0x07, 0x12}, KEY_N | KEY_MOD_LEFT_SHIFT},
        {{IrdaProtocolSamsung32, 0x07, 0x0F}, KEY_M},
    };
    return dispatch_table;
}

void irusb_dispatch_free(IrusbDispatchTable* dispatch_table) {
    delete dispatch_table;
}
