/****************************************************************************
 * Copyright (C) 2021 by Fabrice Muller                                     *
 *                                                                          *
 * This file is useful for ESP32 Design course.                             *
 *                                                                          *
 ****************************************************************************/

/**
 * @file main.c
 * @author Fabrice Muller
 * @date 12 Nov. 2021
 * @brief File containing the lab1-1 of Part 7.
 *
 * @see https://github.com/fmuller-pns/esp32-vscode-project-template
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_netif.h"
#include "mqtt_tcp.h"
#include "wifi_connect.h"

static const char *TAG1 = "WIFI_LAB";
static const char *TAG2 = "MQTT_MAIN";

void vConnected_wifi_Task(void *pvParameters);
	esp_mqtt_client_handle_t client_MQTT;

/**
 * @brief Starting point function
 * MAC Address : d8-a0-1d-5e-19-64
 * bssid = b8:27:eb:f2:0c:d2
 * sta ip: 10.3.141.220, mask: 255.255.255.0, gw: 10.3.141.1
 * 
 */


void testMqttTask(void *para);

/**
 * @brief Wifi Task example to use the wifi connection 
 * 
 * @param para parameter of the task
 */

void app_main() {
  /* ERROR, WARNING, INFO level log */
  esp_log_level_set(TAG1, ESP_LOG_INFO);
  
  /* Init WiFi */
  wifiInit();


  /* Create connected WiFi Task, STACK=3*1024, Priority=5 */
  xTaskCreate(vConnected_wifi_Task, "Connected_Wifi_Task", 3072, NULL, 5, NULL);

  /* Delete task */
  vTaskDelete(NULL);
}

//TODO Prendre le semaphore dans connected wifi task
void vConnected_wifi_Task(void *pvParameters)
{
	BaseType_t is_semaphore_taken = pdTRUE;

	for(;;)
	{
		is_semaphore_taken = xSemaphoreTake(getConnectionWifiSemaphore(), pdMS_TO_TICKS(10000));

		if (is_semaphore_taken == pdTRUE)
		{
			printf("Connected on %s\n", WIFI_SSID);
			printf("Run application\n");
			/* Start MQTT client */
			mqtt_start("mqtt://134.59.213.29:1883");
			/* Get MQTT connection event (semaphore) */
			esp_log_level_set(TAG1, ESP_LOG_INFO);
			is_semaphore_taken = xSemaphoreTake(getConnectionMqttSemaphore(), pdMS_TO_TICKS(10000));
			/* Wait on MQTT Connected event */
			if (is_semaphore_taken == pdTRUE)
			{
				printf("Connected on MQQTT\n");
				xTaskCreate(testMqttTask, "Connected_MQQT_Task", 3072, client_MQTT, 6, NULL);
			}
			else
			{
				printf("Semaphore MQTT isn't taken\n");
				for (uint8_t i = 1; i <= 5; i++)
				{
					printf("%d/5\n", i);
					vTaskDelay(pdMS_TO_TICKS(500));
				}
			}
			vTaskDelete(NULL);
			xSemaphoreTake(getConnectionWifiSemaphore(), portMAX_DELAY);
			printf("Connection lost\nRetry to connect...\n");
		}
		else
		{
			printf("Semaphore isn't taken\n");
			for (uint8_t i = 1; i <= 5; i++)
			{
				printf("%d/5\n", i);
				vTaskDelay(pdMS_TO_TICKS(500));
			}
			esp_restart();
		}
	}
}


void testMqttTask(void *para) {
const char* AREA1,AREA2,E110,S113; 
//   /* Get MQTT Client passed by task parameter */
//      esp_mqtt_client_handle_t mqtt_client = (esp_mqtt_client_handle_t)para;

//    /* Subscription of the End-Node : Sub_AREA1 */
//   esp_mqtt_client_subscribe(mqtt_client, AREA1,1);


//    /* Publishing temperature with the End-Node : Pub_E110_Room */
//   esp_mqtt_client_publish(mqtt_client, E110);


//    /* Wait 3 sec. */
//    vTaskDelay(pdMS_TO_TICKS(3000));

//   /* Publishing humidity AND temperature with the End-Node : Pub_Sxxx_Room */


//   /* Subscription of the End-Node : Sub_E110 */
//      esp_mqtt_client_subscribe(mqtt_client, E110,1);

//   /* Publishing temperature with the End-Node : Pub_E110_Room */
//   esp_mqtt_client_publish(mqtt_client, E110);


//   /* Wait 3 sec. */
//    vTaskDelay(pdMS_TO_TICKS(500));

//   /* Publishing humidity AND temperature with the End-Node : Pub_Sxxx_Room */


//   /* Unsubscribe of the End-Node : Sub_AREA1 */
//  esp_mqtt_client_unsubscribe(mqtt_client, AREA1);


//    /* Publishing temperature with the End-Node : Pub_E110_Room */
//    esp_mqtt_client_publish(mqtt_client, E110);

//    /* Wait 3 sec. */
//   vTaskDelay(pdMS_TO_TICKS(500));

//    /* Publishing humidity with the End-Node : Pub_Sxxx_Room */
//   esp_mqtt_client_publish(mqtt_client, S113);


   vTaskDelete(NULL);
 }
