#include "si4432.hpp"

#define LOG_TAG "Si443x"

Si4432::Si4432(SpiRegisterOps* spiOps, GpioOps* gpioOps, int ssPin, int shdnPin)
    : m_spiOps(spiOps)
    , m_gpioOps(gpioOps)
    , m_ssPin(ssPin)
    , m_shdnPin(shdnPin)
{}

void Si4432::initHw() {
    m_gpioOps->reset(m_ssPin);
    m_gpioOps->setDirection(m_ssPin, GpioDirection::Output);
    m_gpioOps->setLevel(m_ssPin, 1);

    m_gpioOps->reset(m_shdnPin);
    m_gpioOps->setDirection(m_shdnPin, GpioDirection::Output);
    m_gpioOps->setLevel(m_shdnPin, 0);
}

void Si4432::initRegs() {
    setDataSource(ModulationDataSource::FIFO);
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
    if((uint8_t)modType > (uint8_t)ModulationType::GFSK) {
        // not valid
        return;
    }

    uint8_t mod_control_2_reg_val = m_spiOps->readReg(Regs::MODULATION_CONTROL_2);
    uint8_t modtyp_bits = 0x00;
    modtyp_bits = (uint8_t)modType;

    mod_control_2_reg_val &= ~modtyp_mask;
    mod_control_2_reg_val |= modtyp_bits;

    m_spiOps->writeReg(Regs::MODULATION_CONTROL_2, mod_control_2_reg_val);
}

Si4432::ModulationType Si4432::getModulation() {
    uint8_t mod_control_2_reg_val = m_spiOps->readReg(Regs::MODULATION_CONTROL_2) & this->modtyp_mask;
    if(mod_control_2_reg_val <= (uint8_t)ModulationType::GFSK) {
        return (Si4432::ModulationType)mod_control_2_reg_val;
    } else {
        return Si4432::ModulationType::UNKNOWN;
    }
}

void Si4432::setDataSource(ModulationDataSource dataSource) {
    uint8_t dataSourceMask = 0b00110000;
    uint8_t dataSourceInt = (uint8_t)dataSource;
    uint8_t mod_control_2_reg_val = m_spiOps->readReg(Regs::MODULATION_CONTROL_2);
    mod_control_2_reg_val &= (~dataSourceMask);
    mod_control_2_reg_val |= (dataSourceInt << 4);
    m_spiOps->writeReg(Regs::MODULATION_CONTROL_2, mod_control_2_reg_val);
}

void Si4432::transmit(uint8_t* data, int len) {
    // TODO: implement
}

void Si4432::setOnReceive(OnReceive onReceive) {
    m_onReceive = onReceive;
}

void Si4432::receive() {
    // TODO: implement
}
