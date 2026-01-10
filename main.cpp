#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"

#include "wifi/AccessPoint.hpp"
#include "pwm/Output.hpp"
#include "Macros.hpp"

#include "pico/cyw43_arch.h"

int main()
{
    stdio_init_all();

    if (cyw43_arch_init())
    {
        printf("failed to initialise cyw43_arch\n");
        return 1;
    }

    // cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
    Wifi::AccessPoint ap("pico_test_ap", "password");
    ap.installCallbackOnDhcpConnect(
        []()
        {
            printf("##############################\n");
            printf("Hello\n");
            printf("##############################\n");
        }
    );


    MAIN_LOOP_START
    ap.runLoop();
    MAIN_LOOP_END

    cyw43_arch_deinit();
    printf("Main complete\n");
    return 0;
}