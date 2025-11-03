#include <vector>

#include "digital/Output.hpp"
#include "digital/Input.hpp"
#include "analog/Input.hpp"
#include "Macro.hpp"

void setHigh(std::vector<Digital::Output> &ledVector, uint gpio);
void setLow(std::vector<Digital::Output> &ledVector, uint gpio);

int main()
{
    if (cyw43_arch_init())
    {
        printf("Wi-Fi init failed!\n");
        return 1;
    }

    stdio_init_all();
    Digital::Gpio::onboardLedOn();

    MAIN_LOOP_START
    Digital::Input::runLoop();
    Analog::Input::runLoop();
    MAIN_LOOP_END
}

void setHigh(std::vector<Digital::Output> &ledVector, uint gpio)
{
    ledVector.at(gpio).setHigh();
}

void setLow(std::vector<Digital::Output> &ledVector, uint gpio)
{
    ledVector.at(gpio).setLow();
}
