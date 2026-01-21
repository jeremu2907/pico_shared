#include "pico/stdlib.h"
#include "hardware/timer.h"

#include "wifi/ApScan.hpp"
#include "gpio/Output.hpp"
#include "gpio/Input.hpp"
#include "pwm/Servo.hpp"
#include "pwm/Output.hpp"
#include "Macros.hpp"

#define SSID "_____jeremy_phone_____"

#define SR04_TIMEOUT_US 30000

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

    Gpio::Input rx(27);
    gpio_pull_down(rx.gpio());
    Gpio::Output tx(26);
    Gpio::Output ledGreen(21);
    Gpio::Output ledYellow(19);

    Wifi::ApScan scanner(SSID);

    Pwm::Servo servo(0, 90, 1000, 2000, 28, 20000);

    tx.setLow();

    volatile bool echoStarted = false;
    volatile uint64_t startTime = time_us_64();
    volatile uint64_t endTime = time_us_64();

    rx.installCallbackHigh([&echoStarted, &startTime]()
                           {
        if(echoStarted)
        {
            return;
        }

        startTime = time_us_64();
        echoStarted = true; });

    rx.installCallbackLow([&echoStarted, &endTime]()
                          {
        if (!echoStarted)
        {
            return;
        }
        endTime = time_us_64(); });

    bool firstConnect = true;
    int consecutiveFailures = 0;

    const int MAX_TIME_FAIL = Wifi::ApScan::MAX_CONSECUTIVE_FAIL_SCANS + 1;

    MAIN_LOOP_START
    echoStarted = false;
    startTime = 0;
    endTime = 0;

    tx.setHigh();
    sleep_us(10);
    tx.setLow();

    uint64_t echoLoopStart = time_us_64();
    while (time_us_64() - echoLoopStart < SR04_TIMEOUT_US)
    {
        Gpio::Input::runLoop();
    }

    uint64_t pulseWidthUs = endTime - startTime;
    double distanceCm = static_cast<double>(pulseWidthUs) / 58.0f;
    if(distanceCm > 30)
    {
        ledYellow.setHigh();
    }
    else
    {
        ledYellow.setLow();
    }

    if (scanner.scan())
    {
        if (firstConnect || consecutiveFailures >= MAX_TIME_FAIL)
        {
            ledGreen.setHigh();
            toggleServo(servo);
            firstConnect = false;

            for (int flashTime = 1; flashTime <= 10; flashTime++)
            {
                ledYellow.setHigh();
                sleep_ms(500);
                ledYellow.setLow();
                sleep_ms(500);
            }
        }
        consecutiveFailures = 0;
    }
    else
    {
        if (consecutiveFailures > MAX_TIME_FAIL)
        {
            sleep_ms(200);
            continue;
        }

        if (++consecutiveFailures == MAX_TIME_FAIL && !firstConnect)
        {
            ledGreen.setLow();
            toggleServo(servo);
        }
    }
    sleep_ms(200);
    MAIN_LOOP_END

    return 0;
}