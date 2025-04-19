#ifndef SI443X_HPP
#define SI443X_HPP

#include <cinttypes>

class SpiRegisterOps {
public:
    virtual void writeReg(uint8_t reg, uint8_t val) = 0;
    virtual uint8_t readReg(uint8_t reg) = 0;
    virtual void writeBurst(uint8_t reg, uint8_t* data, uint64_t len) {}
    virtual void readBurst(uint8_t reg, uint8_t* data, uint64_t len) {}
};

class Si4432 {
private:
    enum Regs {
        REG_SYNC_WORD3 = 0x36,
        REG_SYNC_WORD2 = 0x37,
        REG_SYNC_WORD1 = 0x38,
        REG_SYNC_WORD0 = 0x39,
    };

private:
    SpiRegisterOps* m_spiOps;

public:
    Si4432(SpiRegisterOps* spiOps);
    void getSyncWord(uint8_t* data);

};

#endif // SI443X_HPP
