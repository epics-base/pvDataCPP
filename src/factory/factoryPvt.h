#ifndef FACTORYPVT_H
#define FACTORYPVT_H

namespace epics { namespace pvData {

// use to bound recursion in FieldCreate and PVDataCreator singletons
struct RecurseGuard {
    RecurseGuard();
    ~RecurseGuard();
};

}} // namespace epics::pvData

#endif // FACTORYPVT_H
