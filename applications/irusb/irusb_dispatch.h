#pragma once

#ifdef __cplusplus
#include <map>
#endif

#include "irda.h"

typedef enum {
    IrusbHidReportNone = 0,
    IrusbHidReportKb,
    IrusbHidReportMouseMove,
    IrusbHidReportMouseClick,
} IrusbHidReportType;

typedef struct {
    IrusbHidReportType type;
    union {
        uint16_t kb_keycode;
        uint8_t mouse_click_button;
        struct {
            int8_t dx;
            int8_t dy;
        } mouse_move;
    };
} IrusbHidReport;

#ifdef __cplusplus

namespace std {
template <> struct less<IrdaMessage> {
    bool operator()(const IrdaMessage& m1, const IrdaMessage& m2) const {
        return (m1.protocol < m2.protocol) ||
               (m1.protocol == m2.protocol && m1.address < m2.address) ||
               (m1.address == m2.address && m1.command < m2.command) ||
               (m1.command == m2.command && (!m1.repeat && m2.repeat));
    }
};
}

typedef std::map<IrdaMessage, IrusbHidReport> IrusbDispatchTable;
#else
typedef struct IrusbDispatchTable IrusbDispatchTable;
#endif

#ifdef __cplusplus
extern "C" {
#endif

IrusbDispatchTable* irusb_dispatch_init(void);
IrusbHidReport irusb_dispatch(const IrusbDispatchTable* dispatch_table, const IrdaMessage* msg);
void irusb_dispatch_free(IrusbDispatchTable* dispatch_table);

#ifdef __cplusplus
}
#endif
