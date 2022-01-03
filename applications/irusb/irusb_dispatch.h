#pragma once

#include <map>
#include "irda.h"

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

typedef std::map<IrdaMessage, uint16_t> IrusbDispatchTable;
#else
typedef struct IrusbDispatchTable IrusbDispatchTable;
#endif

#ifdef __cplusplus
extern "C" {
#endif

IrusbDispatchTable* irusb_dispatch_init(void);
uint16_t irusb_dispatch(const IrusbDispatchTable* dispatch_table,
    const IrdaMessage* msg);
void irusb_dispatch_free(IrusbDispatchTable* dispatch_table);

#ifdef __cplusplus
}
#endif
