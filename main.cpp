#include <vector>

#include "gpio/Output.hpp"
#include "gpio/Input.hpp"
#include "adc/Input.hpp"
#include "Macro.hpp"

void setHigh(std::vector<Gpio::Output> &ledVector, uint gpio);
void setLow(std::vector<Gpio::Output> &ledVector, uint gpio);

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
