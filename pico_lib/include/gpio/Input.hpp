#pragma once

#include <functional>
#include <thread>
#include <atomic>

#include "Base.hpp"

namespace Gpio
{
    class Input : public Base
    {
    private:
        inline static std::vector<Input *> s_inputQueue = {};
        inline static bool s_running = true;

    public:
        explicit Input(uint gpio);
        ~Input();

        void init() override;

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
        void callbackHigh();
        void callbackLow();

        std::function<void()> m_callbackHigh;
        std::function<void()> m_callbackLow;
    };
}