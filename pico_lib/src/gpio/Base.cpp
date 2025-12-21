#include "gpio/Base.hpp"

using namespace Gpio;

Base::Base(uint gpio) : m_gpio(gpio), m_high(false)
{
    if (s_claimedPinMap.count(gpio) > 0)
    {
        ERR_START
        printf("Error: GPIO %u already claimed!\n", gpio);
        ERR_END
    }
    init();
}

Base::~Base()
{
    s_claimedPinMap.erase(m_gpio);
}

uint Base::gpio() const
{
    return m_gpio;
}
void Base::init()
{
    s_claimedPinMap[m_gpio] = true;
    gpio_init(m_gpio);
}

void Base::onboardLedOn()
{
    const uint LED_PIN = 25;

    gpio_init(LED_PIN);
    gpio_set_dir(LED_PIN, GPIO_OUT);
    gpio_put(LED_PIN, 1);
}

void Base::onboardLedOff()
{
    const uint LED_PIN = 25;

    gpio_put(LED_PIN, 0);
}