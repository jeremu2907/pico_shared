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
        Output(uint gpio, uint pwmPeriodMicroS, uint pwmDutyPeriodMicroS);

        /// @brief 
        /// @param pwmDutyPeriodMicroS 
        void setPwmDutyPeriodMicroS(uint pwmDutyPeriodMicroS);

    private:
        uint m_sliceNum;
        uint m_channel;
    };
}