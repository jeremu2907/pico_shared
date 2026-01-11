#include <cstdio>

#include "hardware/adc.h"
#include "Macros.hpp"
#include "Constants.hpp"

#include "adc/Input.hpp"

using namespace Adc;

Input::Input(uint gpio) : Gpio::Base(gpio)
{
    if (!m_sInit)
    {
        m_sInit = true;
        adc_init();
    }

    if (getAdcIndex() == -1)
    {
        ERR_START
        printf("Error: GPIO %u must be [26, 29]\n", gpio);
        ERR_END
    }

    init();
    m_sInputQueue.emplace_back(this);
}

Input::~Input()
{
    m_sRunning = false;
    m_sInputQueue.clear();
}

uint Input::getAdcIndex()
{
    switch (m_gpio)
    {
    case 26:
        return 0;
    case 27:
        return 1;
    case 28:
        return 2;
    case 29:
        return 3;
    default:
        return -1;
    }
}

void Input::init()
{
    adc_gpio_init(m_gpio);
}

void Input::runLoop()
{
    if (!m_sRunning)
        return;
    for (auto &input : m_sInputQueue)
    {
        uint idx = input->getAdcIndex();
        if (idx == -1)
        {
            continue;
        }
        adc_select_input(idx);
        sleep_ms(10);
        auto raw = adc_read();
        float voltage = (raw / ADC_RESOLUTION) * MAX_VOLTAGE;
        input->callback(voltage);
    }
}

void Input::callback(float voltage)
{
    m_callback(voltage);
}
