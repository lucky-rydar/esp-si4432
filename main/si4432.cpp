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
    m_spiOps->readBurst(Si4432::Regs::SYNC_WORD3, data, sync_word_len);
}

uint8_t Si4432::getType() {
    return m_spiOps->readReg(Si4432::Regs::DEV_TYPE);
}

uint8_t Si4432::getVersion() {
    // default is 0x06
    return m_spiOps->readReg(Si4432::Regs::DEV_VERSION) & 0b00011111;
}

Si4432::Mode Si4432::getCurrentMode() {
    // TODO: implement
    return Mode::SHUTDOWN;
}

void Si4432::setModulation(ModulationType modType) {
    uint8_t mod_control_2_reg_val = m_spiOps->readReg(Regs::MODULATION_CONTROL_2);
    uint8_t modtyp_bits = 0x00;
    uint8_t modtyp_mask = 0b00000011;

    switch(modType) {
        case ModulationType::UNMODULATED:
            modtyp_bits = 0x00;
            break;
        case ModulationType::OOK:
            modtyp_bits = 0x01;
            break;
        case ModulationType::FSK:
            modtyp_bits = 0x02;
            break;
        case ModulationType::GFSK:
            modtyp_bits = 0x03;
            break;
        default:
            return;
    };

    mod_control_2_reg_val &= ~modtyp_mask;
    mod_control_2_reg_val |= modtyp_bits;

    m_spiOps->writeReg(Regs::MODULATION_CONTROL_2, mod_control_2_reg_val);
}

Si4432::ModulationType Si4432::getModulation() {
    uint8_t mod_control_2_reg_val = m_spiOps->readReg(Regs::MODULATION_CONTROL_2);
    switch(mod_control_2_reg_val) {
        case 0x00:
            return Si4432::ModulationType::UNMODULATED;
        case 0x01:
            return Si4432::ModulationType::OOK;
        case 0x02:
            return Si4432::ModulationType::FSK;
        case 0x03:
            return Si4432::ModulationType::GFSK;
        default:
            return Si4432::ModulationType::UNKNOWN;
    };
    return Si4432::ModulationType::UNKNOWN;
}
