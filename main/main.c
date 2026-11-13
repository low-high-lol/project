#include <stdint.h>
#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <driver/spi_master.h>
#include <driver/gpio.h>

#include <esp_wifi.h>
#include <esp_event.h>
#include <nvs_flash.h>
#include <mqtt_client.h>

#include "aht_driver.h"
#include <driver/i2c_master.h>

#include <dht.h>
#include "esp_err.h"
#include "float_to_buffer.h"
#include "display_driver.h"


#define CONTROL_BUTTON_PIN GPIO_NUM_0 //esp32 own button (boot)
// #define SENSOR_PIN GPIO_NUM_19 // dht22 pin (it operates with single pin)


#define WIFI_SSID "CasaGodoy2.4"
#define WIFI_PASSWORD "casagodoy2022"
#define MQTT_USER "danil"
#define MQTT_PASSWORD "xperia-mqtt-2026"

static bool wifi_connected = false;
static bool mqtt_started = false;


// this is wifi event handler
static void wifi_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void* event_data)
{

    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START)
    {
        printf("Connecting to wifi \n");
        esp_wifi_connect();
    }

    else if (event_base ==  WIFI_EVENT && event_id == WIFI_EVENT_STA_CONNECTED) {
        printf("Connected! \n");
        
    }

    else if (event_base ==  IP_EVENT && event_id == IP_EVENT_STA_GOT_IP)
    {
        printf("Got ip \n");
        wifi_connected = true;
    }

    else if (event_base ==  WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        printf("Disconnnected \n");
        wifi_connected = false;
        esp_wifi_connect();
    }
}; 


// This is mqtt event handler
static void mqtt_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{


    esp_mqtt_event_handle_t event = (esp_mqtt_event_handle_t) event_data;
    esp_mqtt_client_handle_t mqtt_handle = event->client;

    if (event_id == MQTT_EVENT_CONNECTED)
    {
        char message[] = "Sensor connected";
        esp_mqtt_client_publish(mqtt_handle, "/temp" ,message,36, 2, 1 );
        printf("Подключен к mqtt \n");

        
    };
    if (event_id == MQTT_EVENT_PUBLISHED)
    {
        printf("Сообщение отправлено \n");
        
        
    };

};


// We will fill those floats and ints with our data by reading the sensor
float temperature = 0;
float humidity = 0;

uint32_t temperature_raw = 0; 
uint32_t humidity_raw = 0;



void app_main(void)
{

    // Preparing all the components for wifi and mqtt - flash, netif and event loop

    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_sta();

     //Configurating wifi, registering handler

    wifi_init_config_t init_config = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&init_config));
    esp_wifi_set_mode(WIFI_MODE_STA);

    wifi_config_t wifi_config = 
    {
        .sta = 
        {
            .ssid = WIFI_SSID,
            .password = WIFI_PASSWORD
        },
    };
    
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA,&wifi_config));

    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, NULL, NULL));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT, ESP_EVENT_ANY_ID, &wifi_event_handler, NULL, NULL));

    // configuring mqtt, refgistering events

    esp_mqtt_client_config_t mqtt_config = 
    {
        .broker.address.uri = "mqtts://xperia-n8n.tailefa49a.ts.net:8443",
        .credentials.client_id = "esp_temperature",
        .credentials.username =  MQTT_USER,
        .credentials.authentication.password = MQTT_PASSWORD,
        .session.last_will.msg = "Sensor disconnected",
        .session.last_will.topic = "/temp",
        .session.last_will.qos = 1
    };

    esp_mqtt_client_handle_t mqtt_handle = esp_mqtt_client_init(&mqtt_config);
    ESP_ERROR_CHECK(esp_mqtt_client_register_event(mqtt_handle, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL));


    // start wifi
    ESP_ERROR_CHECK(esp_wifi_start()); 


    // Launch spi bus & add the display on it
    ESP_ERROR_CHECK(start_device_spi()); 
    
    aht_configure();
 
    // Initial config of the display - resetting, setting landscape orientation and RGB pixel format
    ESP_ERROR_CHECK(spi_display_prepare());

    // main cycle
    while(1)
    {
        // we start mqtt if wifi is connected but mqtt is not
        if (wifi_connected == true  && mqtt_started == false)
        {

            ESP_ERROR_CHECK(esp_mqtt_client_start(mqtt_handle));
            mqtt_started = true;
        };


        if(gpio_get_level(CONTROL_BUTTON_PIN) == 0)
        {
            // This dht library function returns humidity and temperature from the dht22 sensor
            // ESP_ERROR_CHECK(dht_read_float_data(DHT_TYPE_AM2301, SENSOR_PIN, &humidity, &temperature)); 
            
            aht20_read_data(&temperature_raw, &temperature, &humidity_raw, &humidity);
            printf("H: %f \n",humidity);
            printf("T: %f,\n",temperature);

            //Creating the pixel buffer - 320 * 240 pixels, each pixel is 2 byte unsigned integer that represents an rgb colour 
            uint16_t* buffer_to_send = float_to_pixel_buffer('h', humidity);

            //Sending the created buffer to the display
            spi_display_paint(0, 319, 0, 119 ,buffer_to_send);
            
            buffer_to_send = float_to_pixel_buffer('t', temperature);


            spi_display_paint(0, 319, 120, 239 ,buffer_to_send);
            



            if(mqtt_started == true)
            {
                char message[64];
                snprintf(message, sizeof(message) ,"H: %.2f, T:%.2f", humidity, temperature);
                esp_mqtt_client_publish(mqtt_handle, "/temp" , message,0, 2, 1 );
                printf("Сообщение отправлено \n");
            };
        };

        // delay to register only the first button press
        vTaskDelay(pdMS_TO_TICKS(200));
    };


};