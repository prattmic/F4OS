#include <kernel/sched.h>

#include "sensors.h"
#include "ui.h"

void main(void) {
    /* Initializes sensor resources, which the read_sensor task will inherit. */
    new_task(&init_sensors, 5, 0);

    /* Main ui.  Won't be able to run until init_sensors is done */
    new_task(&ui, 1, 0);
}
