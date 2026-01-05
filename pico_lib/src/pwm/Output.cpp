#include "pico/stdlib.h"
#include "hardware/pwm.h"

#include "pwm/Output.hpp"

using namespace Pwm;

Output::Output(uint gpio, uint pwmPeriodMicroS, uint pwmDutyPeriodMicroS): Gpio::Base(gpio)
{
    gpio_set_function(gpio, GPIO_FUNC_PWM);
    m_sliceNum = pwm_gpio_to_slice_num(gpio);
    m_channel = pwm_gpio_to_channel(gpio);

    pwm_config config = pwm_get_default_config();

    // 125 sys clk cycles per microsecond
    constexpr float SYS_CLK = 125e6f;
    constexpr float MICROSECONDS_PER_SECONDS = 1e6f;
    constexpr float CLK_DIVISION = SYS_CLK / MICROSECONDS_PER_SECONDS;

    pwm_config_set_clkdiv(&config, CLK_DIVISION);
    pwm_config_set_wrap(&config, pwmPeriodMicroS);
    pwm_init(m_sliceNum, &config, true);

    setPwmDutyPeriodMicroS(pwmDutyPeriodMicroS);
}

void Output::setPwmDutyPeriodMicroS(uint pwmDutyPeriodMicroS)
{
    pwm_set_chan_level(m_sliceNum, m_channel, pwmDutyPeriodMicroS);
}
