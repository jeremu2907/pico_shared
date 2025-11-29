#pragma once

#include <functional>
#include <vector>
#include <cstdio>
#include <map>
#include "pico/stdlib.h"

#include "Macro.hpp"
#include "BoardType.h"

#if PICO_BOARD_TYPE == PICO_W
#include "pico/cyw43_arch.h"
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