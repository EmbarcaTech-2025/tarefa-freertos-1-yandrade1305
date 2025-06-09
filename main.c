#include "FreeRTOS.h"
#include "task.h"
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"

#define RED_PIN 13           
#define GREEN_PIN 11         
#define BLUE_PIN 12          
#define BUZZER1_PIN 10      
#define BUZZER2_PIN 21       
#define BUTTON_A_PIN 5
#define BUTTON_B_PIN 6

TaskHandle_t led_task_handle = NULL;
TaskHandle_t buzzer_task_handle = NULL;

uint32_t last_button_a_time = 0;
uint32_t last_button_b_time = 0;
const uint32_t debounce_delay = 200;

void led_task(void *pvParameters) {
    gpio_init(RED_PIN);
    gpio_init(GREEN_PIN);
    gpio_init(BLUE_PIN);
    
    gpio_set_dir(RED_PIN, GPIO_OUT);
    gpio_set_dir(GREEN_PIN, GPIO_OUT);
    gpio_set_dir(BLUE_PIN, GPIO_OUT);
    
    uint8_t current_color = 0;
    
    while (true) {
        gpio_put(RED_PIN, 0);
        gpio_put(GREEN_PIN, 0);
        gpio_put(BLUE_PIN, 0);
        
        switch(current_color) {
            case 0:
                gpio_put(RED_PIN, 1);
                break;
            case 1:
                gpio_put(GREEN_PIN, 1);
                break;
            case 2:
                gpio_put(BLUE_PIN, 1);
                break;
        }
        
        current_color = (current_color + 1) % 3;
        
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

void buzzer_task(void *pvParameters) {
    gpio_init(BUZZER1_PIN);
    gpio_set_dir(BUZZER1_PIN, GPIO_OUT);

    gpio_init(BUZZER2_PIN);
    gpio_set_dir(BUZZER2_PIN, GPIO_OUT);
    
    while (true) {
        gpio_put(BUZZER1_PIN, 1);
        gpio_put(BUZZER2_PIN, 1);
        vTaskDelay(pdMS_TO_TICKS(100));
        
        gpio_put(BUZZER1_PIN, 0);
        gpio_put(BUZZER2_PIN, 0);
        vTaskDelay(pdMS_TO_TICKS(900));
    }
}

void button_task(void *pvParameters) {
    gpio_init(BUTTON_A_PIN);
    gpio_init(BUTTON_B_PIN);
    
    gpio_set_dir(BUTTON_A_PIN, GPIO_IN);
    gpio_set_dir(BUTTON_B_PIN, GPIO_IN);
    
    gpio_pull_up(BUTTON_A_PIN);
    gpio_pull_up(BUTTON_B_PIN);
    
    bool led_task_suspended = false;
    bool buzzer_task_suspended = false;
    
    while (true) {
        if (gpio_get(BUTTON_A_PIN) == 0) {
            uint32_t current_time = to_ms_since_boot(get_absolute_time());
            if ((current_time - last_button_a_time) > debounce_delay) {
                last_button_a_time = current_time;
                
                if (led_task_suspended) {
                    vTaskResume(led_task_handle);
                    printf("LED task resumed\n");
                } else {
                    vTaskSuspend(led_task_handle);
                    gpio_put(RED_PIN, 0);
                    gpio_put(GREEN_PIN, 0);
                    gpio_put(BLUE_PIN, 0);
                    printf("LED task suspended\n");
                }
                led_task_suspended = !led_task_suspended;
            }
        }
        
        if (gpio_get(BUTTON_B_PIN) == 0) {
            uint32_t current_time = to_ms_since_boot(get_absolute_time());
            if ((current_time - last_button_b_time) > debounce_delay) {
                last_button_b_time = current_time;
                
                if (buzzer_task_suspended) {
                    vTaskResume(buzzer_task_handle);
                    printf("Buzzer task resumed\n");
                } else {
                    vTaskSuspend(buzzer_task_handle);
                    gpio_put(BUZZER1_PIN, 0);
                    gpio_put(BUZZER2_PIN, 0);
                    printf("Buzzer task suspended\n");
                }
                buzzer_task_suspended = !buzzer_task_suspended;
            }
        }
        
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

int main() {
    stdio_init_all();
    
    xTaskCreate(led_task, "LED Task", 256, NULL, 1, &led_task_handle);
    xTaskCreate(buzzer_task, "Buzzer Task", 256, NULL, 1, &buzzer_task_handle);
    xTaskCreate(button_task, "Button Task", 256, NULL, 2, NULL);

    vTaskStartScheduler();
}