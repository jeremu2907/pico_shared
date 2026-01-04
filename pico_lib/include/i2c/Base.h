#pragma once

#include <map>
#include <stdio.h>
#include <cstring>

#include "pico/stdlib.h"
#include "hardware/i2c.h"

namespace I2c
{
    class Base
    {
    public:
        static inline constexpr uint DEFAULT_BAUDRATE_HZ = 400000;

    private:
        inline static std::map<uint, bool> m_sInitializedBusMap{};

    public:
        Base(uint addr,
             uint gpioSda = PICO_DEFAULT_I2C_SDA_PIN,
             uint gpioScl = PICO_DEFAULT_I2C_SCL_PIN,
             i2c_inst_t *busInstance = i2c0);

        ~Base();

        void scan();

        /// @brief This function writes raw data to GDDRAM assuming Horizontal Addressing Mode
        /// @param src data array pointer. Each byte is a a segment, representing 8-bit
        /// column data for the screen. There are 128 segments per page (row) and 8 pages [0:7].
        /// The array thus should be in the form of src[i] = data at page[i / 128] segment[i % 128]
        /// @param len size of src in bytes
        /// @return number of bytes written, negative if error
        virtual int writeRaw(const uint8_t *src, size_t len);

    protected:
        bool isReservedAddr(uint8_t addr);

        int writeBlocking(const uint8_t *src, size_t len, bool nostop = true);

        i2c_inst_t *getBusInstance();

        uint m_addr;
        uint m_gpioScl;
        uint m_gpioSda;
        uint m_baudrate;
        uint m_busNumber;
    };
}