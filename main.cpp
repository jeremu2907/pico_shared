#include "pico/stdlib.h"

#include "wifi/ApScan.hpp"
#include "gpio/Output.hpp"
#include "Macros.hpp"

#define SSID "_____jeremy_phone_____"

int main()
{
    stdio_init_all();
    sleep_ms(2000);

    Wifi::ApScan scanner(SSID);
    Gpio::Output led(0);

    int consecutiveFailures = 0;

    MAIN_LOOP_START
        bool found = scanner.scan();
        
        scanner.printResults();
        
        if (found) {
            consecutiveFailures = 0;
            led.setHigh();
        } else {
            consecutiveFailures++;
            if(consecutiveFailures > 3)
            {
                led.setLow();
            }
        }
        
        int waitMs = 200;
        printf("\nWaiting %d ms before next scan...\n", waitMs);
        printf("========================================\n\n");
        sleep_ms(waitMs);
    MAIN_LOOP_END

    return 0;
}