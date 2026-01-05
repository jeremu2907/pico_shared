#pragma once

#include <map>

#include "pico/stdlib.h"

namespace Gpio
{
    class Base
    {
    private:
        inline static std::map<uint, bool> m_sClaimedPinMap{};

    public:
        Base(uint gpio);
        ~Base();

        uint gpio() const;
        virtual void init(){};

    protected:
        uint m_gpio;
    };
}