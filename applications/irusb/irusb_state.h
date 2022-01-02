#pragma once

#include <map>
#include <vector>
#include <string>
#include <furi.h>
#include <gui/gui.h>
#include <irda.h>
#include <irda_worker.h>
#include <storage/storage.h>

#define TAG "IRUSB"
#define MOUSE_MOVE_SHORT 5
#define MOUSE_MOVE_LONG 20


namespace std {
    template<>
    struct less<IrdaMessage> {
        bool operator()(const IrdaMessage& m1, const IrdaMessage& m2) const {
            return (m1.protocol < m2.protocol)
                || (m1.protocol == m2.protocol && m1.address < m2.address)
                || (m1.address == m2.address && m1.command < m2.command);
        }
    };
}


typedef struct {
    osMessageQueueId_t event_queue;
    ViewPort* view_port;
    Gui* gui;
    std::map<IrdaMessage, uint16_t> dispatch_table;
    IrdaWorker* worker;

    Storage* storage;
    char irda_text[64];
    std::vector<std::string> app_list, remote_list;
    uint8_t app_list_pos, remote_list_pos;
} IrusbState;


typedef enum {
    EventTypeInput
} EventType;

typedef struct {
    union {
        InputEvent input;
    };
    EventType type;
} IrusbEvent;

void irusb_input_callback(InputEvent* input_event, void* ctx);
void irusb_tick_callback(void* ctx);
void irusb_render_callback(Canvas* canvas, void* ctx);
IrusbState* irusb_init(ValueMutex* state_mutex);
void irusb_free(IrusbState* state);
void irusb_loop(IrusbState* state, ValueMutex* state_mutex);
