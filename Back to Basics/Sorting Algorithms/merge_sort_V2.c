
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
    

void merge(int arr[], const size_t left, const size_t middle, const size_t right) {

    const size_t num_elements = (right - left) + 1;
    int temp_array[num_elements];
    size_t temp_index = 0;
    size_t left_index = left;
    size_t right_index = middle+1;

    while (left_index <= middle && right_index <= right) {
        if (arr[left_index] <= arr[right_index]) {
            temp_array[temp_index++] = arr[left_index++];
        }
        else {
            temp_array[temp_index++] = arr[right_index++];
        }
    }

    while (left_index <= middle)
    {
        temp_array[temp_index++] = arr[left_index++];
    }

    while (right_index <= right)
    {
        temp_array[temp_index++] = arr[right_index++];
    }

    //copy everything back
    temp_index=0;
    for (size_t i = left; i <= right; i++) {
        arr[i] = temp_array[temp_index++];
    }
}

void merge_sort(int arr[], const size_t left, const size_t right)
{
    if (left >= right)
    {
        return;
    }

    const size_t middle = (left + right) / 2;

    merge_sort(arr, left, middle);
    merge_sort(arr, middle + 1, right);

    merge(arr, left, middle, right);
}

int main(void) {
    const size_t INPUT_SIZE = 10;
    int input[] = {4, 5, 1, 2, 7, 3, 9, 6, 8, 0};

    printf("Input:");
    for (size_t i = 0; i < INPUT_SIZE; i++ ) {
        printf(" %d ", input[i]);
    }

    merge_sort(input, 0, INPUT_SIZE - 1);

    printf("Output:");
    for (size_t i = 0; i < INPUT_SIZE; i++ ) {
        printf(" %d ", input[i]);
    }

    return 0;
}