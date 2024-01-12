/****************************************************************************
 * Copyright (C) 2021 by Fabrice Muller                                     *
 *                                                                          *
 * This file is useful for ESP32 Design course.                             *
 *                                                                          *
 ****************************************************************************/

/**
 * @file mqtt_tcp.c
 * @author Fabrice Muller
 * @date 12 Nov. 2021
 * @brief File containing the lab1 of Part 8.
 *
 * @see https://github.com/fmuller-pns/esp32-vscode-project-template
 */

#include "mqtt_tcp.h"

static const char *TAG = "MQTT_TCP";

/* Local communication */
static xSemaphoreHandle connectionMqttSemaphore;

/**
 * @brief Get the connected MQTT Semaphore
 *
 * @return xSemaphoreHandle connected semaphore
 */
xSemaphoreHandle getConnectionMqttSemaphore() {
    return connectionMqttSemaphore;
}

/**
 * @brief Call-back handler for MQTT events
 *
 * @param handler_args arguments
 * @param base event base
 * @param event_id event id
 * @param event_data event data
 */
void mqtt_event_handler_cb(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data) {
    esp_mqtt_event_handle_t event = (esp_mqtt_event_handle_t)event_data;

    switch (event_id) {

    /* Connection */
    case MQTT_EVENT_CONNECTED:
        ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
        xSemaphoreGive(connectionMqttSemaphore);
        break;
    case MQTT_EVENT_DISCONNECTED:
        ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
        break;

    /* Subscription event type */
    case MQTT_EVENT_SUBSCRIBED:
        ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_UNSUBSCRIBED:
        ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_DATA:
        ESP_LOGI(TAG, "MQTT_EVENT_DATA");
        printf("TOPIC=%.*s\r\n", event->topic_len, event->topic);
        printf("DATA=%.*s\r\n", event->data_len, event->data);
        break;

    /* Publication event type */
    case MQTT_EVENT_PUBLISHED:
        ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
        break;

    /* Error event */
    case MQTT_EVENT_ERROR:
        ESP_LOGI(TAG, "MQTT_EVENT_ERROR");
        if (event->error_handle->error_type == MQTT_ERROR_TYPE_TCP_TRANSPORT) {
            if (event->error_handle->esp_tls_last_esp_err != 0)
                ESP_LOGE(TAG, "Last error %s: 0x%x", "reported from esp-tls", event->error_handle->esp_tls_last_esp_err);
            if (event->error_handle->esp_tls_stack_err != 0)
                ESP_LOGE(TAG, "Last error %s: 0x%x", "reported from tls stack", event->error_handle->esp_tls_stack_err);
            if (event->error_handle->esp_transport_sock_errno != 0)
                ESP_LOGE(TAG, "Last error %s: 0x%x", "captured as transport's socket errno", event->error_handle->esp_transport_sock_errno);
            ESP_LOGI(TAG, "Last errno string (%s)", strerror(event->error_handle->esp_transport_sock_errno));
        }
        break;
    default:
        ESP_LOGI(TAG, "Other event id:%d", event->event_id);
        break;
    }
}

/**
 * @brief MQTT start function
 * 
 * @param url broker address (example: "mqtt://10.3.141.1:1883")
 * @return esp_mqtt_client_handle_t reference to the client handler created
 */
esp_mqtt_client_handle_t mqtt_start(const char *url) {
    esp_err_t result;

    /* Creates MQTT semaphore as a connected event to MQTT broker */
    connectionMqttSemaphore = xSemaphoreCreateBinary();

    /* Creates MQTT client */
    esp_mqtt_client_config_t mqtt_cfg = {
        .uri = url,
    };
    esp_mqtt_client_handle_t mqtt_client = esp_mqtt_client_init(&mqtt_cfg);

    /* Registers MQTT events */
    result = esp_mqtt_client_register_event(mqtt_client, ESP_EVENT_ANY_ID, mqtt_event_handler_cb, mqtt_client);
    if (result != ESP_OK)
        return NULL;

    /* Starts MQTT client */
    result = esp_mqtt_client_start(mqtt_client);
    if (result != ESP_OK)
        return NULL;
    printf("Connected on MQQTTSSSSSSSSSSSSSSSSSSSS\n");

    return mqtt_client;
}
