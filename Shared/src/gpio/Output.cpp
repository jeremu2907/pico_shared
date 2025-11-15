#include "gpio/Output.hpp"

using namespace Gpio;

Output::Output(uint gpio): Base(gpio)
{
    init();
}

void Output::init()
{
    Base::init();
    gpio_set_dir(m_gpio, true);
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
    m_high = high;
    gpio_put(m_gpio, m_high);
}
