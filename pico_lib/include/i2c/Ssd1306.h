#include <stdio.h>
#include <cstring>

#include "pico/stdlib.h"
#include "hardware/i2c.h"

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define DEFAULT_BAUDRATE_HZ 400000

namespace I2c
{
    class Ssd1306
    {
        enum class ControlByte : uint8_t
        {
            // [Co#][D/C#]00 0000
            NO_CO_COMMAND = 0x00,
            NO_CO_DATA = 0x40,
            CO_COMMAND = 0x80,
            CO_DATA = 0xC0
        };

        enum class Command : uint8_t
        {
            SET_CONTRAST = 0x81,            // Followed by 1 byte: contrast level (0–255)
            ENTIRE_DISPLAY_RESUME = 0xA4,   // Display RAM content
            ENTIRE_DISPLAY_FORCE_ON = 0xA5, // All pixels on
            NORMAL_DISPLAY = 0xA6,          // Non-inverted
            INVERT_DISPLAY = 0xA7,          // Inverted
            DISPLAY_OFF = 0xAE,
            DISPLAY_ON = 0xAF,

            RIGHT_HORIZONTAL_SCROLL = 0x26,
            LEFT_HORIZONTAL_SCROLL = 0x27,
            VERTICAL_AND_RIGHT_SCROLL = 0x29,
            VERTICAL_AND_LEFT_SCROLL = 0x2A,
            DEACTIVATE_SCROLL = 0x2E,
            ACTIVATE_SCROLL = 0x2F,
            SET_VERTICAL_SCROLL_AREA = 0xA3,

            SET_MEMORY_MODE = 0x20, // 0=horizontal,1=vertical,2=page
            SET_COLUMN_ADDR = 0x21, // two bytes: start, end
            SET_PAGE_ADDR = 0x22,   // two bytes: start, end

            SET_START_LINE = 0x40,          // 0x40–0x7F (use OR with value)
            SET_SEGMENT_REMAP = 0xA1,       // A0 or A1
            SET_MULTIPLEX_RATIO = 0xA8,     // 1 byte: 0x0F–0x3F (for 128×64 = 0x3F)
            SET_COM_OUTPUT_SCAN_DIR = 0xC8, // or 0xC0 (normal)
            SET_DISPLAY_OFFSET = 0xD3,      // 1 byte
            SET_COM_PINS_CONFIG = 0xDA,     // 1 byte: 0x02 / 0x12 etc.

            SET_DISPLAY_CLOCK_DIV = 0xD5, // 1 byte: divide ratio + oscillator frequency
            SET_PRECHARGE_PERIOD = 0xD9,  // 1 byte
            SET_VCOMH_DESELECT = 0xDB,    // 1 byte

            CHARGE_PUMP = 0x8D, // 1 byte: 0x14=on, 0x10=off

        };

    private:
        inline static bool s_init = false;
        inline static uint8_t s_HW_ADDR = 0x3C;

    public:
        explicit Ssd1306();
        ~Ssd1306();

        void scan();

        /// @brief This function writes to GDDRAM assuming Horizontal Addressing Mode
        /// @param src data array pointer. Each byte is a a segment, representing 8-bit
        /// column data for the screen. There are 128 segments per page (row) and 8 pages [0:7].
        /// The array thus should be in the form of src[i] = data at page[i / 128] segment[i % 128]
        /// @param len size of src in bytes
        /// @return number of bytes written, negative if error
        int writeScreen(const uint8_t *src, size_t len);

    private:
        int writeBlocking(const uint8_t *src, size_t len, bool nostop = true);
        bool isReservedAddr(uint8_t addr);

        uint m_baudrate;
        uint m_addr;
    };
}
