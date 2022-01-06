#include <furi_hal.h>
#include <irda.h>
#include "irusb_config.h"
#include "irusb_dispatch.h"

#define IRUSB_DISPATCH_REPEAT true
#define IRUSB_DISPATCH_NOREPEAT false

IrusbDispatchTable* irusb_dispatch_init(void) {
    auto dispatch_table = new IrusbDispatchTable {
        // vol up
        {{IrdaProtocolNEC, 0x00, 0x15, IRUSB_DISPATCH_NOREPEAT},
            {.type = IrusbHidReportKb, .kb_keycode = KEY_UP_ARROW}},
        {{IrdaProtocolNEC, 0x00, 0x15, IRUSB_DISPATCH_REPEAT},
            {.type = IrusbHidReportKb, .kb_keycode = KEY_UP_ARROW}},
        {{IrdaProtocolNEC, 0x00, 0x11, IRUSB_DISPATCH_NOREPEAT},
            {.type = IrusbHidReportKb, .kb_keycode = KEY_UP_ARROW}},
        {{IrdaProtocolNEC, 0x00, 0x11, IRUSB_DISPATCH_REPEAT},
            {.type = IrusbHidReportKb, .kb_keycode = KEY_UP_ARROW}},
        // vol down
        {{IrdaProtocolNEC, 0x00, 0x7, IRUSB_DISPATCH_NOREPEAT},
            {.type = IrusbHidReportKb, .kb_keycode = KEY_DOWN_ARROW}},
        {{IrdaProtocolNEC, 0x00, 0x7, IRUSB_DISPATCH_REPEAT},
            {.type = IrusbHidReportKb, .kb_keycode = KEY_DOWN_ARROW}},
        {{IrdaProtocolNEC, 0x00, 0x10, IRUSB_DISPATCH_NOREPEAT},
            {.type = IrusbHidReportKb, .kb_keycode = KEY_DOWN_ARROW}},
        {{IrdaProtocolNEC, 0x00, 0x10, IRUSB_DISPATCH_REPEAT},
            {.type = IrusbHidReportKb, .kb_keycode = KEY_DOWN_ARROW}},
        // rewind
        {{IrdaProtocolNEC, 0x00, 0x44, IRUSB_DISPATCH_NOREPEAT},
            {.type = IrusbHidReportKb, .kb_keycode = KEY_LEFT_ARROW}},
        {{IrdaProtocolNEC, 0x00, 0x44, IRUSB_DISPATCH_REPEAT},
            {.type = IrusbHidReportKb, .kb_keycode = KEY_LEFT_ARROW}},
        // fast forward
        {{IrdaProtocolNEC, 0x00, 0x40, IRUSB_DISPATCH_NOREPEAT},
            {.type = IrusbHidReportKb, .kb_keycode = KEY_RIGHT_ARROW}},
        {{IrdaProtocolNEC, 0x00, 0x40, IRUSB_DISPATCH_REPEAT},
            {.type = IrusbHidReportKb, .kb_keycode = KEY_RIGHT_ARROW}},
        // play/pause (no repeat)
        {{IrdaProtocolNEC, 0x00, 0x43, IRUSB_DISPATCH_NOREPEAT},
            {.type = IrusbHidReportKb, .kb_keycode = KEY_SPACE}},

        // vol up
        {{IrdaProtocolSamsung32, 0x07, 0x07, IRUSB_DISPATCH_NOREPEAT},
            {.type = IrusbHidReportKb, .kb_keycode = KEY_UP_ARROW}},
        {{IrdaProtocolSamsung32, 0x07, 0x07, IRUSB_DISPATCH_REPEAT},
            {.type = IrusbHidReportKb, .kb_keycode = KEY_UP_ARROW}},
        // vol down
        {{IrdaProtocolSamsung32, 0x07, 0x0B, IRUSB_DISPATCH_NOREPEAT},
            {.type = IrusbHidReportKb, .kb_keycode = KEY_DOWN_ARROW}},
        {{IrdaProtocolSamsung32, 0x07, 0x0B, IRUSB_DISPATCH_REPEAT},
            {.type = IrusbHidReportKb, .kb_keycode = KEY_DOWN_ARROW}},
        // rewind
        {{IrdaProtocolSamsung32, 0x07, 0x45, IRUSB_DISPATCH_NOREPEAT},
            {.type = IrusbHidReportKb, .kb_keycode = KEY_LEFT_ARROW}},
        {{IrdaProtocolSamsung32, 0x07, 0x45, IRUSB_DISPATCH_REPEAT},
            {.type = IrusbHidReportKb, .kb_keycode = KEY_LEFT_ARROW}},
        // fast forward
        {{IrdaProtocolSamsung32, 0x07, 0x48, IRUSB_DISPATCH_NOREPEAT},
            {.type = IrusbHidReportKb, .kb_keycode = KEY_RIGHT_ARROW}},
        {{IrdaProtocolSamsung32, 0x07, 0x48, IRUSB_DISPATCH_REPEAT},
            {.type = IrusbHidReportKb, .kb_keycode = KEY_RIGHT_ARROW}},
        // play/pause (no repeat)
        {{IrdaProtocolSamsung32, 0x07, 0x4A, IRUSB_DISPATCH_NOREPEAT},
            {.type = IrusbHidReportKb, .kb_keycode = KEY_SPACE}},
        {{IrdaProtocolSamsung32, 0x07, 0x47, IRUSB_DISPATCH_NOREPEAT},
            {.type = IrusbHidReportKb, .kb_keycode = KEY_SPACE}},
        // previous video (no repeat)
        {{IrdaProtocolSamsung32, 0x07, 0x10, IRUSB_DISPATCH_NOREPEAT},
            {.type = IrusbHidReportKb, .kb_keycode = KEY_P | KEY_MOD_LEFT_SHIFT}},
        // next video (no repeat)
        {{IrdaProtocolSamsung32, 0x07, 0x12, IRUSB_DISPATCH_NOREPEAT},
            {.type = IrusbHidReportKb, .kb_keycode = KEY_N | KEY_MOD_LEFT_SHIFT}},
        // mute/unmute (no repeat)
        {{IrdaProtocolSamsung32, 0x07, 0x0F, IRUSB_DISPATCH_NOREPEAT},
            {.type = IrusbHidReportKb, .kb_keycode = KEY_M}},
        // mouse move
        {{IrdaProtocolSamsung32, 0x07, 0x60, IRUSB_DISPATCH_NOREPEAT},
            {.type = IrusbHidReportMouseMove, .mouse_move = {
                .dx = 0, .dy = -MOUSE_MOVE_SHORT
            }}},
        {{IrdaProtocolSamsung32, 0x07, 0x60, IRUSB_DISPATCH_REPEAT},
            {.type = IrusbHidReportMouseMove, .mouse_move = {
                .dx = 0, .dy = -MOUSE_MOVE_LONG
            }}},
        {{IrdaProtocolSamsung32, 0x07, 0x61, IRUSB_DISPATCH_NOREPEAT},
            {.type = IrusbHidReportMouseMove, .mouse_move = {
                .dx = 0, .dy = MOUSE_MOVE_SHORT
            }}},
        {{IrdaProtocolSamsung32, 0x07, 0x61, IRUSB_DISPATCH_REPEAT},
            {.type = IrusbHidReportMouseMove, .mouse_move = {
                .dx = 0, .dy = MOUSE_MOVE_LONG
            }}},
        {{IrdaProtocolSamsung32, 0x07, 0x65, IRUSB_DISPATCH_NOREPEAT},
            {.type = IrusbHidReportMouseMove, .mouse_move = {
                .dx = -MOUSE_MOVE_SHORT, .dy = 0
            }}},
        {{IrdaProtocolSamsung32, 0x07, 0x65, IRUSB_DISPATCH_REPEAT},
            {.type = IrusbHidReportMouseMove, .mouse_move = {
                .dx = -MOUSE_MOVE_LONG, .dy = 0
            }}},
        {{IrdaProtocolSamsung32, 0x07, 0x62, IRUSB_DISPATCH_NOREPEAT},
            {.type = IrusbHidReportMouseMove, .mouse_move = {
                .dx = MOUSE_MOVE_SHORT, .dy = 0
            }}},
        {{IrdaProtocolSamsung32, 0x07, 0x62, IRUSB_DISPATCH_REPEAT},
            {.type = IrusbHidReportMouseMove, .mouse_move = {
                .dx = MOUSE_MOVE_LONG, .dy = 0
            }}},
        // mouse click (no repeat)
        {{IrdaProtocolSamsung32, 0x07, 0x68, IRUSB_DISPATCH_NOREPEAT},
            {.type = IrusbHidReportMouseClick,
             .mouse_click_button = HID_MOUSE_BTN_LEFT}},
        {{IrdaProtocolSamsung32, 0x07, 0x1F, IRUSB_DISPATCH_NOREPEAT},
            {.type = IrusbHidReportMouseClick,
             .mouse_click_button = HID_MOUSE_BTN_RIGHT}},
    };
    return dispatch_table;
}

IrusbHidReport irusb_dispatch(const IrusbDispatchTable* dispatch_table,
    const IrdaMessage* msg) {
    furi_assert(dispatch_table);
    furi_assert(msg);

    auto dispatch_report = dispatch_table->find(*msg);
    if (dispatch_report != dispatch_table->end()) {
        return dispatch_report->second;
    } else {
        return { .type = IrusbHidReportNone };
    }
}

void irusb_dispatch_free(IrusbDispatchTable* dispatch_table) {
    furi_assert(dispatch_table);
    delete dispatch_table;
}
