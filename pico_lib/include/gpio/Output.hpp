#pragma once

#include "pico/stdlib.h"
#include <functional>

#include "Base.hpp"

namespace Gpio
{
    class Output : public Base
    {
    public:
        explicit Output(uint gpio);
        ~Output() = default;

        /// @brief Sets gpio pin to HIGH voltage
        void setHigh();

        /// @brief Sets gpio pin to LOW voltage
        void setLow();

    private:
        void init() override;
        void put(bool high);
    };
}