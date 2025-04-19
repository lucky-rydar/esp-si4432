#include "si4432.hpp"

#define LOG_TAG "Si443x"

Si4432::Si4432(SpiRegisterOps* spiOps)
: m_spiOps(spiOps)
{}

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
