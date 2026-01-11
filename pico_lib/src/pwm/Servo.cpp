#include "pwm/Servo.hpp"
#include <cstdio>

using namespace Pwm;

Servo::Servo(double angleMin,
             double angleMax,
             uint pulseWidthMicroSMin,
             uint pulseWidthMicroSMax,
             uint gpio,
             uint pwmPeriodMicroS,
             uint pwmDutyPeriodMicroS) : Output(gpio, pwmPeriodMicroS, pwmDutyPeriodMicroS),
                                         m_angleMin(angleMin),
                                         m_angleMax(angleMax),
                                         m_pulseWidthMicroSMin(pulseWidthMicroSMin),
                                         m_pulseWidthMicroSMax(pulseWidthMicroSMax)
{
}

void Servo::setAngle(double angle)
{
    double anglePercent = angle / m_angleMax;
    uint dutyPeriod = m_pulseWidthMicroSMin + anglePercent * (m_pulseWidthMicroSMax - m_pulseWidthMicroSMin);

    printf("duty period: %d\n", dutyPeriod);

    setPwmDutyPeriodMicroS(dutyPeriod);
}