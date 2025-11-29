#include "i2c/Font8x8.h"

void Font8x8::getFont(uint8_t* buf, std::string s)
{
    size_t size = s.length();
    
    size_t bufIdx = 0;  // Must be incremented by 8 every time. 1 bmp char = 8 bytes
    for(auto &c : s)
    {
        getFont(buf + bufIdx, c);
        bufIdx += 8;
    }
}

void Font8x8::getFont(uint8_t* buf, char c)
{
    if(c > 0x7E)    // Char not valid
    {
        return;
    }

    // Subtracting ' ' because first 32 chars aren't used
    // index starting at 0
    memcpy(buf, m_font[c - ' '], 8);
}