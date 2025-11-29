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

        void setHigh();
        void setLow();

    private:
        void init() override;
        void put(bool high);
    };
}