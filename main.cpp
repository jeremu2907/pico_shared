#include "pico/stdlib.h"

#include "wifi/ApScan.hpp"
#include "gpio/Output.hpp"
#include "pwm/Servo.hpp"
#include "pwm/Output.hpp"
#include "Macros.hpp"

#define SSID "_____jeremy_phone_____"

void toggleServo(Pwm::Servo &servo)
{
    servo.setAngle(45);
    sleep_ms(500);
    servo.setAngle(0);
}

int main()
{
    stdio_init_all();
    sleep_ms(2000);

    Wifi::ApScan scanner(SSID);
    Gpio::Output led(0);
    Pwm::Servo servo(0, 90, 1000, 2000, 28, 20000);

    int consecutiveFailures = 0;
    bool firstConnect = true;

    const int MAX_TIME_FAIL = Wifi::ApScan::MAX_CONSECUTIVE_FAIL_SCANS + 1;

    MAIN_LOOP_START
    if (scanner.scan())
    {
        if(firstConnect || consecutiveFailures >= MAX_TIME_FAIL)
        {
            led.setHigh();
            toggleServo(servo);

            firstConnect = false;
        }
        consecutiveFailures = 0;
    }
    else
    {
        if(consecutiveFailures > MAX_TIME_FAIL)
        {
            continue;
        }

        if (++consecutiveFailures == MAX_TIME_FAIL)
        {
            led.setLow();
            toggleServo(servo);
        }
    }

    int waitMs = 200;
    sleep_ms(waitMs);
    MAIN_LOOP_END

    return 0;
}