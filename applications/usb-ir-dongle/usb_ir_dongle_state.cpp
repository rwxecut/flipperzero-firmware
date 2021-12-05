#include "usb_ir_dongle.h"
#include "usb_ir_dongle_state.h"


static void usb_ir_dongle_keystroke(uint16_t button) {
	furi_hal_hid_kb_press(button);
	furi_hal_hid_kb_release(button);
}


static void uid_irda_to_usb(IrdaProtocol prot, uint32_t addr, uint32_t cmd) {
	if (prot == IrdaProtocolNEC && addr == 0x00) {
		switch (cmd) {
			case 0x15:
				usb_ir_dongle_keystroke(KEY_UP_ARROW);
				break;
			case 0x07:
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

	canvas_set_font(canvas, FontSecondary);
	canvas_draw_str(canvas, 0, 10, state->display_text[0] ? state->display_text : "USB IR Dongle Bongle");

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
				state->display_text,
				sizeof(state->display_text),
				"%s A:0x%0*lX C:0x%0*lX %s",
				irda_get_protocol_name(message->protocol),
				ROUND_UP_TO(irda_get_protocol_address_length(message->protocol), 4),
				message->address,
				ROUND_UP_TO(irda_get_protocol_command_length(message->protocol), 4),
				message->command,
				message->repeat ? " R\r\n" : "\r\n");
		uid_irda_to_usb(message->protocol, message->address, message->command);
	} else {
		const uint32_t* timings;
		size_t timings_cnt;
		irda_worker_get_raw_signal(sig, &timings, &timings_cnt);
		snprintf(
				state->display_text,
				sizeof(state->display_text),
				"RAW %d samples\r\n",
				timings_cnt);
	}
	view_port_update(state->view_port);
	printf("%s\r\n", state->display_text);
}


UIDState* usb_ir_dongle_init(ValueMutex* state_mutex) {
	UIDState* state = (UIDState*)furi_alloc(sizeof(UIDState));
	state->worker = irda_worker_alloc();
	irda_worker_rx_start(state->worker);
	irda_worker_rx_set_received_signal_callback(state->worker, usb_ir_dongle_signal_received_callback, state);
	irda_worker_rx_enable_blink_on_receiving(state->worker, true);
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
					FURI_LOG_I(TAG, "Pressed Left");
					break;
				case InputKeyRight:
					FURI_LOG_I(TAG, "Pressed Right");
					break;
				case InputKeyUp:
					FURI_LOG_I(TAG, "Pressed Up");
					break;
				case InputKeyDown:
					FURI_LOG_I(TAG, "Pressed Down");
					// TODO: next app
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
