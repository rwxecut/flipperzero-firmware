#include <furi.h>
#include <gui/gui.h>
#include <input/input.h>

#define TAG "XECUT_HELLO"

typedef struct {
    bool press[5];
    uint16_t up;
    uint16_t down;
    uint16_t left;
    uint16_t right;
    uint16_t ok;
} XecutHelloState;

typedef enum {
    EventTypeInput,
} EventType;

typedef struct {
    union {
        InputEvent input;
    };
    EventType type;
} XecutHelloEvent;

static void xecut_hello_render_callback(Canvas* canvas, void* ctx) {
	XecutHelloState* state = (XecutHelloState*)acquire_mutex((ValueMutex*)ctx, 25);
	canvas_clear(canvas);

	canvas_set_font(canvas, FontPrimary);
	canvas_draw_str(canvas, 0, 10, "XECUT: AIRWAVES");
	canvas_draw_str(canvas, 0, 19, "DON'T");
	canvas_draw_str(canvas, 0, 28, "LIE");

	release_mutex((ValueMutex*)ctx, state);
}

int32_t xecut_hello_app(void* p) {
	osMessageQueueId_t event_queue = osMessageQueueNew(32, sizeof(XecutHelloState), NULL);
	furi_check(event_queue);

	XecutHelloState _state = {{false, false, false, false, false}, 0, 0, 0, 0, 0};

	ValueMutex state_mutex;
	if(!init_mutex(&state_mutex, &_state, sizeof(XecutHelloState))) {
		FURI_LOG_E(TAG, "cannot create mutex");
		return 0;
	}

	ViewPort* view_port = view_port_alloc();

	view_port_draw_callback_set(view_port, xecut_hello_render_callback, &state_mutex);

	Gui* gui = furi_record_open("gui");
	gui_add_view_port(gui, view_port, GuiLayerFullscreen);

		XecutHelloEvent event;
    while(1) {
        osStatus_t event_status = osMessageQueueGet(event_queue, &event, NULL, osWaitForever);
        XecutHelloState* state = (XecutHelloState*)acquire_mutex_block(&state_mutex);
        if(event_status == osOK) {
        }
        acquire_mutex_block(&state_mutex);
        view_port_update(view_port);
        release_mutex(&state_mutex, state);
    }
    // remove & free all stuff created by app
    gui_remove_view_port(gui, view_port);
    view_port_free(view_port);
    osMessageQueueDelete(event_queue);
    delete_mutex(&state_mutex);
    return 0;
}
