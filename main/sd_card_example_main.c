/* SD card and FAT filesystem example.
   This example uses SPI peripheral to communicate with SD card.

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <string.h>
#include <stdio.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <string.h>
#include <esp_timer.h>
#include <esp_vfs_fat.h>
#include <freertos/ringbuf.h>
#include <esp_timer.h>
// Include I2S driver
#include <driver/i2s.h>
#include "driver/sdmmc_types.h"
#include "esp_task_wdt.h"
// Incluse SD card driver
#include <../component/FileManager/sdcard1.h>
#include "esp_dsp.h"
#include <../component/processing_data/process.h>
SemaphoreHandle_t semaphorePPG = NULL;
SemaphoreHandle_t semaphorePCG = NULL;

const char* ppg_file = "ppg";
const char* pcg_file = "pcg";
// int ppg_arr[10] = {0};
// int pcg_arr[10] = {0};
char *returnPtr_ppg;
char *returnPtr_pcg;
long int index_ppg = 0;
long int index_pcg = 0;

#define size_process_data    1100
int windowsize = 10;

int ppg_data[size_process_data];
int pcg_data[size_process_data];

TaskHandle_t xTaskHandle_readPPG;
TaskHandle_t xTaskHandle_readPCG;
TaskHandle_t xTaskHandle_proccesing_data;
    // int arr[size];
int index_peaks_ppg[5];
int index_peaks_pcg[200];

int number_peaks_ppg = 2;
int number_peaks_pcg = 2;

int prominence = 10000;
 
int s1[20];
int s2[20];

int mean_data[size_process_data];// khai bao mang mean_data voi kich thuoc size
//

void printArray(int arr[], int size) 
{
    printf("Các giá trị trong mảng là:\n");
    for (int i = 0; i < size; i++) {
        printf("%d ", arr[i]);
    }
    printf("\n");
}
void read_filePPG_from_sdCard(void* parameter)
{
    int countPPG = 0;
    while(1)
    {
        
        if(xSemaphoreTake(semaphorePPG,portMAX_DELAY) == pdTRUE)
        {
            int ppg_sample;
            //ESP_LOGI(__func__,"PPG = %d", ppg_sample);
            if (countPPG < size_process_data) 
            {
                index_ppg++;
                for (int i = 0; i < size_process_data; i++) 
                {
                    ESP_ERROR_CHECK_WITHOUT_ABORT(sdcard_readDataFromFile(&index_ppg, ppg_file, "%d\n", &ppg_sample));
                    ppg_data[i] = ppg_sample;
                    //ESP_LOGI(__func__,"data PPG[%d] = %d",i , ppg_data[i]);
                    countPPG++;
                }
                //printf("countPPG = %d \n", countPPG);
            }
            else
            {
                int a =0;
                a++;
            }
            // ESP_LOGI(__func__,"dia chi returnPtr in ben ngoai= %p",returnPtr_ppg);
            xSemaphoreGive(semaphorePPG);
            vTaskDelay(pdMS_TO_TICKS(1000));
            //vTaskSuspend(xTaskHandle_readPPG);
            vTaskDelete(NULL);
        }
    }
}

void read_filePCG_from_sdCard(void* parameter)
{
    int countPCG = 0;
    while(1)
    {
        
        if(xSemaphoreTake(semaphorePCG,portMAX_DELAY) == pdTRUE)
        {
            int pcg_sample;
            //ESP_LOGI(__func__,"PPG = %d", ppg_sample);
            if (countPCG < size_process_data) 
            {
                index_pcg++;
                for (int i = 0; i < size_process_data; i++) 
                {
                    ESP_ERROR_CHECK_WITHOUT_ABORT(sdcard_readDataFromFile(&index_pcg, pcg_file, "%d\n", &pcg_sample));
                    pcg_data[i] = pcg_sample;
                    //ESP_LOGI(__func__,"data PcG[%d] = %d",i , pcg_data[i]);
                    countPCG++;
                }
                //printf("countPPG = %d \n", countPPG);
            }
            else
            {
                int a =0;
                a++;
            }
            // ESP_LOGI(__func__,"dia chi returnPtr in ben ngoai= %p",returnPtr_ppg);
            xSemaphoreGive(semaphorePCG);
            vTaskDelay(pdMS_TO_TICKS(1000));
            vTaskDelete(NULL);
            //vTaskSuspend(xTaskHandle_readPCG);
        }
    }
}
void processing_data(void* parameter)
{
    int oneTime = 1;
    xTaskHandle_proccesing_data = xTaskGetCurrentTaskHandle();
    while(1)
    {
        if(xSemaphoreTake(semaphorePPG,portMAX_DELAY) == pdTRUE 
            && xSemaphoreTake(semaphorePCG,portMAX_DELAY) == pdTRUE)
        {
            if(oneTime == 1)
            {
                moving_mean(ppg_data, size_process_data, windowsize, mean_data);
                ESP_LOGW(__func__,"find peak ppg");
                findpeaks(mean_data, size_process_data, index_peaks_ppg, &number_peaks_ppg, prominence);
                //esp_task_wdt_reset();
                ESP_LOGW(__func__,"find peak pcg");
                findpeaks(pcg_data, size_process_data, index_peaks_pcg, &number_peaks_pcg, 0);
                findTwoLargest(pcg_data, size_process_data, index_peaks_pcg, number_peaks_pcg, number_peaks_ppg, index_peaks_ppg, s1, s2, 150);
                xSemaphoreGive(semaphorePPG);
                xSemaphoreGive(semaphorePCG);
                oneTime = 0;
            }
            else
            {
                oneTime = 0;
                xSemaphoreGive(semaphorePPG);
                xSemaphoreGive(semaphorePCG);
            }
            //return(0);
            vTaskDelay(pdMS_TO_TICKS(100));
            xSemaphoreGive(semaphorePPG);
            xSemaphoreGive(semaphorePCG);
        }
        //vTaskDelete(NULL);
    }

}
void print_PPG(void* parameter)
{

}
void print_PCG(void* parameter)
{

}


void app_main(void)
{
    ESP_LOGI(__func__, "Initialize SD card with SPI interface.");
    esp_vfs_fat_mount_config_t mount_config_t = MOUNT_CONFIG_DEFAULT();
    spi_bus_config_t spi_bus_config_t = SPI_BUS_CONFIG_DEFAULT();
    vTaskDelay(pdMS_TO_TICKS(100));
    sdmmc_host_t host_t = SDSPI_HOST_DEFAULT();
    sdspi_device_config_t slot_config = SDSPI_DEVICE_CONFIG_DEFAULT();
    slot_config.gpio_cs = CONFIG_PIN_NUM_CS;
    slot_config.host_id = host_t.slot;
    
    sdmmc_card_t SDCARD;
    ESP_ERROR_CHECK(sdcard_initialize(&mount_config_t, &SDCARD, &host_t, &spi_bus_config_t, &slot_config));
    semaphorePPG = xSemaphoreCreateMutex();
    semaphorePCG = xSemaphoreCreateMutex();
    if (semaphorePPG == NULL) {
        ESP_LOGI(__func__,"Failed to create semaphorePPG\n");
    }
    if (semaphorePCG == NULL) {
        ESP_LOGI(__func__,"Failed to create semaphorePCG\n");
    }
    
    xTaskCreate(read_filePPG_from_sdCard,     // Hàm mà task sẽ chạy
                "read_filePPG_from_sdCard",            // Tên của task
                4096*4,              // Kích thước ngăn xếp (tính bằng từ)
                NULL,                // Tham số của task
                2,                   // Ưu tiên của task (1 là ưu tiên thấp nhất)
                &xTaskHandle_readPPG); 
    xTaskCreate(read_filePCG_from_sdCard,     // Hàm mà task sẽ chạy
                "read_filePCG_from_sdCard",            // Tên của task
                4096*4,              // Kích thước ngăn xếp (tính bằng từ)
                NULL,                // Tham số của task
                2,                   // Ưu tiên của task (1 là ưu tiên thấp nhất)
                &xTaskHandle_readPCG); 
    xTaskCreate(processing_data,     // Hàm mà task sẽ chạy
                "processing data",            // Tên của task
                4096*4,              // Kích thước ngăn xếp (tính bằng từ)
                NULL,                // Tham số của task
                1,                   // Ưu tiên của task (1 là ưu tiên thấp nhất)
                &xTaskHandle_proccesing_data);
    vTaskDelay(pdMS_TO_TICKS(100));
    //return(0);
}