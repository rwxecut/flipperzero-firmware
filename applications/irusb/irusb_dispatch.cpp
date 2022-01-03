#include <furi-hal.h>
#include <irda.h>
#include "irusb_config.h"
#include "irusb_dispatch.h"

IrusbDispatchTable* irusb_dispatch_init(void) {
    auto dispatch_table = new IrusbDispatchTable {
        {{IrdaProtocolNEC, 0x00, 0x15},
            {.type = IrusbActionKb, .kb_keycode = KEY_UP_ARROW}},
        {{IrdaProtocolNEC, 0x00, 0x11},
            {.type = IrusbActionKb, .kb_keycode = KEY_UP_ARROW}},
        {{IrdaProtocolNEC, 0x00, 0x7},
            {.type = IrusbActionKb, .kb_keycode = KEY_DOWN_ARROW}},
        {{IrdaProtocolNEC, 0x00, 0x10},
            {.type = IrusbActionKb, .kb_keycode = KEY_DOWN_ARROW}},
        {{IrdaProtocolNEC, 0x00, 0x44},
            {.type = IrusbActionKb, .kb_keycode = KEY_LEFT_ARROW}},
        {{IrdaProtocolNEC, 0x00, 0x40},
            {.type = IrusbActionKb, .kb_keycode = KEY_RIGHT_ARROW}},
        {{IrdaProtocolNEC, 0x00, 0x43},
            {.type = IrusbActionKb, .kb_keycode = KEY_SPACE}},

        {{IrdaProtocolSamsung32, 0x07, 0x07},
            {.type = IrusbActionKb, .kb_keycode = KEY_UP_ARROW}},
        {{IrdaProtocolSamsung32, 0x07, 0x0B},
            {.type = IrusbActionKb, .kb_keycode = KEY_DOWN_ARROW}},
        {{IrdaProtocolSamsung32, 0x07, 0x45},
            {.type = IrusbActionKb, .kb_keycode = KEY_LEFT_ARROW}},
        {{IrdaProtocolSamsung32, 0x07, 0x48},
            {.type = IrusbActionKb, .kb_keycode = KEY_RIGHT_ARROW}},
        {{IrdaProtocolSamsung32, 0x07, 0x4A},
            {.type = IrusbActionKb, .kb_keycode = KEY_SPACE}},
        {{IrdaProtocolSamsung32, 0x07, 0x47},
            {.type = IrusbActionKb, .kb_keycode = KEY_SPACE}},
        {{IrdaProtocolSamsung32, 0x07, 0x10},
            {.type = IrusbActionKb, .kb_keycode = KEY_P | KEY_MOD_LEFT_SHIFT}},
        {{IrdaProtocolSamsung32, 0x07, 0x12},
            {.type = IrusbActionKb, .kb_keycode = KEY_N | KEY_MOD_LEFT_SHIFT}},
        {{IrdaProtocolSamsung32, 0x07, 0x0F},
            {.type = IrusbActionKb, .kb_keycode = KEY_M}},
        {{IrdaProtocolSamsung32, 0x07, 0x60},
            {.type = IrusbActionMouseMove, .mouse_move = {
                .dx = 0, .dy = -MOUSE_MOVE_SHORT
            }}},
        {{IrdaProtocolSamsung32, 0x07, 0x61},
            {.type = IrusbActionMouseMove, .mouse_move = {
                .dx = 0, .dy = MOUSE_MOVE_SHORT
            }}},
        {{IrdaProtocolSamsung32, 0x07, 0x65},
            {.type = IrusbActionMouseMove, .mouse_move = {
                .dx = -MOUSE_MOVE_SHORT, .dy = 0
            }}},
        {{IrdaProtocolSamsung32, 0x07, 0x62},
            {.type = IrusbActionMouseMove, .mouse_move = {
                .dx = MOUSE_MOVE_SHORT, .dy = 0
            }}},
        {{IrdaProtocolSamsung32, 0x07, 0x68},
            {.type = IrusbActionMouseClick,
             .mouse_click_button = HID_MOUSE_BTN_LEFT}},
        {{IrdaProtocolSamsung32, 0x07, 0x1F},
            {.type = IrusbActionMouseClick,
             .mouse_click_button = HID_MOUSE_BTN_RIGHT}},
    };
    return dispatch_table;
}

IrusbAction irusb_dispatch(const IrusbDispatchTable* dispatch_table,
    const IrdaMessage* msg) {
    auto dispatch_action = dispatch_table->find(*msg);
    if (dispatch_action != dispatch_table->end()) {
        return dispatch_action->second;
    } else {
        return { .type = IrusbActionNone };
    }
}

void irusb_dispatch_free(IrusbDispatchTable* dispatch_table) {
    delete dispatch_table;
}
