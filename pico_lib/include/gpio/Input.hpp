#pragma once

#include <functional>

#include "Base.hpp"

namespace Gpio
{
    class Input : public Base
    {
    private:
        inline static std::vector<Input *> m_sInputQueue = {};
        inline static bool m_sRunning = true;

    public:
        /// @brief 
        /// @param gpio 
        explicit Input(uint gpio);

        ~Input();

        /// @brief 
        static void runLoop();

        /// @brief Callback ran when input is High
        /// @tparam Func 
        /// @param f 
        template <typename Func>
        void installCallbackHigh(Func f)
        {
            m_callbackHigh = f;
        }

        /// @brief Callback ran when input is Low
        /// @tparam Func 
        /// @param f 
        template <typename Func>
        void installCallbackLow(Func f)
        {
            m_callbackLow = f;
        }

    private:
        void init() override;

        void callbackLow();
        void callbackHigh();

        std::function<void()> m_callbackLow;
        std::function<void()> m_callbackHigh;
    };
}