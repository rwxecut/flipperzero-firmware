#pragma once

#ifdef __cplusplus
#include <map>
#endif

#include "irda.h"

typedef enum {
    IrusbActionNone = 0,
    IrusbActionKb,
    IrusbActionMouseMove,
    IrusbActionMouseClick,
} IrusbActionType;

typedef struct {
    IrusbActionType type;
    union {
        uint16_t kb_keycode;
        uint8_t mouse_click_button;
        struct {
            int8_t dx;
            int8_t dy;
        } mouse_move;
    };
} IrusbAction;

#ifdef __cplusplus

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

typedef std::map<IrdaMessage, IrusbAction> IrusbDispatchTable;
#else
typedef struct IrusbDispatchTable IrusbDispatchTable;
#endif

#ifdef __cplusplus
extern "C" {
#endif

IrusbDispatchTable* irusb_dispatch_init(void);
IrusbAction irusb_dispatch(const IrusbDispatchTable* dispatch_table,
    const IrdaMessage* msg);
void irusb_dispatch_free(IrusbDispatchTable* dispatch_table);

#ifdef __cplusplus
}
#endif
