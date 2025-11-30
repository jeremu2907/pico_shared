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
        inline static std::map<uint, bool> s_claimedPinMap{};
        inline static std::vector<Input *> s_inputQueue = {};
        inline static bool s_init = false;
        inline static bool s_running = true;

    public:
        explicit Input(uint gpio, uint adcIdx);
        ~Input();

        void init();

        static void runLoop();

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