#include <cstdio>

#include "gpio/Base.hpp"

#include "Macros.hpp"

using namespace Gpio;

Base::Base(uint gpio) : m_gpio(gpio)
{
    if (m_sClaimedPinMap.count(gpio) > 0)
    {
        ERR_START
        printf("Error: GPIO %u already claimed!\n", gpio);
        ERR_END
    }

    m_sClaimedPinMap[m_gpio] = true;
}

Base::~Base()
{
    m_sClaimedPinMap.erase(m_gpio);
}

uint Base::gpio() const
{
    return m_gpio;
}
