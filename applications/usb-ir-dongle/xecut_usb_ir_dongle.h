#pragma once

#include <furi.h>
#include <furi-hal.h>
#include <gui/gui.h>
#include <input/input.h>
#include <irda.h>
#include <irda_worker.h>


#define TAG "XECUT_USB_IR_DONGLE"


typedef struct {
	char display_text[64];
	osMessageQueueId_t event_queue;
	ViewPort* view_port;
	Gui* gui;
	IrdaWorker* worker;
} XecutUIDState;

typedef enum {
	EventTypeTick,
	EventTypeInput
} EventType;

typedef struct {
	union {
		InputEvent input;
	};
	EventType type;
} XecutUIDEvent;
