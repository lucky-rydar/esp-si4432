#ifndef SI4432_ESP_HAL_HPP
#define SI4432_ESP_HAL_HPP

#include <stdint.h>
#include "driver/spi_master.h"
#include "driver/gpio.h"
#include "si4432.hpp"

class SpiRegisterOps;

class Si4432SpiRegisterOps : public SpiRegisterOps {
private:
    spi_device_handle_t m_spi;
    gpio_num_t m_spiSs;

    uint8_t spi_transfer(uint8_t address);

public:
    Si4432SpiRegisterOps(spi_device_handle_t spi, gpio_num_t spiSs);

    void writeReg(uint8_t reg, uint8_t val) override;
    uint8_t readReg(uint8_t reg) override;
    void writeBurst(uint8_t reg, uint8_t* data, uint64_t len) override;
    void readBurst(uint8_t reg, uint8_t* data, uint64_t len) override;
};

#endif // SI4432_ESP_HAL_HPP