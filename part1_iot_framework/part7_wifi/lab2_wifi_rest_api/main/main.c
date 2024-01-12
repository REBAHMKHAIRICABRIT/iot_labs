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

#include "cJSON.h"

static const char *TAG = "WIFI_LAB";

/* openweathermap API URL for Cannes city, Unit = degree */
const char *CITY = "Cannes";
const char *OPEN_WEATHER_MAP_URL = "http://api.openweathermap.org/data/2.5/weather?q=Cannes&appid=bfaf90865d45e39c390da17ffa61e195&units=metric";



static const uint32_t STACK_SIZE = 3*1024;
static const uint32_t CONNECTED_WIFI_PRIORITY = 5;

/* Sensor information */
# define WEATHERMAPINFO_DESCRIPTION_LENGTH 100

/**
 * @brief Starting point function
 * 
 */

typedef struct {
  float latitude;
  float longitude;
  float temp;
  float feels_like;
  float temp_min;
  float temp_max;
  char description[WEATHERMAPINFO_DESCRIPTION_LENGTH];

} weathermapinfo_t;


void vTaskConnectedWifi(void *pvParameters);

void extractJSONWeatherMapInformation(char *resp, weathermapinfo_t *weathermapinfo) {

    /* Convert textual resp to JSON object */
    cJSON *payload = cJSON_Parse(resp);

    /* Coordonate (JSon Items)43.550000,7.016700 */
    cJSON *coord = cJSON_GetObjectItem(payload, "coord");   
    cJSON *longitude = cJSON_GetObjectItem(coord, "lon");
    cJSON *latitude = cJSON_GetObjectItem(coord, "lat");
    cJSON *maini = cJSON_GetObjectItem(payload, "main"); 
    cJSON *temp = cJSON_GetObjectItem(maini, "temp");
    cJSON *feels_like = cJSON_GetObjectItem(maini, "feels_like");
    cJSON *temp_min = cJSON_GetObjectItem(maini, "temp_min");
    cJSON *temp_max = cJSON_GetObjectItem(maini, "temp_max");
    cJSON *description = cJSON_GetObjectItem(cJSON_GetObjectItem(payload, "weather")->child, "description");


    /* Set information in the structure */
    weathermapinfo->latitude = latitude->valuedouble;
    weathermapinfo->longitude = longitude->valuedouble;   
    weathermapinfo->temp = temp->valuedouble; 
    weathermapinfo->feels_like = feels_like->valuedouble;   
    weathermapinfo->temp_min = temp_min->valuedouble;   
    weathermapinfo->temp_max = temp_max->valuedouble;  
    strcpy(weathermapinfo->description, description->valuestring);
       

    /* Free memory */
    cJSON_Delete(payload);
}

void MyApplication(){
  weathermapinfo_t weathermapinfo;
  printf("lab7 wifi_connected \n\r");
  http_param_t param;
  fetchHttpData(&param,"http://api.openweathermap.org/data/2.5/weather?q=Cannes&appid=bfaf90865d45e39c390da17ffa61e195");
  extractJSONWeatherMapInformation(param.buffer, &weathermapinfo);
  printf("Températures : %f°C\n%f°C\n%f°C\n%f°C\n\nDescription : %s", weathermapinfo.temp, weathermapinfo.feels_like, weathermapinfo.temp_min, weathermapinfo.temp_max, weathermapinfo.description);
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


