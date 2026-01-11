#pragma once

#include "gpio/Base.hpp"

namespace Pwm
{
    class Output : public Gpio::Base
    {
    public:
        /// @brief
        /// @param gpio
        /// @param pwmPeriodMicroS
        /// @param pwmDutyPeriodMicroS
        Output(uint gpio, uint pwmPeriodMicroS, uint pwmDutyPeriodMicroS = 0);

        /// @brief
        /// @param pwmDutyPeriodMicroS
        void setPwmDutyPeriodMicroS(uint pwmDutyPeriodMicroS);

        /// @brief
        /// @param dutyCycle in percent (ie dutyCycle = 1 = 1%)
        /// @return Duty period in microseconds
        uint calculateDutyPeriodMicroSFromDutyCycle(uint dutyCycle);

    private:
        uint m_channel;
        uint m_sliceNum;
        uint m_pwmPeriodMicroS;
    };
}