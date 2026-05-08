// A basic bubble sort
// O(n²)

#include <stdbool.h>

int main(void) {

    //Bubble sorting
    const unsigned short int input_size = 10;
    unsigned int input[] = {4,5,1,2,7,3,9,6,8,0};
    bool sorted = false;

    printf("Input:");
    for (int i = 0; i < input_size; i++ ) {
        printf(" %d ", input[i]);
    }

    do {
        bool swapped = false;

        //iterate over the array
        for (int i = 0; i < input_size - 1; i++) {
            //check the neighbors
            //if the number on the right is smaller than the number on the left we swap
            if (input[i] > input[i+1]) {
                int temp = input[i];
                input[i] = input[i+1];
                input[i+1] = temp;
                swapped = true;
                //if there was a swap, then the array was not sorted so we loop again
            }
        }

        //Condition to end the looping since there was no swap, we are done.
        if (!swapped) {
            sorted = true;
        }

    }while (!sorted);

    printf("Sorted:");
    for (int i = 0; i < input_size; i++ ) {
        printf(" %d ", input[i]);
    }

    return 0;
}