#include "adc/Input.hpp"

using namespace Adc;

Input::Input(uint gpio, uint adcIdx) : m_gpio(gpio),
                                       m_adcIdx(adcIdx)
{
    if(!m_sInit)
    {
        m_sInit = true;
        adc_init();
    }

    if(m_gpio < 26 || m_gpio > 29)
    {
        ERR_START
        printf("Error: GPIO %u must be [26, 29]\n", gpio);
        ERR_END
    }

    if (m_sClaimedPinMap.count(gpio) > 0)
    { 
        ERR_START
        printf("Error: GPIO %u already claimed!\n", gpio);
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

void Input::init()
{
    m_sClaimedPinMap[m_gpio] = true;
    adc_gpio_init(m_gpio);
}

void Input::runLoop()
{
    if(!m_sRunning) return;
    for (auto &input : m_sInputQueue)
    {
        adc_select_input(input->m_adcIdx);
        sleep_ms(10);
        auto raw = adc_read();
        float voltage = (raw / PICO_2_W_ADC_RESOLUTION) * MAX_VOLTAGE;
        input->callback(voltage);
    }
}

void Input::callback(float voltage)
{
    m_callback(voltage);
}
