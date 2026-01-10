#pragma once

#include "pico/stdlib.h"

#include "Base.hpp"

namespace Gpio
{
    class Output : public Base
    {
    public:
        explicit Output(uint gpio);
        ~Output() = default;

        /// @brief Sets gpio voltage level High
        void setHigh();

        /// @brief Sets gpio voltage level Low
        void setLow();

    private:
        void init() override;
        void put(bool high);
    };
}