#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sdkconfig.h"

#include "esp_system.h"
#include "driver/spi_master.h"
#include "driver/gpio.h"

#define PIN_MISO 5
#define PIN_MOSI 6
#define PIN_SS 7
#define PIN_CLK 4
#define PIN_NIRQ 8
#define PIN_SHDN 9
#define PIN_SI4432_GPIO0 1
#define PIN_SI4432_GPIO1 2
#define PIN_SI4432_GPIO2 3

extern "C" void app_main(void)
{
    int counter = 0;

    while (1) {
        printf("Hello world! Counter: %d\n", counter++);
        vTaskDelay(pdMS_TO_TICKS(1000)); // Delay for 1 second
    }
}
