#include <stdio.h>

/// @brief To make the signal to smoother
/// @param arr is the signal
/// @param arr_len is the length of signal
/// @param k is the window sliding on the signal
/// @param result is the output signal 
void moving_mean(const int *arr, int arr_len, int k, int *result);

/// @brief To caculate average line of signal 
/// @param arr is the signal
/// @param arr_len is the length of signal
/// @param k is the window sliding on the signal
/// @param result is output signal
void moving_median(const float *arr, int arr_len, int k, float *result);

/// @brief To find the peak on the signal
/// @param arr is the signal
/// @param size is the length of signal
/// @param index is the containing array the index of peaks
/// @param number_peak is number peak of signal
/// @param prominence is the prominence when compare the peak value with the values on the both sides
void findpeaks(int *arr,int size, int *index, int *number_peak, int prominence );

/// @brief To find 2 value biggest on the pcg signal corresponding to each ppg peak
/// @param arr is pcg signal
/// @param size_arr is length of pcg signal
/// @param peak_pcg is peak index pcg signal
/// @param number_peak_pcg is number peak pcg
/// @param number_peak_ppg is the number peak ppg
/// @param peak_ppg is peak index ppg signal
/// @param s1 is peak s1
/// @param s2 is peak s2
/// @param window is window with ppg peak is center
void findTwoLargest(int* arr,int size_arr,int* peak_pcg, int number_peak_pcg, int number_peak_ppg, int* peak_ppg, int* s1, int* s2, int window);