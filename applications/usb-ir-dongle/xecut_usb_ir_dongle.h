#pragma once

#include <furi.h>
#include <furi-hal.h>
#include <gui/gui.h>
#include <input/input.h>

#define TAG "XECUT_USB_IR_DONGLE"


typedef struct {
	osMessageQueueId_t event_queue;
	ViewPort* view_port;
	Gui* gui;
	osTimerId_t timer;
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
