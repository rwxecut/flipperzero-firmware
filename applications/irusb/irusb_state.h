#pragma once

#include <map>
#include <vector>
#include <string>
#include <furi.h>
#include <gui/gui.h>
#include <irda.h>
#include <irda_worker.h>
#include <storage/storage.h>


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


IrusbState* irusb_init(ValueMutex* state_mutex);
void irusb_loop(IrusbState* state, ValueMutex* state_mutex);
void irusb_free(IrusbState* state);
