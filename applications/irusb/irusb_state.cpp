#include <map>
#include <vector>
#include <string>
#include <furi.h>
#include <furi-hal.h>
#include <gui/gui.h>
#include <input/input.h>
#include <irda.h>
#include <irda_worker.h>
#include <storage/storage.h>
#include "irusb_config.h"
#include "irusb_state.h"
#include "irusb_dispatch.h"

typedef enum {
    EventTypeInput
} EventType;

typedef struct {
    union {
        InputEvent input;
    };
    EventType type;
} IrusbEvent;

static void irusb_do_action(IrusbAction action) {
	switch(action.type) {
		case IrusbActionKb:
			furi_hal_hid_kb_press(action.kb_keycode);
			furi_hal_hid_kb_release(action.kb_keycode);
			break;
		case IrusbActionMouseMove:
			// TODO: handle repeat and long/short mouse movements in dispatch
			furi_hal_hid_mouse_move(action.mouse_move.dx, action.mouse_move.dy);
			break;
		case IrusbActionMouseClick:
			furi_hal_hid_mouse_press(action.mouse_click_button);
			furi_hal_hid_mouse_release(action.mouse_click_button);
			break;
		case IrusbActionNone:
		default:
			break;
	}
}


static void irusb_input_callback(InputEvent* input_event, void* ctx) {
	furi_assert(ctx);
	IrusbState *state = (IrusbState*)ctx;
	IrusbEvent event = {.input = *input_event, .type = EventTypeInput};
	osMessageQueuePut(state->event_queue, &event, 0, osWaitForever);
}


static void irusb_render_callback(Canvas* canvas, void* ctx) {
	furi_assert(ctx);
	IrusbState *state = (IrusbState*)acquire_mutex((ValueMutex*)ctx, 25);
	if (!state) return;

	const char *app_text = state->app_list.size() > 0 ? state->app_list[state->app_list_pos].c_str() : "Empty";
	const char *remote_text = state->remote_list.size() > 0 ? state->remote_list[state->remote_list_pos].c_str() : "Empty";
	canvas_draw_icon(canvas, 0, 0, &I_irusb);
	canvas_set_font(canvas, FontSecondary);
	//canvas_draw_str(canvas, 0, 37, "U/D App, L/R Remote");
	canvas_draw_str(canvas, 15, 34, app_text);
	canvas_draw_str(canvas, 15, 49, remote_text);
	//canvas_draw_str(canvas, 0, 46, "Last IRDA Signal received:");
	canvas_draw_str(canvas, 15, 19, state->irda_text[0] ? state->irda_text : "None");

	release_mutex((ValueMutex*)ctx, state);
}


static void irusb_signal_received_callback(void* ctx, IrdaWorkerSignal* sig)
{
	furi_assert(ctx);
	furi_assert(sig);
	IrusbState *state = (IrusbState*)ctx;
	if (irda_worker_signal_is_decoded(sig)) {
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
				message->repeat ? " R\r\n" : "\r\n");
		IrusbAction action = irusb_dispatch(state->dispatch_table, message);
		irusb_do_action(action);
	} else {
		const uint32_t* timings;
		size_t timings_cnt;
		irda_worker_get_raw_signal(sig, &timings, &timings_cnt);
		snprintf(
				state->irda_text,
				sizeof(state->irda_text),
				"RAW %d samples\r\n",
				timings_cnt);
	}
	view_port_update(state->view_port);
	printf("Received IRDA signal %s\n", state->irda_text);
}


IrusbState* irusb_init(ValueMutex* state_mutex) {
	IrusbState* state = (IrusbState*)furi_alloc(sizeof(IrusbState));

	state->worker = irda_worker_alloc();
	irda_worker_rx_start(state->worker);
	irda_worker_rx_set_received_signal_callback(state->worker, irusb_signal_received_callback, state);
	irda_worker_rx_enable_blink_on_receiving(state->worker, true);

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
			state->remote_list.push_back(filename);
        }
    } while(result);
    storage_dir_close(assets_dir);
	storage_dir_open(assets_dir, "/ext/irusb/apps");
    do {
        result = storage_dir_read(assets_dir, &fileinfo, filename, filename_size);
        if(result) {
			state->app_list.push_back(filename);
        }
    } while(result);
    storage_dir_close(assets_dir);
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
	irusb_dispatch_free(state->dispatch_table);
	osMessageQueueDelete(state->event_queue);
	irda_worker_rx_stop(state->worker);
	irda_worker_free(state->worker);
	free(state);
}


void irusb_loop(IrusbState* state, ValueMutex* state_mutex) {
	IrusbEvent event;
	for (bool running = true; running;) {
		osStatus_t event_status = osMessageQueueGet(state->event_queue, &event, NULL, 100);
		IrusbState *_state = (IrusbState*)acquire_mutex_block(state_mutex);
		if (event_status == osOK && event.type == EventTypeInput && event.input.type == InputTypePress) {
			switch (event.input.key) {
				case InputKeyLeft:
					_state->remote_list_pos--;
					if (_state->remote_list_pos == UINT8_MAX)
						state->remote_list_pos = state->remote_list.size() - 1;
					break;
				case InputKeyRight:
					_state->remote_list_pos++;
					if (_state->remote_list_pos == state->remote_list.size())
						state->remote_list_pos = 0;
					break;
				case InputKeyUp:
					_state->app_list_pos--;
					if (_state->app_list_pos == UINT8_MAX)
						state->app_list_pos = state->app_list.size() - 1;
					break;
				case InputKeyDown:
					_state->app_list_pos++;
					if (_state->app_list_pos == state->app_list.size())
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
