#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"

#include "Macros.hpp"

int main()
{
    stdio_init_all();

    if(cyw43_arch_init())
    {
        ERR_START
        printf("CYW43 init failed\n");
        ERR_END
    }
    
    while (true)
    {
        printf("LED ON\n");
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
        sleep_ms(500);
        
        printf("LED OFF\n");
        cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);
        sleep_ms(500);
    }
}