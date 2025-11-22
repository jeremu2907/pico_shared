#include "i2c/Oled.h"

using namespace I2c;

Oled::Oled() : m_addr(0x3C)
{
    if (!s_init)
    {
        s_init = true;
        m_baudrate = i2c_init(i2c_default, DEFAULT_BAUDRATE_HZ);
        gpio_set_function(PICO_DEFAULT_I2C_SDA_PIN, GPIO_FUNC_I2C);
        gpio_set_function(PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C);
        gpio_pull_up(PICO_DEFAULT_I2C_SDA_PIN);
        gpio_pull_up(PICO_DEFAULT_I2C_SCL_PIN);
        uint8_t init_seq[] = {
            0xAE,       // Display OFF
            0xD5, 0x80, // Set display clock
            0xA8, 0x3F, // Multiplex 64
            0xD3, 0x00, // Display offset
            0x40,       // Start line 0
            0x8D, 0x14, // Charge pump
            0x20, 0x00, // Memory mode
            0xA1,       // Segment remap
            0xC8,       // COM scan direction
            0xDA, 0x12, // COM pins
            0x81, 0xCF, // Contrast
            0xD9, 0xF1, // Precharge
            0xDB, 0x40, // VCOM detect
            0xA4,       // Display all on resume
            0xA6,       // Normal display
            0xAF        // Display ON
        };
        i2c_write_blocking(i2c_default, 0x3C, init_seq, sizeof(init_seq), false);
    }
}

void Oled::scan()
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

int Oled::writeBlocking(const uint8_t *src, size_t len, bool nostop)
{
    uint8_t buf[len + 1];
    buf[0] = 0x40; // data control byte
    memcpy(buf + 1, src, len);
    return i2c_write_blocking(i2c_default, m_addr, buf, len + 1, false);
}

bool Oled::isReservedAddr(uint8_t addr)
{
    return (addr & 0x78) == 0 || (addr & 0x78) == 0x78;
}