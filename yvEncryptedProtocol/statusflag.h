#ifndef STATUSFLAG_H
#define STATUSFLAG_H

namespace StatusFlag {
    enum {
        Acknowledge=1,
        Connect=2,
        Data=4,
        Reset=8
    };
}

#endif // STATUSFLAG_H
