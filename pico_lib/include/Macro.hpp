#pragma once

#define PICO 0
#define PICO_W 1
#define PICO_2 2
#define PICO_2_W 3

#define ERR_START while (true) { sleep_ms(1000);
#define ERR_END tight_loop_contents(); }

#define MAIN_LOOP_START while (true) {
#define MAIN_LOOP_END }

#define MAX_VOLTAGE 3.3f
#define MID_VOLTAGE 3.3 / 2.0f
#define MID_VOLTAGE_HI MID_VOLTAGE + 0.3f
#define MID_VOLTAGE_LO MID_VOLTAGE - 0.3f

