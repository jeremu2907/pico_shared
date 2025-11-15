#include "gpio/Input.hpp"

using namespace Gpio;

Input::Input(uint gpio) : Base(gpio)
{
    init();
    s_inputQueue.emplace_back(this);
}

Input::~Input()
{
    s_running = false;
    s_inputQueue.clear();
}

void Input::init()
{
    Base::init();
    gpio_set_dir(m_gpio, false);
    gpio_pull_up(m_gpio);
}

void Input::runLoop()
{
    if(!s_running) return;
    for (auto &input : s_inputQueue)
    {
        // Active - Low invert logic by Pull-Up Resistor
        input->m_high = gpio_get(input->m_gpio);
        if (input->m_high)
        {
            input->callbackLow();
        }
        else
        {
            input->callbackHigh();
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