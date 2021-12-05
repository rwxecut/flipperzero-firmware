#pragma once

#include <furi.h>
#include <furi-hal.h>
#include <gui/gui.h>
#include <input/input.h>
#include <irda.h>
#include <irda_worker.h>
#include <map>
#include <memory>


#define TAG "USB_IR_DONGLE"

namespace std {
template<>
struct less<IrdaMessage>
{
   bool operator()(const IrdaMessage& m1, const IrdaMessage& m2) const
   {
   		return (m1.protocol < m2.protocol)
   			|| (m1.protocol == m2.protocol && m1.address < m2.address)
   			|| (m1.address == m2.address && m1.command < m2.command)
   			|| (m1.command == m2.command && m1.repeat < m2.repeat);
   }
};
}


typedef struct {
	char display_text[64];
	osMessageQueueId_t event_queue;
	ViewPort* view_port;
	Gui* gui;
	std::map<IrdaMessage, uint16_t> dispatch_table;
	IrdaWorker* worker;
} UIDState;


typedef enum {
	EventTypeTick,
	EventTypeInput
} EventType;

typedef struct {
	union {
		InputEvent input;
	};
	EventType type;
} UIDEvent;
