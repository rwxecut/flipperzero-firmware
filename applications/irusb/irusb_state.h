#pragma once

#include <m-string.h>
#include <m-array.h>
#include <furi.h>
#include <gui/gui.h>
#include <irda.h>
#include <irda_worker.h>
#include <storage/storage.h>
#include "irusb_dispatch.h"

ARRAY_DEF(string_array, string_t, STRING_OPLIST)

typedef struct {
    osMessageQueueId_t event_queue;
    ViewPort* view_port;
    Gui* gui;
    IrusbDispatchTable* dispatch_table;
    IrdaWorker* worker;

    Storage* storage;
    char irda_text[64];
    string_array_t app_list, remote_list;
    uint8_t app_list_pos, remote_list_pos;
} IrusbState;


IrusbState* irusb_init(ValueMutex* state_mutex);
void irusb_loop(IrusbState* state, ValueMutex* state_mutex);
void irusb_free(IrusbState* state);
