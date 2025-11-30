#pragma once

#define PICO 0
#define PICO_W 1
#define PICO_2 2
#define PICO_2_W 3
#define IS_WIRELESS PICO_BOARD_TYPE == PICO_W || PICO_BOARD_TYPE == PICO_2_W

#define ERR_START while (true) { sleep_ms(1000);
#define ERR_END tight_loop_contents(); }

#define MAIN_LOOP_START while (true) {
#define MAIN_LOOP_END }
