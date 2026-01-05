#pragma once

#include <functional>

#include <pico/stdlib.h>

#include "gpio/Base.hpp"

namespace Adc
{
    class Input : public Gpio::Base
    {
    private:
        inline static std::vector<Input *> m_sInputQueue = {};
        inline static bool m_sInit = false;
        inline static bool m_sRunning = true;

    public:
        explicit Input(uint gpio);
        ~Input();

        static void runLoop();

        /// @brief Installs callback on ADC input voltage is sampled
        /// @tparam Func of type void(float voltage)
        /// @param f callback takes in 1 arg that is the voltage float
        template <typename Func>
        void installCallback(Func f)
        {
            m_callback = f;
        }

    private:
        void init() override;
        uint getAdcIndex();

        void callback(float voltage);

        std::function<void(float voltage)> m_callback;
    };
}