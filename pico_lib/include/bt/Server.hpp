#pragma once

#include "pico/stdlib.h"
#include "btstack.h"
#include "pico/cyw43_arch.h"
#include "pico/btstack_cyw43.h"
#include "hardware/adc.h"

#define ADVERTISE_DATA 0x02, BLUETOOTH_DATA_TYPE_FLAGS, APP_AD_FLAGS, 0x17, BLUETOOTH_DATA_TYPE_COMPLETE_LOCAL_NAME, 'P', 'i', 'c', 'o', ' ', '0', '0', ':', '0', '0', ':', '0', '0', ':', '0', '0', ':', '0', '0', ':', '0', '0', 0x03, BLUETOOTH_DATA_TYPE_COMPLETE_LIST_OF_16_BIT_SERVICE_CLASS_UUIDS, 0x1a, 0x18

extern uint8_t const profile_data[];

namespace Bt
{
    class Server
    {
    public:
        inline static const uint HEARTBEAT_PERIOD_MS = 1000;
        inline static const uint8_t APP_AD_FLAGS = 0x06;
        static uint8_t s_advertiseData[];
        static uint8_t s_advertiseDataLen;
        static int s_notificationEnabled;
        static hci_con_handle_t s_conHandle;
        static btstack_timer_source_t s_heartbeat;
        static btstack_packet_callback_registration_t s_hciEventCallbackRegistration;

        static void packetHandler(uint8_t packet_type, uint16_t channel, uint8_t *packet, uint16_t size);
        static uint16_t attReadCallback(hci_con_handle_t connection_handle, uint16_t att_handle, uint16_t offset, uint8_t *buffer, uint16_t buffer_size);
        static int attWriteCallback(hci_con_handle_t connection_handle, uint16_t att_handle, uint16_t transaction_mode, uint16_t offset, uint8_t *buffer, uint16_t buffer_size);
        static void heartbeatHandler(struct btstack_timer_source *ts);
    };
}