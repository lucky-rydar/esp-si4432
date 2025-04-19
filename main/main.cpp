#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sdkconfig.h"
#include "esp_log.h"

#include "esp_system.h"

#include <cstring>

#include "si4432_esp_hal.hpp"
#include "si4432.hpp"

// TODO: all the following definitions should be specified in the kmenu
// #define ESP32_S3
#define ESP32_C3_MINI

#ifdef ESP32_C3_MINI
#define PIN_MISO 5
#define PIN_MOSI 6
#define PIN_SS GPIO_NUM_7
#define PIN_CLK 4
#define PIN_NIRQ 3
#define PIN_SHDN GPIO_NUM_10
#define PIN_SI4432_GPIO0 1
#define PIN_SI4432_GPIO1 2
#define PIN_SI4432_GPIO2 3
#endif

#ifdef ESP32_S3
#define PIN_MISO 9
#define PIN_MOSI 10
#define PIN_SS GPIO_NUM_7
#define PIN_CLK 8
#define PIN_NIRQ 6
#define PIN_SHDN GPIO_NUM_13 // ?
#define PIN_SI4432_GPIO0 1 // ?
#define PIN_SI4432_GPIO1 2 // ?
#define PIN_SI4432_GPIO2 3 // ?
#endif


extern "C" void app_main(void)
{
    esp_err_t ret;
    
    // TODO: should be configured with driver of Si4432 via GPIO HAL
    gpio_reset_pin(PIN_SS);
	gpio_set_direction(PIN_SS, GPIO_MODE_OUTPUT);
	gpio_set_level(PIN_SS, 1);
    
    spi_device_handle_t spi;
    spi_bus_config_t buscfg = {
        .mosi_io_num = PIN_MOSI,
        .miso_io_num = PIN_MISO,
        .sclk_io_num = PIN_CLK,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
    };

    spi_device_interface_config_t devcfg = {
        .mode = 0,
        .clock_speed_hz = 10'000'000,
        .spics_io_num = -1, // manual
        .flags = SPI_DEVICE_NO_DUMMY,
        .queue_size = 7,
    };

    ret = spi_bus_initialize(SPI2_HOST, &buscfg, SPI_DMA_CH_AUTO);
    ESP_ERROR_CHECK(ret);

    ret = spi_bus_add_device(SPI2_HOST, &devcfg, &spi);
    ESP_ERROR_CHECK(ret);

    // TODO: should be moved to driver's code
    gpio_reset_pin(PIN_SHDN);
	gpio_set_direction(PIN_SHDN, GPIO_MODE_OUTPUT);
    gpio_set_level(PIN_SHDN, 0);

    vTaskDelay(pdMS_TO_TICKS(1000)); // Delay for 1 second

    Si4432SpiRegisterOps si4432Ops(spi, PIN_SS);
    Si4432 si4432((SpiRegisterOps*)&si4432Ops);

    uint8_t syncWord[4];
    si4432.getSyncWord(syncWord);
    printf("Sync Word: 0x%02X 0x%02X 0x%02X 0x%02X\n", syncWord[0], syncWord[1], syncWord[2], syncWord[3]);

    while (1) {
        uint8_t reg_val = si4432Ops.readReg(0x01);
        printf("Register 0x01 (Device Version) value: 0x%02X\n", reg_val);

        vTaskDelay(pdMS_TO_TICKS(1000)); // Delay for 1 second
    }
}
