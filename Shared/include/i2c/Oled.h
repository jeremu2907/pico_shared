#include <stdio.h>
#include <cstring>

#include "pico/stdlib.h"
#include "hardware/i2c.h"

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define DEFAULT_BAUDRATE_HZ 400000

namespace I2c
{
    class Oled
    {
    private:
        inline static bool s_init = false;

    public:
        explicit Oled();
        ~Oled();

        void scan();
        
        int writeBlocking(const uint8_t* src, size_t len, bool nostop = true);
    
    private:
        bool isReservedAddr(uint8_t addr);

        uint m_baudrate;
        uint m_addr;
    };
}
