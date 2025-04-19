#include "si4432.hpp"

#define LOG_TAG "Si443x"

Si4432::Si4432(SpiRegisterOps* spiOps)
: m_spiOps(spiOps)
{}

void Si4432::getSyncWord(uint8_t* data) {
    data[3] = m_spiOps->readReg(REG_SYNC_WORD3);
    data[2] = m_spiOps->readReg(REG_SYNC_WORD2);
    data[1] = m_spiOps->readReg(REG_SYNC_WORD1);
    data[0] = m_spiOps->readReg(REG_SYNC_WORD0);
}


