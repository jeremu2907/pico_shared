#include <vector>
#include <cstring>

#include "gpio/Output.hpp"
#include "gpio/Input.hpp"
#include "adc/Input.hpp"
#include "i2c/Ssd1306.h"
#include "i2c/Font8x8.h"
#include "Macros.hpp"

int main()
{
#if IS_WIRELESS
    if (cyw43_arch_init())
    {
        printf("Wi-Fi init failed!\n");
        return 1;
    }
#endif

    stdio_init_all();
    Gpio::Base::onboardLedOn();
    
    I2c::Ssd1306 oled(20, 21, i2c0);

    MAIN_LOOP_START
    oled.clearData();
    oled.setData("\nHello World!\nTest newline...\n\nBye bye...");
    oled.writeData();
    sleep_ms(3000);
    oled.clearData();
    oled.setData("\nThis line might\n\nactually be\n\nlong, so please\n\nbe nice to it!");
    oled.writeData();
    sleep_ms(3000);
    Gpio::Input::runLoop();
    Adc::Input::runLoop();
    MAIN_LOOP_END
}
