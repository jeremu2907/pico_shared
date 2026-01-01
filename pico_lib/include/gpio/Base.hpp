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
        inline static std::map<uint, bool> s_claimedPinMap{};

    public:

        /// @param gpio Gpio number
        Base(uint gpio);

        ~Base();

        /// @return Returns the gpio number
        uint gpio() const;

        /// @brief Initializes the gpio pin
        virtual void init();

        /// @brief Turns onboard LED ON for non-W models
        static void onboardLedOn();

        /// @brief Turns onboard LED OFF for non-W models
        static void onboardLedOff();

    protected:
        uint m_gpio;
        bool m_out;
        bool m_high;
    };
}