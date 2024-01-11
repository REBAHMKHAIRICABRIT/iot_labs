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
#include "my_helper_fct.h"

#include "wifi_connect.h"
#include "esp_http_client.h"

#include "http_data.h"

static const char *TAG = "WIFI_LAB";




static const uint32_t STACK_SIZE = 3*1024;
static const uint32_t CONNECTED_WIFI_PRIORITY = 5;

/**
 * @brief Starting point function
 * 
 */


void vTaskConnectedWifi(void *pvParameters);
void MyApplication(){
  printf("lab7 wifi_connected \n\r");
  http_param_t param;
  fetchHttpData(&param,"http://www.google.com");



  
}
void app_main() {
  /* ERROR, WARNING, INFO level log */
  esp_log_level_set(TAG, ESP_LOG_INFO);
  
  /* Init WiFi */
	wifiInit();

	vTaskSuspendAll();



  /* Create connected WiFi Task, STACK=3*1024, Priority=5 */
	xTaskCreatePinnedToCore(vTaskConnectedWifi, "Connected Wifi", STACK_SIZE, (void *)"Connected Wifi", CONNECTED_WIFI_PRIORITY, NULL, CORE_0);

	xTaskResumeAll();

  /* Delete task */
  vTaskDelete(NULL);
}

void vTaskConnectedWifi(void *pvParameters){

	for(;;){
		if(xSemaphoreTake(getConnectionWifiSemaphore(), pdMS_TO_TICKS(10000)) == pdTRUE){
			DISPLAY("Connected on %s", WIFI_SSID);
			DISPLAY("Run Application");
			MyApplication();
			if(xSemaphoreTake(getConnectionWifiSemaphore(), portMAX_DELAY) == pdTRUE){
				DISPLAY("Retried connection on %s", WIFI_SSID);
			}
		}
		else{
			DISPLAY("Failed to connect. Retry in");
			for(int i = 1 ; i <= 5; i++){
				DISPLAY("... %d", i);
				vTaskDelay(pdMS_TO_TICKS(1000));
			}
			esp_restart();
		}
	}
}
