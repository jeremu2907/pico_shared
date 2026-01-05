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
        explicit Input(uint gpio);
        ~Input();

        static void runLoop();

        template <typename Func>
        void installCallbackHigh(Func f)
        {
            m_callbackHigh = f;
        }

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