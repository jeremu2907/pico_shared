#include "adc/Input.hpp"

using namespace Adc;

Input::Input(uint gpio, uint adcIdx) : m_gpio(gpio),
                                       m_adcIdx(adcIdx)
{
    if(!s_init)
    {
        s_init = true;
        adc_init();
    }

    if(m_gpio < 26 || m_gpio > 29)
    {
        ERR_START
        printf("Error: GPIO %u must be [26, 29]\n", gpio);
        ERR_END
    }

    if (s_claimedPinMap.count(gpio) > 0)
    { 
        ERR_START
        printf("Error: GPIO %u already claimed!\n", gpio);
        ERR_END
    }
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
    s_claimedPinMap[m_gpio] = true;
    adc_gpio_init(m_gpio);
}

void Input::runLoop()
{
    if(!s_running) return;
    for (auto &input : s_inputQueue)
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
