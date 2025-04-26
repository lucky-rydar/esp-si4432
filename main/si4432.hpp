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
        INTERRUPT_STATUS_1 = 0x03,
        INTERRUPT_STATUS_2 = 0x04,
        INTERRUPT_ENABLE_1 = 0x05,
        INTERRUPT_ENABLE_2 = 0x06,
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

    struct InterruptStatus {
        union {
            struct {
                // Bitfields for Interrupt Status 1 (Address 0x03)
                uint8_t icrcrerror : 1; // D0: CRC Error Interrupt Status (Valid Packet Received Interrupt Enable = 1 required)
                uint8_t ipkvalid   : 1; // D1: Packet Valid Interrupt Status
                uint8_t ipksent    : 1; // D2: Packet Sent Interrupt Status
                uint8_t iext       : 1; // D3: External Interrupt Status
                uint8_t irxffafull : 1; // D4: RX FIFO Almost Full Interrupt Status
                uint8_t itxffaem   : 1; // D5: TX FIFO Almost Empty Interrupt Status
                uint8_t itxffafull : 1; // D6: TX FIFO Almost Full Interrupt Status
                uint8_t iferr      : 1; // D7: FIFO Error Interrupt Status

                // Bitfields for Interrupt Status 2 (Address 0x04)
                uint8_t ipor       : 1; // D0: Power-On Reset Interrupt Status
                uint8_t ichiprdy   : 1; // D1: Chip Ready (XTAL OK) Interrupt Status
                uint8_t ilbd       : 1; // D2: Low Battery Detect Interrupt Status
                uint8_t iwut       : 1; // D3: Wake-Up Timer Interrupt Status
                uint8_t irssi      : 1; // D4: RSSI Interrupt Status
                uint8_t ipreainval : 1; // D5: Invalid Preamble Detected Interrupt Status
                uint8_t ipreaval   : 1; // D6: Valid Preamble Detected Interrupt Status
                uint8_t iswdet     : 1; // D7: Sync Word Detected Interrupt Status
            } bits;
            struct {
                uint8_t reg1;
                uint8_t reg2;
            } regs;
        };
    };

    struct InterruptEnable {
        union {
            struct {
                // Bitfields for Interrupt Enable 1 (Address 0x05)
                uint8_t encrerror  : 1; // D0
                uint8_t enpkvalid  : 1; // D1
                uint8_t enpksent   : 1; // D2
                uint8_t enext      : 1; // D3
                uint8_t enrxffafull: 1; // D4
                uint8_t entxffaem  : 1; // D5
                uint8_t entxffafull: 1; // D6
                uint8_t enfferr    : 1; // D7

                // Bitfields for Interrupt Enable 2 (Address 0x06)
                uint8_t enpor      : 1; // D0
                uint8_t enchiprdy  : 1; // D1
                uint8_t enlbd      : 1; // D2
                uint8_t enwut      : 1; // D3
                uint8_t enrssi     : 1; // D4
                uint8_t enpreainval: 1; // D5
                uint8_t enpreaval  : 1; // D6
                uint8_t enswdet    : 1; // D7
            } bits;
            struct {
                uint8_t reg1;
                uint8_t reg2;
            } regs;
        };
    };

private:
    const uint8_t modtyp_mask = 0b00000011;

    SpiRegisterOps* m_spiOps;
    GpioOps* m_gpioOps;
    int m_ssPin = -1;
    int m_shdnPin = -1;

    InterruptStatus m_interruptStatusLatest;

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

    void applyInterruptEnable(InterruptEnable interruptEnable);

private:
    void setDataSource(ModulationDataSource dataSource);
    InterruptStatus getInterruptStatus();

};

#endif // SI443X_HPP
