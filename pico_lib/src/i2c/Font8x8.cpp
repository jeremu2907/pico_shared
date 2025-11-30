#include "i2c/Font8x8.h"

void Font8x8::getBmp(uint8_t *buf, std::string s, size_t *startPos, size_t *endPos)
{
    size_t size = s.length();

    size_t bufIdx = (startPos == nullptr) ? 0 : *startPos; // Must be incremented by 8 every time. 1 bmp char = 8 bytes

    for (auto &c : s)
    {
        if (c == '\n')
        {
            // Must add startPos to account for initial dataframe offset if it exists
            bufIdx = ((bufIdx / 128) + 1) * 128 + (*startPos);
            continue;
        }
        getBmp(buf + bufIdx, c);
        bufIdx += 8;
    }

    if (endPos != nullptr)
    {
        *endPos = bufIdx;
    }
}

void Font8x8::getBmp(uint8_t *buf, char c)
{
    if (c > 0x7E) // Char not valid
    {
        return;
    }

    // Subtracting ' ' because first 32 chars aren't used
    // index starting at 0
    memcpy(buf, m_font[c - ' '], 8);
}