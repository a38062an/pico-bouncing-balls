#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"

int main()
{
    stdio_init_all();

    // Initialize the WiFi chip (which controls the LED)
    if (cyw43_arch_init())
    {
        return -1;
    }

    // Turn LED off
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);

    while (true)
    {
        sleep_ms(1000);
    }
}
