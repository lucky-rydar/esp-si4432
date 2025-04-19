#include "si4432.hpp"

#define LOG_TAG "Si443x"

Si4432::Si4432(SpiRegisterOps* spiOps, GpioOps* gpioOps, int ssPin, int shdnPin)
    : m_spiOps(spiOps)
    , m_gpioOps(gpioOps)
    , m_ssPin(ssPin)
    , m_shdnPin(shdnPin)
{}

void Si4432::init() {
    m_gpioOps->reset(m_ssPin);
    m_gpioOps->setDirection(m_ssPin, GpioDirection::Output);
    m_gpioOps->setLevel(m_ssPin, 1);

    m_gpioOps->reset(m_shdnPin);
    m_gpioOps->setDirection(m_shdnPin, GpioDirection::Output);
    m_gpioOps->setLevel(m_shdnPin, 0);
}

void Si4432::getSyncWord(uint8_t* data) {
    const int sync_word_len = 4;
    m_spiOps->readBurst(SYNC_WORD3, data, sync_word_len);
}

uint8_t Si4432::getType() {
    return m_spiOps->readReg(Regs::DEV_TYPE);
}

uint8_t Si4432::getVersion() {
    // default is 0x06
    return m_spiOps->readReg(Regs::DEV_VERSION) & 0b00011111;
}
