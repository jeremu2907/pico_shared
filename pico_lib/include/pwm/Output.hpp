#pragma once

#include "gpio/Base.hpp"

namespace Pwm
{
    class Output : public Gpio::Base
    {
    public:
        Output(uint gpio, uint pwmPeriodMicroS, uint pwmDutyPeriodMicroS);

        void setPwmDutyPeriodMicroS(uint pwmDutyPeriodMicroS);

    private:
        uint m_sliceNum;
        uint m_channel;
    };
}