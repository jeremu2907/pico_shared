#pragma once

#include <stdio.h>
#include <string.h>
#include <functional>

#include "pico/cyw43_arch.h"

namespace Wifi
{
    class ApScan
    {
    private:
        static const int MAX_RESULTS = 50;

    public:
        /// @brief
        static const int MAX_CONSECUTIVE_FAIL_SCANS = 10;

    public:
        /// @brief
        /// @param targetSsid
        ApScan(const char *targetSsid);

        /// @brief
        /// @return
        void init();

        /// @brief
        /// @return
        bool scan();

        /// @brief
        /// @return
        int getResultCount() const { return m_scanResultCount; }

        /// @brief
        /// @return
        int getTargetChannel() const { return m_knownChannel; }

        /// @brief
        void printResults() const;

        /// @brief
        /// @return
        bool isTargetFound() const { return m_targetFound; }

        /// @brief
        /// @param index
        /// @return
        const cyw43_ev_scan_result_t *getResult(int index) const;

    private:
        int scanAll();
        int scanForTarget();

        bool m_targetFound;
        int m_knownChannel;
        char m_targetSsid[33];
        int m_scanResultCount;
        bool m_useTargetedScan;
        int m_consecutiveFailures;
        cyw43_ev_scan_result_t m_scanResults[MAX_RESULTS];

        static ApScan *m_sCurrentInstance;
        static int scanCallback(void *env, const cyw43_ev_scan_result_t *result);
    };
}
