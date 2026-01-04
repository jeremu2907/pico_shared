#pragma once

#include <functional>
#include <vector>
#include <cstdio>
#include <map>
#include "pico/stdlib.h"

#include "Macros.hpp"

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
        virtual void init();

        static void onboardLedOn();
        static void onboardLedOff();

    protected:
        uint m_gpio;
        bool m_out;
        bool m_high;
    };
}