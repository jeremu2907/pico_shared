#include "i2c/Ssd1306.h"

using namespace I2c;

Ssd1306::Ssd1306() : m_addr(0x3C)
{
    if (!s_init)
    {
        s_init = true;
        m_baudrate = i2c_init(i2c_default, DEFAULT_BAUDRATE_HZ);
        gpio_set_function(PICO_DEFAULT_I2C_SDA_PIN, GPIO_FUNC_I2C);
        gpio_set_function(PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C);
        gpio_pull_up(PICO_DEFAULT_I2C_SDA_PIN);
        gpio_pull_up(PICO_DEFAULT_I2C_SCL_PIN);
        const uint8_t init_seq[] = {
            0x00,
            static_cast<uint8_t>(Command::DISPLAY_OFF),
            static_cast<uint8_t>(Command::SET_DISPLAY_CLOCK_DIV), 0x80,
            static_cast<uint8_t>(Command::SET_MULTIPLEX_RATIO), 0x3F,
            static_cast<uint8_t>(Command::SET_DISPLAY_OFFSET), 0x00,
            static_cast<uint8_t>(Command::SET_START_LINE) | 0x00,
            static_cast<uint8_t>(Command::CHARGE_PUMP), 0x14,
            static_cast<uint8_t>(Command::SET_MEMORY_MODE), 0x00,
            static_cast<uint8_t>(Command::SET_SEGMENT_REMAP),
            static_cast<uint8_t>(Command::SET_COM_OUTPUT_SCAN_DIR),
            static_cast<uint8_t>(Command::SET_COM_PINS_CONFIG), 0x12,
            static_cast<uint8_t>(Command::SET_CONTRAST), 0x7F,
            static_cast<uint8_t>(Command::SET_PRECHARGE_PERIOD), 0xF1,
            static_cast<uint8_t>(Command::SET_VCOMH_DESELECT), 0x40,
            static_cast<uint8_t>(Command::ENTIRE_DISPLAY_FORCE_ON),
            static_cast<uint8_t>(Command::NORMAL_DISPLAY),
            static_cast<uint8_t>(Command::DISPLAY_ON)
        };

        i2c_write_blocking(i2c_default, 0x3C, init_seq, sizeof(init_seq), false);
    }
}

void Ssd1306::scan()
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
            ret = i2c_read_blocking(i2c_default, addr, &rxdata, 1, false);

        printf(ret < 0 ? "." : "@");
        printf(addr % 16 == 15 ? "\n" : "  ");
    }
}

int Ssd1306::writeBlocking(const uint8_t *src, size_t len, bool nostop)
{
    uint8_t buf[len + 1];
    buf[0] = 0x40; // data control byte
    memcpy(buf + 1, src, len);
    return i2c_write_blocking(i2c_default, m_addr, buf, len + 1, false);
}

bool Ssd1306::isReservedAddr(uint8_t addr)
{
    return (addr & 0x78) == 0 || (addr & 0x78) == 0x78;
}