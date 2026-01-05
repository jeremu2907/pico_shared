#include "gpio/Output.hpp"

using namespace Gpio;

Output::Output(uint gpio): Base(gpio)
{
    init();
}

void Output::init()
{
    gpio_init(m_gpio);
    gpio_set_dir(m_gpio, GPIO_OUT);
}

void Output::setHigh()
{
    put(true);
}

void Output::setLow()
{
    put(false);
}

void Output::put(bool high)
{
    gpio_put(m_gpio, high);
}
