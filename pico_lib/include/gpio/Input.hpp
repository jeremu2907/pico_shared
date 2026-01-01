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

        /// @brief Only call once between MAIN_LOOP_START and MAIN_LOOP_END
        /// used to poll and read gpio input voltage level and executes corresponding
        /// callback
        static void runLoop();

        /// @brief Installs callback to be run when input gpio reads high
        /// @tparam Func
        /// @param f a function of type void()
        template <typename Func>
        void installCallbackHigh(Func f)
        {
            m_callbackHigh = f;
        }

        /// @brief Installs callback to be run when input gpio reads low
        /// @tparam Func 
        /// @param f a function of type void()
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