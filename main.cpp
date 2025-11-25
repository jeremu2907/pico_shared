#include <vector>
#include <cstring>

#include "gpio/Output.hpp"
#include "gpio/Input.hpp"
#include "adc/Input.hpp"
#include "i2c/Ssd1306.h"
#include "Macro.hpp"

void setHigh(std::vector<Gpio::Output> &ledVector, uint gpio);
void setLow(std::vector<Gpio::Output> &ledVector, uint gpio);
void draw_circle(int cx, int cy, int r, uint8_t screen[]);

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
    // oled.writeBlocking(screen, size);
    // while(true)
    // {
    //     printf("write size %d", oled.writeBlocking(screen, size));
    // }

    MAIN_LOOP_START
    Gpio::Input::runLoop();
    Adc::Input::runLoop();
    MAIN_LOOP_END
}

void setHigh(std::vector<Gpio::Output> &ledVector, uint gpio)
{
    ledVector.at(gpio).setHigh();
}

void setLow(std::vector<Gpio::Output> &ledVector, uint gpio)
{
    ledVector.at(gpio).setLow();
}
