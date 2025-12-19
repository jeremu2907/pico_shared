#include <stdio.h>

#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"

const char SSID[] = "PICO-AP";
const char PASSWORD[] = "p@ssw0rd";

int main() {
    stdio_init_all();
    sleep_ms(1000);

    printf("Initializng CYW34...");
    if (cyw43_arch_init()) {
        return 1;
    }
    printf("CYW34 init done...\n");

    cyw43_wifi_ap_set_channel(&cyw43_state, 6);

    cyw43_arch_enable_ap_mode(
        SSID,
        PASSWORD,
        CYW43_AUTH_WPA2_AES_PSK
    );

    printf("AP started\n");
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);

    while (true) {
    }
}
