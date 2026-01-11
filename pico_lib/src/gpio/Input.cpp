#include "gpio/Input.hpp"

using namespace Gpio;

Input::Input(uint gpio) : Base(gpio)
{
    init();
    m_sInputQueue.emplace_back(this);
}

Input::~Input()
{
    m_sRunning = false;
    m_sInputQueue.clear();
}

void Input::init()
{
    gpio_init(m_gpio);
    gpio_set_dir(m_gpio, GPIO_IN);
    gpio_pull_up(m_gpio);
}

void Input::runLoop()
{
    if(!m_sRunning) return;
    for (auto &input : m_sInputQueue)
    {
        bool high = gpio_get(input->m_gpio);
        if (high)
        {
            input->callbackHigh();
        }
        else
        {
            input->callbackLow();
        }
    }
}

void Input::callbackHigh()
{
    m_callbackHigh();
}

void Input::callbackLow()
{
    m_callbackLow();
}