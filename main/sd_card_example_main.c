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

// Incluse SD card driver
#include <../component/FileManager/sdcard1.h>
#include "esp_dsp.h"
#include <../component/processing_data/process.h>
SemaphoreHandle_t semaphorePPG;
SemaphoreHandle_t semaphorePCG;

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
int countPPG = 0;
void read_filePPG_from_sdCard(void* parameter)
{
    while(1)
    {
        if(xSemaphoreTake(semaphorePPG,portMAX_DELAY))
        {
            int ppg_sample;
            sdcard_readDataFromFile(&number_peaks_ppg, ppg_file, "%d\n", &ppg_sample );
            ESP_LOGI(__func__,"PPG = %d", ppg_sample);
            countPPG++;
            if (countPPG < size_process_data) {
                // Mảng đã đầy, có thể xử lý hoặc thoát khỏi vòng lặp tại đây
                // Ví dụ:
                // Xử lý 100 giá trị đã đọc
                ppg_data[count] = ppg_sample;
                // for (int i = 0; i < 1000; i++) {
                //     printf("%d\n", ppg_arr[i]);
                // }
                // break;
            }
            else
            {
                printArray(ppg_data,900);
            }
            // ESP_LOGI(__func__,"dia chi returnPtr in ben ngoai= %p",returnPtr_ppg);
            xSemaphoreGive(semaphorePPG);
            vTaskDelay(pdMS_TO_TICKS(10));
        }
    }
}
void read_filePCG_from_sdCard(void* parameter)
{
    while(1)
    {
         if(xSemaphoreTake(semaphorePCG,portMAX_DELAY))
        {
            int pcg_sample;
            sdcard_readDataFromFile(&index_pcg, pcg_file, "%d\n", &pcg_sample );
            ESP_LOGI(__func__,"PCG = %d", pcg_sample);
            count++;
            if (count < 1000) {
                pcg_data[count] = pcg_sample;
            }
            else
            {
                printArray(pcg_data,900);
            }
            // ESP_LOGI(__func__,"dia chi returnPtr in ben ngoai= %p",returnPtr_ppg);
            xSemaphoreGive(semaphorePCG);
            vTaskDelay(pdMS_TO_TICKS(10));
        }
    }
}
void processing_data(void* parameter)
{
    while(1)
    {
        if(xSemaphoreTake(semaphorePPG,portMAX_DELAY) == pdTRUE 
            && xSemaphoreTake(semaphorePCG,portMAX_DELAY) == pdTrue)
        {
            
            xSemaphoreGive(semaphorePPG);
            xSemaphoreGive(semaphorePCG);
        }
    }

}
void print_PPG(void* parameter)
{

}
void print_PCG(void* parameter)
{

}


int app_main(void)
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
    semaphorePPG = xSemaphoreCreateBinary();
    semaphorePCG = xSemaphoreCreateBinary();
    xTaskCreate(read_filePPG_from_sdCard,     // Hàm mà task sẽ chạy
                "read_filePPG_from_sdCard",            // Tên của task
                4096*4,              // Kích thước ngăn xếp (tính bằng từ)
                NULL,   // Tham số của task
                1,                 // Ưu tiên của task (1 là ưu tiên thấp nhất)
                NULL); 
    xTaskCreate(read_filePCG_from_sdCard,     // Hàm mà task sẽ chạy
                "read_filePCG_from_sdCard",            // Tên của task
                4096*4,              // Kích thước ngăn xếp (tính bằng từ)
                NULL,   // Tham số của task
                1,                 // Ưu tiên của task (1 là ưu tiên thấp nhất)
                NULL); 
    vTaskDelay(pdMS_TO_TICKS(100));
    return(0);
}