#pragma once

#include <functional>
#include <stdint.h>
#include <thread>
#include <cstdio>
#include <atomic>
#include <map>
#include "hardware/adc.h"
#include "pico/stdlib.h"

#include "Macros.hpp"
#include "Constants.hpp"

namespace Adc
{
    class Input
    {
    public:
        static inline constexpr uint GPIO_26_ADC = 0;
        static inline constexpr uint GPIO_27_ADC = 1;
        static inline constexpr uint GPIO_28_ADC = 2;
        static inline constexpr float PICO_2_W_ADC_RESOLUTION = 4095.0f;

    private:
        inline static std::map<uint, bool> m_sClaimedPinMap{};
        inline static std::vector<Input *> m_sInputQueue = {};
        inline static bool m_sInit = false;
        inline static bool m_sRunning = true;

    public:
        explicit Input(uint gpio, uint adcIdx);
        ~Input();

        void init();

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
        void callback(float voltage);

        std::function<void(float voltage)> m_callback;
        uint m_gpio;
        uint m_adcIdx;
    };
}