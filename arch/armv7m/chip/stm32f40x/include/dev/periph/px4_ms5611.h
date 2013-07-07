#ifndef DEV_PERIPH_PX4_MS5611_H_INCLUDED
#define DEV_PERIPH_PX4_MS5611_H_INCLUDED

rd_t open_px4_ms5611(void);
int read_px4_ms5611(rd_t rd, struct barometer *baro);

#endif
