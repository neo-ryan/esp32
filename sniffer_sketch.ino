#include <WiFi.h>
#include "esp_wifi.h"

typedef struct {
    uint16_t frame_control;
    uint16_t duration_id;
    uint8_t  addr1[6];
    uint8_t  addr2[6];
    uint8_t  addr3[6];
    uint16_t sequence_ctrl;
} __attribute__((packed)) ieee80211_hdr_t;

uint8_t current_channel = 1;

void promiscuous_rx_cb(void* buf, wifi_promiscuous_pkt_type_t type) {
    wifi_promiscuous_pkt_t* pkt = (wifi_promiscuous_pkt_t*)buf;
    ieee80211_hdr_t* hdr = (ieee80211_hdr_t*)pkt->payload;

    int rssi = pkt->rx_ctrl.rssi;

    Serial.printf("[Ch %2d | RSSI: %3d dBm] SRC: %02X:%02X:%02X:%02X:%02X:%02X -> DST: %02X:%02X:%02X:%02X:%02X:%02X\r\n",
                  current_channel,
                  rssi,
                  hdr->addr2[0], hdr->addr2[1], hdr->addr2[2],
                  hdr->addr2[3], hdr->addr2[4], hdr->addr2[5],
                  hdr->addr1[0], hdr->addr1[1], hdr->addr1[2],
                  hdr->addr1[3], hdr->addr1[4], hdr->addr1[5]);
}

void setup() {
    Serial.begin(115200);
    delay(1000);

    WiFi.mode(WIFI_STA);
    WiFi.disconnect();

    esp_wifi_set_promiscuous(true);
    esp_wifi_set_promiscuous_rx_cb(&promiscuous_rx_cb);

    esp_wifi_set_channel(current_channel, WIFI_SECOND_CHAN_NONE);

    Serial.println("\n--- Ativo ---");
}

void loop() {
    vTaskDelay(500 / portTICK_PERIOD_MS);

    current_channel++;
    if (current_channel > 13) {
        current_channel = 1;
    }

    esp_wifi_set_channel(current_channel, WIFI_SECOND_CHAN_NONE);
}