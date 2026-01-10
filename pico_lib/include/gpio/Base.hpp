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
        /// @brief 
        /// @param gpio 
        Base(uint gpio);

        ~Base();

        /// @brief 
        /// @return gpio
        uint gpio() const;

        /// @brief
        virtual void init(){};

    protected:
        uint m_gpio;
    };
}