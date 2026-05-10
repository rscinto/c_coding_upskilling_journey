#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main(void) {

    const size_t INPUT_SIZE = 10;
    unsigned short int input[] = {4, 5, 1, 2, 7, 3, 9, 6, 8, 0};
    size_t size_sorted_region = 1;
    //  3 2 1
    //  3| 2 1
    // _ 3 | 2
    // 2 3 | 1
    // _ 2 3 | 1
    // 1 2 3 | fin

    printf("Input:");
    for (size_t i = 0; i < INPUT_SIZE; i++ ) {
        printf(" %hu ", input[i]);
    }

    for (size_t  i = 0; i<INPUT_SIZE-1; i++) { //Outer loop to incrememnt through the array
        // select number on edge of sorted region
        unsigned short int current_number = input[i + 1];
        size_t current_number_index = i + 1;
        size_t insertion_index = current_number_index;

        for (size_t j = 0; j <= size_sorted_region; j++) { //Inner loop to find where to place the new number
            if (current_number <= input[j]) {
                insertion_index = j;
                break; //We found the insertion location, this loop has completed its purpose
            }
        }

        //Shifting
        for (size_t k = current_number_index; k > insertion_index; k--) {
            input[k] = input[k-1];
        }
        //Insertion
        input[insertion_index] = current_number;
        size_sorted_region++;
    }

    printf("Sorted:");
    for (size_t i = 0; i < INPUT_SIZE; i++ ) {
        printf(" %hu ", input[i]);
    }

    return 0;
}