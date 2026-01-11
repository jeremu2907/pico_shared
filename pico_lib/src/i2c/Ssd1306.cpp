#include "i2c/Ssd1306.h"

using namespace I2c;

Ssd1306::Ssd1306(uint gpioSda, uint gpioScl, i2c_inst_t *busInstance) : Base(SSD1306_ADDR,
                                                                           gpioSda,
                                                                           gpioScl,
                                                                           busInstance)
{
    const uint8_t payload[] = {
        static_cast<uint8_t>(ControlByte::NO_CO_COMMAND),
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
        static_cast<uint8_t>(Command::ENTIRE_DISPLAY_RESUME),
        static_cast<uint8_t>(Command::NORMAL_DISPLAY),
        static_cast<uint8_t>(Command::DISPLAY_ON)};

    writeBlocking(payload, sizeof(payload), false);
}

void Ssd1306::setData(std::string s, size_t startPos)
{
    m_data_left_ptr = startPos + 1; // Must account for the first Co byte
    Font8x8::getBmp(m_data, s, &m_data_left_ptr, &m_data_right_ptr);
}

int Ssd1306::writeData()
{
    m_data[0] = static_cast<uint8_t>(ControlByte::NO_CO_DATA);
    return writeBlocking(m_data, TOTAL_SEGMENTS + 1);
}

void Ssd1306::clearData()
{
    for (size_t i = m_data_left_ptr; i <= m_data_right_ptr; i++)
    {
        m_data[i] = 0x00;
    }

    m_data_left_ptr = 0;
    m_data_right_ptr = TOTAL_SEGMENTS;
}

int Ssd1306::writeRaw(const uint8_t *src, size_t len)
{
    // 8 (Pages) * 128 (Segments per Page) in bytes
    const size_t REQUIRED_SIZE = NUM_PAGES * SEGMENTS_PER_PAGE;
    if (len != REQUIRED_SIZE)
    {
        return -1;
    }

    uint8_t payload[len + 1];
    payload[0] = static_cast<uint8_t>(ControlByte::NO_CO_DATA);
    memcpy(payload + 1, src, len);
    return writeBlocking(payload, len + 1);
}