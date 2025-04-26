#ifndef SI443X_HPP
#define SI443X_HPP

#include <cinttypes>
#include <functional>
#include <stdio.h>

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
    enum Regs : uint8_t {
        DEV_TYPE = 0x00,
        DEV_VERSION = 0x01,
        SYNC_WORD3 = 0x36,
        SYNC_WORD2 = 0x37,
        SYNC_WORD1 = 0x38,
        SYNC_WORD0 = 0x39,
        MODULATION_CONTROL_1 = 0x70,
        MODULATION_CONTROL_2 = 0x71,
    };

public:
    enum class Mode {
        SHUTDOWN, // to tx: 16.8ms, to rx: 16.8ms, supply 15nA
        STANDBY,  // to tx: 800mcs, to rx: 800mcs, supply 450nA
        SLEEP,    // to tx: 800mcs, to rx: 800mcs, supply 1mcA
        SENSOR,   // to tx: 800mcs, to rx: 800mcs, supply 1mcA
        READY,    // to tx: 200mcs, to rx: 200mcs, supply 800mcA
        TUNING,   // to tx: 200mcs, to rx: 200mcs, supply 8.5mA
        TRANSMIT, // to tx: -     , to rx: 200mcs, supply 30mA + 13dB
        RECEIVE   // to tx: 200mcs, to rx: -     , supply 18.5mA
    };

    enum class ModulationType : uint8_t {
        UNMODULATED = 0x00,
        OOK = 0x01,
        FSK = 0x02,
        GFSK = 0x03,
        UNKNOWN = 0xFF
    };

    enum class ModulationDataSource : uint8_t {
        DIRECT_GPIO = 0x00,
        DIRECT_SPI = 0x01,
        FIFO = 0x02, // default
        PN9 = 0x03,
        UNKNOWN = 0xFF
    };

private:
    const uint8_t modtyp_mask = 0b00000011;

    SpiRegisterOps* m_spiOps;
    GpioOps* m_gpioOps;
    int m_ssPin = -1;
    int m_shdnPin = -1;

public:
    Si4432(SpiRegisterOps* spiOps, GpioOps* gpioOps, int ssPin, int shdnPin);

    void initHw();
    void initRegs();

    void getSyncWord(uint8_t* data);
    uint8_t getType();
    uint8_t getVersion();
    Si4432::Mode getCurrentMode();

    void setModulation(ModulationType modType);
    ModulationType getModulation();

    void transmit(uint8_t* data, int len);
    bool isReceived();
    void receive();

    /**
     * Function: onIrq
     * 
     * Description: expected to be called externally from irq handler.
     */
    void onIrq();

private:
    void setDataSource(ModulationDataSource dataSource);

};

#endif // SI443X_HPP
