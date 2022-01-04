#include <furi-hal.h>
#include <irda.h>
#include "irusb_config.h"
#include "irusb_dispatch.h"

IrusbDispatchTable* irusb_dispatch_init(void) {
    auto dispatch_table = new IrusbDispatchTable {
        // vol up
        {{IrdaProtocolNEC, 0x00, 0x15, false},
            {.type = IrusbActionKb, .kb_keycode = KEY_UP_ARROW}},
        {{IrdaProtocolNEC, 0x00, 0x15, true},
            {.type = IrusbActionKb, .kb_keycode = KEY_UP_ARROW}},
        {{IrdaProtocolNEC, 0x00, 0x11, false},
            {.type = IrusbActionKb, .kb_keycode = KEY_UP_ARROW}},
        {{IrdaProtocolNEC, 0x00, 0x11, true},
            {.type = IrusbActionKb, .kb_keycode = KEY_UP_ARROW}},
        // vol down
        {{IrdaProtocolNEC, 0x00, 0x7, false},
            {.type = IrusbActionKb, .kb_keycode = KEY_DOWN_ARROW}},
        {{IrdaProtocolNEC, 0x00, 0x7, true},
            {.type = IrusbActionKb, .kb_keycode = KEY_DOWN_ARROW}},
        {{IrdaProtocolNEC, 0x00, 0x10, false},
            {.type = IrusbActionKb, .kb_keycode = KEY_DOWN_ARROW}},
        {{IrdaProtocolNEC, 0x00, 0x10, true},
            {.type = IrusbActionKb, .kb_keycode = KEY_DOWN_ARROW}},
        // rewind
        {{IrdaProtocolNEC, 0x00, 0x44, false},
            {.type = IrusbActionKb, .kb_keycode = KEY_LEFT_ARROW}},
        {{IrdaProtocolNEC, 0x00, 0x44, true},
            {.type = IrusbActionKb, .kb_keycode = KEY_LEFT_ARROW}},
        // fast forward
        {{IrdaProtocolNEC, 0x00, 0x40, false},
            {.type = IrusbActionKb, .kb_keycode = KEY_RIGHT_ARROW}},
        {{IrdaProtocolNEC, 0x00, 0x40, true},
            {.type = IrusbActionKb, .kb_keycode = KEY_RIGHT_ARROW}},
        // play/pause (no repeat)
        {{IrdaProtocolNEC, 0x00, 0x43, false},
            {.type = IrusbActionKb, .kb_keycode = KEY_SPACE}},

        // vol up
        {{IrdaProtocolSamsung32, 0x07, 0x07, false},
            {.type = IrusbActionKb, .kb_keycode = KEY_UP_ARROW}},
        {{IrdaProtocolSamsung32, 0x07, 0x07, true},
            {.type = IrusbActionKb, .kb_keycode = KEY_UP_ARROW}},
        // vol down
        {{IrdaProtocolSamsung32, 0x07, 0x0B, false},
            {.type = IrusbActionKb, .kb_keycode = KEY_DOWN_ARROW}},
        {{IrdaProtocolSamsung32, 0x07, 0x0B, true},
            {.type = IrusbActionKb, .kb_keycode = KEY_DOWN_ARROW}},
        // rewind
        {{IrdaProtocolSamsung32, 0x07, 0x45, false},
            {.type = IrusbActionKb, .kb_keycode = KEY_LEFT_ARROW}},
        {{IrdaProtocolSamsung32, 0x07, 0x45, true},
            {.type = IrusbActionKb, .kb_keycode = KEY_LEFT_ARROW}},
        // fast forward
        {{IrdaProtocolSamsung32, 0x07, 0x48, false},
            {.type = IrusbActionKb, .kb_keycode = KEY_RIGHT_ARROW}},
        {{IrdaProtocolSamsung32, 0x07, 0x48, true},
            {.type = IrusbActionKb, .kb_keycode = KEY_RIGHT_ARROW}},
        // play/pause (no repeat)
        {{IrdaProtocolSamsung32, 0x07, 0x4A, false},
            {.type = IrusbActionKb, .kb_keycode = KEY_SPACE}},
        {{IrdaProtocolSamsung32, 0x07, 0x47, false},
            {.type = IrusbActionKb, .kb_keycode = KEY_SPACE}},
        // previous video (no repeat)
        {{IrdaProtocolSamsung32, 0x07, 0x10, false},
            {.type = IrusbActionKb, .kb_keycode = KEY_P | KEY_MOD_LEFT_SHIFT}},
        // next video (no repeat)
        {{IrdaProtocolSamsung32, 0x07, 0x12, false},
            {.type = IrusbActionKb, .kb_keycode = KEY_N | KEY_MOD_LEFT_SHIFT}},
        // mute/unmute (no repeat)
        {{IrdaProtocolSamsung32, 0x07, 0x0F, false},
            {.type = IrusbActionKb, .kb_keycode = KEY_M}},
        // mouse move
        {{IrdaProtocolSamsung32, 0x07, 0x60, false},
            {.type = IrusbActionMouseMove, .mouse_move = {
                .dx = 0, .dy = -MOUSE_MOVE_SHORT
            }}},
        {{IrdaProtocolSamsung32, 0x07, 0x60, true},
            {.type = IrusbActionMouseMove, .mouse_move = {
                .dx = 0, .dy = -MOUSE_MOVE_LONG
            }}},
        {{IrdaProtocolSamsung32, 0x07, 0x61, false},
            {.type = IrusbActionMouseMove, .mouse_move = {
                .dx = 0, .dy = MOUSE_MOVE_SHORT
            }}},
        {{IrdaProtocolSamsung32, 0x07, 0x61, true},
            {.type = IrusbActionMouseMove, .mouse_move = {
                .dx = 0, .dy = MOUSE_MOVE_LONG
            }}},
        {{IrdaProtocolSamsung32, 0x07, 0x65, false},
            {.type = IrusbActionMouseMove, .mouse_move = {
                .dx = -MOUSE_MOVE_SHORT, .dy = 0
            }}},
        {{IrdaProtocolSamsung32, 0x07, 0x65, true},
            {.type = IrusbActionMouseMove, .mouse_move = {
                .dx = -MOUSE_MOVE_LONG, .dy = 0
            }}},
        {{IrdaProtocolSamsung32, 0x07, 0x62, false},
            {.type = IrusbActionMouseMove, .mouse_move = {
                .dx = MOUSE_MOVE_SHORT, .dy = 0
            }}},
        {{IrdaProtocolSamsung32, 0x07, 0x62, true},
            {.type = IrusbActionMouseMove, .mouse_move = {
                .dx = MOUSE_MOVE_LONG, .dy = 0
            }}},
        // mouse click (no repeat)
        {{IrdaProtocolSamsung32, 0x07, 0x68, false},
            {.type = IrusbActionMouseClick,
             .mouse_click_button = HID_MOUSE_BTN_LEFT}},
        {{IrdaProtocolSamsung32, 0x07, 0x1F, false},
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
