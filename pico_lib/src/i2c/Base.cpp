#include <stdio.h>
#include <cstring>

#include "i2c/Base.h"
#include "Macros.hpp"

using namespace I2c;

Base::Base(uint addr, uint gpioSda, uint gpioScl, i2c_inst_t *busInstance) : m_gpioSda(gpioSda),
                                                                             m_gpioScl(gpioScl),
                                                                             m_addr(addr)
{
    m_busNumber = (busInstance == i2c0) ? 0 : 1;

    if (m_sInitializedBusMap.count(m_busNumber) > 0)
    {
        ERR_START
        printf("Error: Bus number %u already claimed!\n", m_busNumber);
        ERR_END
    }

    m_sInitializedBusMap[m_busNumber] = true;
    m_baudrate = i2c_init(busInstance, DEFAULT_BAUDRATE_HZ);
    init();
}

Base::~Base()
{
    m_sInitializedBusMap.erase(m_busNumber);
}

void Base::scan()
{
    printf("\nI2C Bus Scan\n");
    printf("   0  1  2  3  4  5  6  7  8  9  A  B  C  D  E  F\n");
    for (int addr = 0; addr < (1 << 7); ++addr)
    {
        if (addr % 16 == 0)
        {
            printf("%02x ", addr);
        }

        // Perform a 1-byte dummy read from the probe address. If a slave
        // acknowledges this address, the function returns the number of bytes
        // transferred. If the address byte is ignored, the function returns
        // -1.

        // Skip over any reserved addresses.
        int ret;
        uint8_t rxdata;
        if (isReservedAddr(addr))
            ret = PICO_ERROR_GENERIC;
        else
            ret = i2c_read_blocking(getBusInstance(), addr, &rxdata, 1, false);

        printf(ret < 0 ? "." : "@");
        printf(addr % 16 == 15 ? "\n" : "  ");
    }
}

int Base::writeBlocking(const uint8_t *src, size_t len, bool nostop)
{
    return i2c_write_blocking(getBusInstance(), m_addr, src, len, nostop);
}

int Base::writeRaw(const uint8_t *src, size_t len)
{
    return writeBlocking(src, len);
}

bool Base::isReservedAddr(uint8_t addr)
{
    return (addr & 0x78) == 0 || (addr & 0x78) == 0x78;
}

i2c_inst_t *Base::getBusInstance()
{
    return (m_busNumber == 0) ? i2c0 : i2c1;
}

void Base::init()
{
    gpio_set_function(m_gpioSda.gpio(), GPIO_FUNC_I2C);
    gpio_set_function(m_gpioScl.gpio(), GPIO_FUNC_I2C);
    gpio_pull_up(m_gpioSda.gpio());
    gpio_pull_up(m_gpioScl.gpio());
}