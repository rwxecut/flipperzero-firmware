#include "usb_ir_dongle.h"
#include "usb_ir_dongle_state.h"

#define MOUSE_MOVE_SHORT 5
#define MOUSE_MOVE_LONG 20


static void usb_ir_dongle_keystroke(uint16_t button) {
	furi_hal_hid_kb_press(button);
	furi_hal_hid_kb_release(button);
}


static void uid_irda_to_usb(const IrdaMessage *msg) {
	if (msg->protocol == IrdaProtocolNEC && msg->address == 0x00) {
		switch (msg->command) {
			case 0x15: case 0x11:
				usb_ir_dongle_keystroke(KEY_UP_ARROW);
				break;
			case 0x07: case 0x10:
				usb_ir_dongle_keystroke(KEY_DOWN_ARROW);
				break;
			case 0x44:
				usb_ir_dongle_keystroke(KEY_LEFT_ARROW);
				break;
			case 0x40:
				usb_ir_dongle_keystroke(KEY_RIGHT_ARROW);
				break;
			case 0x43:
				usb_ir_dongle_keystroke(KEY_SPACE);
				break;
			default: break;
		}
	} else
	if (msg->protocol == IrdaProtocolSamsung32 && msg->address == 0x07) {
		switch (msg->command) {
			case 0x07:
				usb_ir_dongle_keystroke(KEY_UP_ARROW);
				break;
			case 0x0B:
				usb_ir_dongle_keystroke(KEY_DOWN_ARROW);
				break;
			case 0x45:
				usb_ir_dongle_keystroke(KEY_LEFT_ARROW);
				break;
			case 0x48:
				usb_ir_dongle_keystroke(KEY_RIGHT_ARROW);
				break;
			case 0x4A: case 0x47:
				usb_ir_dongle_keystroke(KEY_SPACE);
				break;
			case 0x10:
				usb_ir_dongle_keystroke(KEY_MOD_LEFT_SHIFT | KEY_P);
				break;
			case 0x12:
				usb_ir_dongle_keystroke(KEY_MOD_LEFT_SHIFT | KEY_N);
				break;
			case 0x0F:
				usb_ir_dongle_keystroke(KEY_M);
				break;
			case 0x60:
				furi_hal_hid_mouse_move(0, msg->repeat ? -MOUSE_MOVE_LONG : -MOUSE_MOVE_SHORT);
				break;
			case 0x61:
				furi_hal_hid_mouse_move(0, msg->repeat ? MOUSE_MOVE_LONG : MOUSE_MOVE_SHORT);
				break;
			case 0x65:
				furi_hal_hid_mouse_move(msg->repeat ? -MOUSE_MOVE_LONG : -MOUSE_MOVE_SHORT, 0);
				break;
			case 0x62:
				furi_hal_hid_mouse_move(msg->repeat ? MOUSE_MOVE_LONG : MOUSE_MOVE_SHORT, 0);
				break;
			case 0x68:
				furi_hal_hid_mouse_press(HID_MOUSE_BTN_LEFT);
				furi_hal_hid_mouse_release(HID_MOUSE_BTN_LEFT);
				break;
			case 0x1F:
				furi_hal_hid_mouse_press(HID_MOUSE_BTN_RIGHT);
				furi_hal_hid_mouse_release(HID_MOUSE_BTN_RIGHT);
				break;
			default: break;
		}
	}
}


void usb_ir_dongle_input_callback(InputEvent* input_event, void* ctx) {
	furi_assert(ctx);
	UIDState *state = (UIDState*)ctx;
	UIDEvent event = {.input = *input_event, .type = EventTypeInput};
	osMessageQueuePut(state->event_queue, &event, 0, osWaitForever);
}


void usb_ir_dongle_render_callback(Canvas* canvas, void* ctx) {
	furi_assert(ctx);
	UIDState *state = (UIDState*)acquire_mutex((ValueMutex*)ctx, 25);
	if (!state) return;

	const char *app_text = state->app_list.size() > 0 ? state->app_list[state->app_list_pos].c_str() : "Empty";
	const char *remote_text = state->remote_list.size() > 0 ? state->remote_list[state->remote_list_pos].c_str() : "Empty";
	canvas_set_font(canvas, FontPrimary);
	canvas_draw_str(canvas, 0, 10, "IR USB Dongle Bongle");
	canvas_set_font(canvas, FontSecondary);
	canvas_draw_str(canvas, 0, 19, "U/D App, L/R Remote");
	canvas_draw_str(canvas, 0, 28, app_text);
	canvas_draw_str(canvas, 0, 37, remote_text);
	canvas_draw_str(canvas, 0, 46, "Last IRDA Signal received:");
	canvas_draw_str(canvas, 0, 55, state->irda_text[0] ? state->irda_text : "None");

	release_mutex((ValueMutex*)ctx, state);
}


void usb_ir_dongle_signal_received_callback(void* ctx, IrdaWorkerSignal* sig)
{
	furi_assert(ctx);
	furi_assert(sig);
	UIDState *state = (UIDState*)ctx;
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
		uid_irda_to_usb(message);
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


UIDState* usb_ir_dongle_init(ValueMutex* state_mutex) {
	UIDState* state = (UIDState*)furi_alloc(sizeof(UIDState));

	state->worker = irda_worker_alloc();
	irda_worker_rx_start(state->worker);
	irda_worker_rx_set_received_signal_callback(state->worker, usb_ir_dongle_signal_received_callback, state);
	irda_worker_rx_enable_blink_on_receiving(state->worker, true);

	state->storage = (Storage*)furi_record_open("storage");
    const uint8_t filename_size = 100;
    char* filename = (char*)furi_alloc(filename_size);
    FileInfo fileinfo;
    File* assets_dir = storage_file_alloc(state->storage);
    int result = 0;

	storage_dir_open(assets_dir, "/ext/usb_ir_dongle/remotes");
    do {
        result = storage_dir_read(assets_dir, &fileinfo, filename, filename_size);
        if(result) {
			state->remote_list.push_back(filename);
        }
    } while(result);
    storage_dir_close(assets_dir);
	storage_dir_open(assets_dir, "/ext/usb_ir_dongle/apps");
    do {
        result = storage_dir_read(assets_dir, &fileinfo, filename, filename_size);
        if(result) {
			state->app_list.push_back(filename);
        }
    } while(result);
    storage_dir_close(assets_dir);
	state->app_list_pos = state->remote_list_pos = 0;

	state->event_queue = osMessageQueueNew(8, sizeof(UIDEvent), NULL);
	furi_check(state->event_queue);

	state->view_port = view_port_alloc();
	view_port_draw_callback_set(state->view_port, usb_ir_dongle_render_callback, state_mutex);
	view_port_input_callback_set(state->view_port, usb_ir_dongle_input_callback, state);
	state->gui = (Gui*)furi_record_open("gui");
	gui_add_view_port(state->gui, state->view_port, GuiLayerFullscreen);

	return state;
}


void usb_ir_dongle_free(UIDState* state) {
	view_port_enabled_set(state->view_port, false);
	gui_remove_view_port(state->gui, state->view_port);
	view_port_free(state->view_port);
	furi_record_close("gui");
	furi_record_close("storage");
	osMessageQueueDelete(state->event_queue);
	irda_worker_rx_stop(state->worker);
	irda_worker_free(state->worker);
	free(state);
}


void usb_ir_dongle_loop(UIDState* state, ValueMutex* state_mutex) {
	UIDEvent event;
	for (bool running = true; running;) {
		osStatus_t event_status = osMessageQueueGet(state->event_queue, &event, NULL, 100);
		UIDState *_state = (UIDState*)acquire_mutex_block(state_mutex);
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
