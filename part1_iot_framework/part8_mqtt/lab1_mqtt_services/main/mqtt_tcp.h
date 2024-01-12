/****************************************************************************
 * Copyright (C) 2021 by Fabrice Muller                                     *
 *                                                                          *
 * This file is useful for ESP32 Design course.                             *
 *                                                                          *
 ****************************************************************************/

/**
 * @file mqtt_tcp.h
 * @author Fabrice Muller
 * @date 12 Nov. 2021
 * @brief File containing the lab1 of Part 8.
 *
 * @see https://github.com/fmuller-pns/esp32-vscode-project-template
 */

#ifndef _MQTT_TCP_
#define _MQTT_TCP_

#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "esp_wifi.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_event.h"
#include "esp_netif.h"


#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"

#include "lwip/sockets.h"
#include "lwip/dns.h"
#include "lwip/netdb.h"

#include "esp_log.h"
#include "mqtt_client.h"

esp_mqtt_client_handle_t mqtt_start(const char *url);
xSemaphoreHandle getConnectionMqttSemaphore();

#endif