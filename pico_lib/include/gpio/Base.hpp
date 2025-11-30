#pragma once

#include <functional>
#include <vector>
#include <cstdio>
#include <map>
#include "pico/stdlib.h"

#include "Macros.hpp"

#if IS_WIRELESS
#pragma message(">>> Compiling for PICO_W / PICO_2_W (cyw43 included)")
#include "pico/cyw43_arch.h"
#else
#pragma message(">>> Compiling for PICO / PICO_2 (cyw43 NOT included)")
#endif

namespace Gpio
{
    class Base
    {
    private:
        inline static std::map<uint, bool> s_claimedPinMap{};

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