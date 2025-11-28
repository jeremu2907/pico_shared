#include <vector>
#include <cstring>

#include "gpio/Output.hpp"
#include "gpio/Input.hpp"
#include "adc/Input.hpp"
#include "i2c/Ssd1306.h"
#include "Macro.hpp"

int main()
{
#if PICO_BOARD_TYPE == PICO_W
    if (cyw43_arch_init())
    {
        printf("Wi-Fi init failed!\n");
        return 1;
    }
#endif

    stdio_init_all();
    Gpio::Base::onboardLedOn();

    // size_t size = SCREEN_WIDTH * (SCREEN_HEIGHT / 8);
    // uint8_t screen[size];
    // memset(screen, 255, size);
    // draw_circle(64, 32, 20, screen);
    
    I2c::Ssd1306 oled;
    uint8_t screenData[8 * 128] = {0};
    for(size_t page = 0; page < 8; page *= 2)
    {
        size_t start = 128 * page;
        size_t end = start + 128;
        for(size_t segment = 128 * page; segment < end ; segment++)
        {
            screenData[segment] = 0xFF;
        }
    }

    while(true)
    {
        printf("write size %d", oled.writeScreen(screenData, 8 * 128));
    }

    MAIN_LOOP_START
    Gpio::Input::runLoop();
    Adc::Input::runLoop();
    MAIN_LOOP_END
}
