#ifndef INCLUDE_DEV_DEVICE_H_INCLUDED
#define INCLUDE_DEV_DEVICE_H_INCLUDED

/*
 * A device is a physical device connected to the system
 * through a physical parent.  All external communications
 * go through this parent.
 */

struct device {
    struct obj  *parent;
};

#endif
