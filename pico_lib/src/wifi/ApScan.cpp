#include "wifi/ApScan.hpp"
#include "Macros.hpp"

using namespace Wifi;

ApScan *ApScan::m_sCurrentInstance = nullptr;

ApScan::ApScan(const char *targetSsid)
    : m_scanResultCount(0),
      m_knownChannel(0),
      m_consecutiveFailures(0),
      m_useTargetedScan(true),
      m_targetFound(false)
{
    strncpy(m_targetSsid, targetSsid, sizeof(m_targetSsid) - 1);
    m_targetSsid[sizeof(m_targetSsid) - 1] = '\0';
    memset(m_scanResults, 0, sizeof(m_scanResults));

    m_sCurrentInstance = this;

    init();
}

void ApScan::init()
{
    if (cyw43_arch_init())
    {
        ERR_START
        printf("CYW43 init failed\n");
        ERR_END
    }

    cyw43_arch_enable_sta_mode();
    printf("Wi-Fi initialized for AP scanning.\n");
}

int ApScan::scanCallback(void *env, const cyw43_ev_scan_result_t *result)
{
    if (m_sCurrentInstance && result && m_sCurrentInstance->m_scanResultCount < MAX_RESULTS)
    {
        memcpy(&m_sCurrentInstance->m_scanResults[m_sCurrentInstance->m_scanResultCount],
               result,
               sizeof(cyw43_ev_scan_result_t));
        m_sCurrentInstance->m_scanResultCount++;
    }
    return 0;
}

int ApScan::scanForTarget()
{
    m_scanResultCount = 0;
    memset(m_scanResults, 0, sizeof(m_scanResults));

    cyw43_wifi_scan_options_t scanOptions = {0};

    // Scan for specific SSID only (much faster than full scan)
    scanOptions.ssid_len = strlen(m_targetSsid);
    memcpy(scanOptions.ssid, m_targetSsid, scanOptions.ssid_len);

    int err = cyw43_wifi_scan(&cyw43_state, &scanOptions, NULL, scanCallback);

    if (err == 0)
    {
        while(cyw43_wifi_scan_active(&cyw43_state))
        {
            cyw43_arch_poll();
            sleep_ms(1);
        }
        return m_scanResultCount;
    }

    return -1;
}

int ApScan::scanAll()
{
    m_scanResultCount = 0;
    memset(m_scanResults, 0, sizeof(m_scanResults));

    cyw43_wifi_scan_options_t scanOptions = {0};
    // Leave ssid_len = 0 to scan all SSIDs

    int err = cyw43_wifi_scan(&cyw43_state, &scanOptions, NULL, scanCallback);

    if (err == 0)
    {
        while(cyw43_wifi_scan_active(&cyw43_state))
        {
            cyw43_arch_poll();
            sleep_ms(1);
        }
        return m_scanResultCount;
    }

    return -1;
}

bool ApScan::scan()
{
    m_targetFound = false;

    if (m_useTargetedScan)
    {
        // printf("Quick Scan for '%s'\n", m_targetSsid);

        // Try targeted SSID scan first (faster)
        int found = scanForTarget();

        if (found > 0)
        {
            printf("Found via targeted scan!\n");

            m_knownChannel = m_scanResults[0].channel;
            // printf("Detected on channel: %d\n", m_knownChannel);

            m_consecutiveFailures = 0;
            m_targetFound = true;
        }
        else
        {
            printf("Not found in targeted scan\n");
            m_consecutiveFailures++;

            // After MAX_CONSECUTIVE_FAIL_SCANS failures, switch to full scan
            if (m_consecutiveFailures >= MAX_CONSECUTIVE_FAIL_SCANS)
            {
                // printf("Switching to full scan mode...\n");
                m_useTargetedScan = false;
                m_consecutiveFailures = 0;
            }
        }
    }
    else
    {
        // Full scan mode
        // printf("Full Scan (All Channels)\n");

        int found = scanAll();

        if (found > 0)
        {
            // Check if our target is in the results
            for (int i = 0; i < m_scanResultCount; i++)
            {
                char ssid[33];
                memcpy(ssid, m_scanResults[i].ssid, m_scanResults[i].ssid_len);
                ssid[m_scanResults[i].ssid_len] = '\0';

                if (strcmp(ssid, m_targetSsid) == 0)
                {
                    m_targetFound = true;
                    m_knownChannel = m_scanResults[i].channel;
                    // printf("Found '%s' on channel %d\n", m_targetSsid, m_knownChannel);

                    // Switch back to targeted scan now that we found it
                    m_useTargetedScan = true;
                    break;
                }
            }

            if (!m_targetFound)
            {
                printf("Target AP not in range\n");
            }
        }
        else
        {
            printf("Full scan failed\n");
        }
    }

    return m_targetFound;
}

void ApScan::printResults() const
{
    printf("\nFound %d access points:\n", m_scanResultCount);
    printf("%-32s | %-17s | Ch | RSSI | Security\n", "SSID", "BSSID");
    printf("----------------------------------------------------------------\n");

    for (int i = 0; i < m_scanResultCount; i++)
    {
        char ssid[33];
        memcpy(ssid, m_scanResults[i].ssid, m_scanResults[i].ssid_len);
        ssid[m_scanResults[i].ssid_len] = '\0';

        if (ssid[0] == '\0')
        {
            strcpy(ssid, "<Hidden Network>");
        }

        printf("%-32s | %02x:%02x:%02x:%02x:%02x:%02x | %2d | %4d | ",
               ssid,
               m_scanResults[i].bssid[0], m_scanResults[i].bssid[1],
               m_scanResults[i].bssid[2], m_scanResults[i].bssid[3],
               m_scanResults[i].bssid[4], m_scanResults[i].bssid[5],
               m_scanResults[i].channel,
               m_scanResults[i].rssi);

        uint32_t auth = m_scanResults[i].auth_mode;
        if (auth == CYW43_AUTH_OPEN)
        {
            printf("Open");
        }
        else if (auth & CYW43_AUTH_WPA2_AES_PSK)
        {
            printf("WPA2");
        }
        else if (auth & CYW43_AUTH_WPA_TKIP_PSK)
        {
            printf("WPA");
        }
        else if (auth & CYW43_AUTH_WPA2_MIXED_PSK)
        {
            printf("WPA/WPA2");
        }
        else
        {
            printf("Other (0x%x)", auth);
        }

        printf("\n");
    }
    printf("\n");
}

const cyw43_ev_scan_result_t *ApScan::getResult(int index) const
{
    if (index >= 0 && index < m_scanResultCount)
    {
        return &m_scanResults[index];
    }
    return nullptr;
}