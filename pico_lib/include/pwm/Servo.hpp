#pragma once

#include "Output.hpp"

namespace Pwm
{
    class Servo : public Output
    {
    public:
        /// @brief
        /// @param gpio
        /// @param pwmPeriodMicroS
        /// @param pwmDutyPeriodMicroS
        /// @param angleMin
        /// @param angleMax
        /// @param
        Servo(double angleMin,
              double angleMax,
              uint pulseWidthMicroSMin,
              uint pulseWidthMicroSMax,
              uint gpio,
              uint pwmPeriodMicroS,
              uint pwmDutyPeriodMicroS = 0);

        /// @brief
        /// @param angle
        void setAngle(double angle);

    private:
        double m_angleMin;
        double m_angleMax;
        double m_pulseWidthMicroSMin;
        double m_pulseWidthMicroSMax;
    };
}