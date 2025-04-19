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

enum class GpioDirection {
    Disable,
    Input,
    Output
};

class GpioOps {
public:
    virtual void reset(int pin) = 0;
    virtual void setDirection(int pin, GpioDirection direction) = 0;
    virtual void setLevel(int pin, int lvl) = 0; 
};

class Si4432 {
private:
    enum Regs {
        DEV_TYPE = 0x00,
        DEV_VERSION = 0x01,

        SYNC_WORD3 = 0x36,
        SYNC_WORD2 = 0x37,
        SYNC_WORD1 = 0x38,
        SYNC_WORD0 = 0x39,
    };

private:
    SpiRegisterOps* m_spiOps;
    GpioOps* m_gpioOps;
    int m_ssPin = -1;
    int m_shdnPin = -1;

public:
    Si4432(SpiRegisterOps* spiOps, GpioOps* gpioOps, int ssPin, int shdnPin);

    void init();

    void getSyncWord(uint8_t* data);
    uint8_t getType();
    uint8_t getVersion();

};

#endif // SI443X_HPP
