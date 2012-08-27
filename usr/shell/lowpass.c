#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <dev/registers.h>
#include <kernel/sched.h>
#include <math.h>
#include <dev/periph/discovery_accel.h>

#include "lowpass.h"

/* INSANE defs to make it obvious where the magic numbers come from */
#define DT_TO_JIFFIES   10000
#define DELTA_T         .01
#define CUTOFF_FREQ     75
#define TAU             1/CUTOFF_FREQ
#define ACC_LP_GAIN     DELTA_T/(TAU+DELTA_T)
#define G(x)            x*.018

typedef struct accel_data {
    int8_t this;
    int8_t x;
    int8_t stuff;
    int8_t y;
    int8_t sucks;
    int8_t z;
} accel_data;

accel_data asdf = {0,0,0,0,0,0};
accel_data *data = &asdf;

void lowpass_test(int argc, char **argv) {
    if (argc != 1) {
        printf("Usage: %s\r\n", argv[0]);
        return;
    }
    printf("This program will continuously print a mathematically correct filtered roll angle.\r\n\
            It is ghetto and CANNOT BE STOPPED WHEN RUN.\r\n\
            q to quit now, any other key will continue.\r\n");
    if(getc() != 'q') {
        printf("LOL TOO LATE NOW FOOL. HOPE YOU GOT YOUR RESET BUTTON HANDY. SCRUB.\r\n");
        new_task(&ghetto_lp, 8, DELTA_T*DT_TO_JIFFIES);
    }
}

void ghetto_lp(void) {
    static float theta = 0;
    static rd_t accelrd = -1;
    if(accelrd == -1) {
        accelrd = open_discovery_accel();
    } 
    read(accelrd, (char *)data, 6);
    theta = lowpass(theta, atan2(G(data->z), G(data->y)), ACC_LP_GAIN);
    printf("Filtered roll: %f\r\n", theta);
}
