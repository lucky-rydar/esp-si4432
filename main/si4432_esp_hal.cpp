#include "si4432_esp_hal.hpp"
#include <string.h>
#include "esp_log.h"
#include "driver/gpio.h"

static const char *TAG = "Si4432SpiRegisterOps";

Si4432SpiRegisterOps::Si4432SpiRegisterOps(spi_device_handle_t spi, gpio_num_t spiSs) {
    m_spi = spi;
    m_spiSs = spiSs;
}

void Si4432SpiRegisterOps::writeReg(uint8_t reg, uint8_t val) {
    writeBurst(reg, &val, 1);
}

uint8_t Si4432SpiRegisterOps::readReg(uint8_t reg) {
    uint8_t val = 0x00;
    readBurst(reg, &val, 1);
    return val;
}

void Si4432SpiRegisterOps::writeBurst(uint8_t reg, uint8_t* data, uint64_t len) {
    uint8_t regVal = (uint8_t) reg | 0x80; // set MSB

    gpio_set_level(m_spiSs, 0);
    spi_transfer(regVal);

    // NOTE: Original loop counter 'i' was uint8_t, potential overflow if len > 255
    // NOTE: Original logic `data[i] = spi_transfer(0xFF);` is likely incorrect for write,
    //       it reads data while sending 0xFF instead of writing data[i.
    //       Keeping original code as requested.
    for (uint8_t i = 0; i < len; ++i) {
        data[i] = spi_transfer(0xFF);
        ESP_LOGI(TAG,"Writing: 0x%02X | 0x%02X", reg, data[i]);
    }

    gpio_set_level(m_spiSs, 1);
}

void Si4432SpiRegisterOps::readBurst(uint8_t reg, uint8_t* data, uint64_t len) {
    uint8_t regVal = (uint8_t) reg & 0x7F; // clear MSB

    gpio_set_level(m_spiSs, 0);
    spi_transfer(regVal);

    // NOTE: Original loop counter 'i' was uint8_t, potential overflow if len > 255
    //       Keeping original code as requested.
    for (uint8_t i = 0; i < len; ++i) {
        data[i] = spi_transfer(0xFF);
        ESP_LOGI(TAG,"Reading: 0x%02X | 0x%02X", reg, data[i]);
    }

    gpio_set_level(m_spiSs, 1);
}

uint8_t Si4432SpiRegisterOps::spi_transfer(uint8_t address)
{
    uint8_t datain[1];
    uint8_t dataout[1];
    dataout[0] = address;

    spi_transaction_t transaction;
    memset( &transaction, 0, sizeof( spi_transaction_t ) );
    transaction.length = 8;
    transaction.tx_buffer = dataout;
    transaction.rx_buffer = datain;
    spi_device_transmit(m_spi, &transaction );

    return datain[0];
}
