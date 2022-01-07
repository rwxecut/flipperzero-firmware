#include <m-string.h>
#include <furi.h>
#include <furi_hal.h>
#include <gui/gui.h>
#include <input/input.h>
#include <irda.h>
#include <irda_worker.h>
#include <storage/storage.h>
#include "irusb_config.h"
#include "irusb_state.h"
#include "irusb_dispatch.h"

typedef enum { EventTypeInput } EventType;

typedef struct {
    union {
        InputEvent input;
    };
    EventType type;
} IrusbEvent;

static void irusb_send_report(IrusbHidReport report) {
    char const* button_text = NULL;

    switch(report.type) {
    case IrusbHidReportKb:
        FURI_LOG_I(TAG, "Sending HID report: keyboard: %#X", report.kb_keycode);
        furi_hal_hid_kb_press(report.kb_keycode);
        furi_hal_hid_kb_release(report.kb_keycode);
        break;
    case IrusbHidReportMouseMove:
        FURI_LOG_I(
            TAG,
            "Sending HID report: mouse move: dx: %d; dy: %d",
            report.mouse_move.dx,
            report.mouse_move.dy);
        furi_hal_hid_mouse_move(report.mouse_move.dx, report.mouse_move.dy);
        break;
    case IrusbHidReportMouseClick:
        button_text = report.mouse_click_button == HID_MOUSE_BTN_LEFT ? "Left" :
                      HID_MOUSE_BTN_RIGHT                             ? "Right" :
                                                                        NULL;
        if(button_text != NULL) {
            FURI_LOG_I(TAG, "Sending HID report: mouse click: %s", button_text);
            furi_hal_hid_mouse_press(report.mouse_click_button);
            furi_hal_hid_mouse_release(report.mouse_click_button);
            break;
        }
        goto bogus_report;
    case IrusbHidReportNone:
        FURI_LOG_W(
            TAG,
            "Unable to dispatch the event: "
            "no HID report defined in config");
        break;
    default:
    bogus_report:
        FURI_LOG_E(
            TAG,
            "Error dispatching the event: got bogus HID report "
            " (probably a bug)");
        break;
    }
}

static void irusb_input_callback(InputEvent* input_event, void* ctx) {
    furi_assert(ctx);
    IrusbState* state = (IrusbState*)ctx;
    IrusbEvent event = {.input = *input_event, .type = EventTypeInput};
    osMessageQueuePut(state->event_queue, &event, 0, osWaitForever);
}

static void irusb_render_callback(Canvas* canvas, void* ctx) {
    furi_assert(ctx);
    IrusbState* state = (IrusbState*)acquire_mutex((ValueMutex*)ctx, 25);
    if(!state) return;

    const char* app_text =
        !string_array_empty_p(state->app_list) ?
            string_get_cstr(*string_array_get(state->app_list, state->app_list_pos)) :
            "Empty";
    const char* remote_text =
        !string_array_empty_p(state->remote_list) ?
            string_get_cstr(*string_array_get(state->remote_list, state->remote_list_pos)) :
            "Empty";
    canvas_draw_icon(canvas, 0, 0, &I_irusb);
    canvas_set_font(canvas, FontSecondary);
    //canvas_draw_str(canvas, 0, 37, "U/D App, L/R Remote");
    canvas_draw_str(canvas, 15, 34, app_text);
    canvas_draw_str(canvas, 15, 49, remote_text);
    //canvas_draw_str(canvas, 0, 46, "Last IRDA Signal received:");
    canvas_draw_str(canvas, 15, 19, state->irda_text[0] ? state->irda_text : "None");

    release_mutex((ValueMutex*)ctx, state);
}

static void irusb_signal_received_callback(void* ctx, IrdaWorkerSignal* sig) {
    furi_assert(ctx);
    furi_assert(sig);
    IrusbState* state = (IrusbState*)ctx;
    IrusbHidReport hid_report = {.type = IrusbHidReportNone};
    if(irda_worker_signal_is_decoded(sig)) {
        const IrdaMessage* message = irda_worker_get_decoded_signal(sig);
        snprintf(
            state->irda_text,
            sizeof(state->irda_text),
            "%s A:0x%0*lX C:0x%0*lX %s",
            irda_get_protocol_name(message->protocol),
            ROUND_UP_TO(irda_get_protocol_address_length(message->protocol), 4),
            message->address,
            ROUND_UP_TO(irda_get_protocol_command_length(message->protocol), 4),
            message->command,
            message->repeat ? " R" : "");
        hid_report = irusb_dispatch(state->dispatch_table, message);
    } else {
        const uint32_t* timings;
        size_t timings_cnt;
        irda_worker_get_raw_signal(sig, &timings, &timings_cnt);
        snprintf(state->irda_text, sizeof(state->irda_text), "RAW %d samples", timings_cnt);
    }
    view_port_update(state->view_port);
    FURI_LOG_I(TAG, "Received an IRDA signal event: %s", state->irda_text);
    irusb_send_report(hid_report);
}

IrusbState* irusb_init(ValueMutex* state_mutex) {
    IrusbState* state = (IrusbState*)furi_alloc(sizeof(IrusbState));

    state->worker = irda_worker_alloc();
    irda_worker_rx_start(state->worker);
    irda_worker_rx_set_received_signal_callback(
        state->worker, irusb_signal_received_callback, state);
    irda_worker_rx_enable_blink_on_receiving(state->worker, true);

    string_array_init(state->remote_list);
    string_array_init(state->app_list);
    state->storage = (Storage*)furi_record_open("storage");
    const uint8_t filename_size = 100;
    char* filename = (char*)furi_alloc(filename_size);
    FileInfo fileinfo;
    File* assets_dir = storage_file_alloc(state->storage);
    int result = 0;

    storage_dir_open(assets_dir, "/ext/irusb/remotes");
    do {
        result = storage_dir_read(assets_dir, &fileinfo, filename, filename_size);
        if(result) {
            string_t* remote_name = string_array_push_new(state->remote_list);
            string_set_str(*remote_name, filename);
        }
    } while(result);
    storage_dir_close(assets_dir);
    storage_dir_open(assets_dir, "/ext/irusb/apps");
    do {
        result = storage_dir_read(assets_dir, &fileinfo, filename, filename_size);
        if(result) {
            string_t* app_name = string_array_push_new(state->app_list);
            string_set_str(*app_name, filename);
        }
    } while(result);
    storage_dir_close(assets_dir);
    storage_file_free(assets_dir);
    free(filename);

    state->app_list_pos = state->remote_list_pos = 0;
    state->dispatch_table = irusb_dispatch_init();

    state->event_queue = osMessageQueueNew(8, sizeof(IrusbEvent), NULL);
    furi_check(state->event_queue);

    state->view_port = view_port_alloc();
    view_port_draw_callback_set(state->view_port, irusb_render_callback, state_mutex);
    view_port_input_callback_set(state->view_port, irusb_input_callback, state);
    state->gui = (Gui*)furi_record_open("gui");
    gui_add_view_port(state->gui, state->view_port, GuiLayerFullscreen);

    return state;
}

void irusb_free(IrusbState* state) {
    view_port_enabled_set(state->view_port, false);
    gui_remove_view_port(state->gui, state->view_port);
    view_port_free(state->view_port);
    furi_record_close("gui");
    furi_record_close("storage");
    string_array_clear(state->remote_list);
    string_array_clear(state->app_list);
    irusb_dispatch_free(state->dispatch_table);
    osMessageQueueDelete(state->event_queue);
    irda_worker_rx_stop(state->worker);
    irda_worker_free(state->worker);
    free(state);
}

void irusb_loop(IrusbState* state, ValueMutex* state_mutex) {
    IrusbEvent event;
    for(bool running = true; running;) {
        osStatus_t event_status = osMessageQueueGet(state->event_queue, &event, NULL, 100);
        IrusbState* _state = (IrusbState*)acquire_mutex_block(state_mutex);
        if(event_status == osOK && event.type == EventTypeInput &&
           event.input.type == InputTypePress) {
            switch(event.input.key) {
            case InputKeyLeft:
                _state->remote_list_pos--;
                if(_state->remote_list_pos == UINT8_MAX)
                    state->remote_list_pos = string_array_size(state->remote_list) - 1;
                break;
            case InputKeyRight:
                _state->remote_list_pos++;
                if(_state->remote_list_pos == string_array_size(state->remote_list))
                    state->remote_list_pos = 0;
                break;
            case InputKeyUp:
                _state->app_list_pos--;
                if(_state->app_list_pos == UINT8_MAX)
                    state->app_list_pos = string_array_size(state->app_list) - 1;
                break;
            case InputKeyDown:
                _state->app_list_pos++;
                if(_state->app_list_pos == string_array_size(state->app_list))
                    state->app_list_pos = 0;
                break;
            case InputKeyOk:
                FURI_LOG_I(TAG, "Pressed OK");
                break;
            case InputKeyBack:
                running = false;
            default:
                break;
            }
        }
        view_port_update(_state->view_port);
        release_mutex(state_mutex, _state);
    }
}
