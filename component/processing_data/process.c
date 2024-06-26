#include <process.h>

void moving_mean(const int *arr, int arr_len, int k, int *result) {
    for (int i = 0; i <= arr_len - 1; ++i) 
    {
        // int window = k;
        if(i < arr_len -k + 1)
        {
            float sum = 0;
            for (int j = i; j < i + k; ++j) {
                sum += arr[j];
            }
            //printf("gia tri K = %d\n", k);
            result[i] = (int)(sum / k);
            //printf("mean data trong ham moving_mean[%d] = %.2f\n",i, result[i]);
        }
        else 
        {
            float sum = 0;
            for(int j = i; j <= arr_len -1; ++j )
            {
                sum += arr[j];
                //printf("arr[%d] = %d\n", j, arr[j]);
            }
            result[i] = (float)(sum)/(arr_len-i); 
            // printf("sum data trong ham k cua so ham sum[%d] = %.2f\n",i, sum);
            // printf("mean data trong ham k cua so ham mean[%d] = %.2f\n",i, result[i]);
            // printf("arr_len = [%d] va i =[%d] \n",arr_len, i);
        }
    }
}

void moving_median(const float *arr, int arr_len, int k, float *result) {
    for (int i = 0; i <= arr_len - k; ++i) {
        float temp[k];
        for (int j = 0; j < k; ++j) {
            temp[j] = arr[i + j];
        }
        // Sắp xếp mảng tạm thời
        for (int m = 0; m < k - 1; ++m) {
            for (int n = m + 1; n < k; ++n) {
                if (temp[m] > temp[n]) {
                    float temp_val = temp[m];
                    temp[m] = temp[n];
                    temp[n] = temp_val;
                }
            }
        }
        // Tìm giá trị trung vị
        if (k % 2 == 0) {
            result[i] = (temp[k / 2 - 1] + temp[k / 2]) / 2.0;
        } else {
            result[i] = temp[k / 2];
        }
    }
}

void findpeaks(int *arr,int size, int *index, int *number_peak, int prominence )
{
    *number_peak = 0;
    int j = 0;
    for(int i = 1; i < size ; ++i )
    {
        //ESP_LOGI(__func__,"arr[%d] = %d", i, arr[i]);
        if( i > 30-1 && i < size-30-1)
        {
            if(arr[i] > arr[i-1] && arr[i] > arr[i+1])
            {
                if( ((arr[i] - arr[i-30]) > prominence) && ( (arr[i]-arr[i+30]) > prominence) )
                {
                    index[j] = i;
                    *number_peak = *number_peak + 1;
                    // ESP_LOGI(__func__,"Dinh ơ vị tri = %d\n", index[j]);
                    // ESP_LOGI(__func__,"arr[%d] = %d\n", i, arr[i]);
                    // ESP_LOGI(__func__,"arr[%d] = %d\n", i-1, arr[i-1]);
                    // ESP_LOGI(__func__,"arr[%d] = %d\n", i+1, arr[i+1]);
                    j++;
                }
            }
        }
        else if(i < 30)
        {
            if(arr[i] > arr[i-1] && arr[i] > arr[i+1])
            {
                if( (arr[i] - arr[0]) > prominence && ( (arr[i]-arr[i+30]> prominence) ))
                {
                    index[j] = i;
                    *number_peak = *number_peak + 1;
                    // ESP_LOGI(__func__,"Dinh ơ vị tri = %d\n", i);
                }
            }
        }
        else if(i > size-30)
        {
            if(arr[i] > arr[i-1] && arr[i] > arr[i+1])
            {
                ESP_LOGI(__func__,"có chạy vao doạn cuối\n");
                if( (arr[i] - arr[i - 30]) > prominence && ( (arr[i]-arr[size- 1]> prominence ) ))
                {
                    index[j] = i;
                    *number_peak = *number_peak + 1;
                    // ESP_LOGI(__func__,"Dinh ơ vị tri = %d\n", i);
                }
            }
        }
    }
}
void findTwoLargest(int* arr,int size_arr,int* peak_pcg, int number_peak_pcg, int number_peak_ppg, int* peak_ppg, int* s1, int* s2, int window)
{
    int max1, max2;
    ESP_LOGI(__func__,"so phan tu cua mang la : %d\n", size_arr);
    ESP_LOGI(__func__,"Number peak ppg : %d", number_peak_ppg);
    ESP_LOGI(__func__,"Number peak pcg : %d", number_peak_pcg);
    for(int i = 0; i < number_peak_ppg; ++i)
    {
        ESP_LOGI(__func__,"Tai dinh ppg thu %d\n", i+1);
        max1 = 0;
        max2 = 0;
        for(int j = 0; j < number_peak_pcg; ++j)
        {
            if( (peak_pcg[j] > peak_ppg[i]- window) && (peak_pcg[j] < peak_ppg[i] + window))
            {
                ESP_LOGI(__func__," kiem tra cua so tai dinh ppg[%d] cua so chay den co pcg[%d] = %d\n", peak_ppg[i], peak_pcg[j], arr[peak_pcg[j]]);
                if(arr[peak_pcg[j]] > max1)
                {
                    max2 = max1;
                    max1 = arr[peak_pcg[j]];
                    s2[i] = s1[i];
                    s1[i] = peak_pcg[j];
                    //printf("co chayj vaof ddaay khoong\n");
                }
                else if(arr[peak_pcg[j]] > max2)
                {
                    s2[i] = peak_pcg[j];
                    max2= arr[peak_pcg[j]];
                }   
            }
        }
    } 
    // In ra vị trí của hai giá trị lớn nhất và lớn thứ hai
    for(int i = 0; i < number_peak_ppg; ++i )
    {
        ESP_LOGI(__func__,"Vi tri cua hai gia tri lon nhat gan PPG thu %d lan luot la: %d va %d\n", i, s1[i], s2[i]);
    }
}

void blood_pressure(float *systolic, float *diastolic, int *inedx_peak_ppg, int number_peak_ppg, int *index_s1, int *index_s2)
{
    int a1 = 1;
    int a2 = 2;
    int b1 = 1;
    int b2 = 2;
    int b3 = 3;
    int VTT ;
    int ET;
    float systolic_;
    float diastolic_;
    for(int i = 0; i < number_peak_ppg; i++)
    {
        if( abs(inedx_peak_ppg- index_s1) - abs(inedx_peak_ppg- index_s2 > 0) )
        {
            VTT = abs(inedx_peak_ppg- index_s1);
            ET = abs(index_s1 - index_s2);
            systolic_ = a1 * VTT + a2;
            diastolic_ = systolic_ - ((b1 * ET) / ( VTT * VTT)) - (b2 / (VTT * VTT)) - b3;
        }
        else 
        {
            VTT = abs(inedx_peak_ppg- index_s2);
            ET = abs(index_s1 - index_s2);
            systolic_ = a1 * VTT + a2;
            diastolic_ = systolic_ - ((b1 * ET) / ( VTT * VTT)) - (b2 / (VTT * VTT)) - b3;
        }
        *systolic = *systolic + systolic_;
        *diastolic = *diastolic + diastolic_;
    }
    *systolic = *systolic / number_peak_ppg;
    *diastolic = *diastolic / number_peak_ppg;
}
int heart_rate(int *index_peak_ppg, int number_index, int fs)
{
    float heart_rate = 0;
    for(int i = 0; i < number_index - 1; i++)
    {
        heart_rate = heart_rate + index_peak_ppg[i+1] - index_peak_ppg[i];
    }
    heart_rate = (float)(heart_rate/number_index);
    return heart_rate;
}