#include <stdio.h>


int main(void) {

    //Selection sorting
    const unsigned short int input_size = 10;
    unsigned short int input[] = {4,5,1,2,7,3,9,6,8,0};
    unsigned short int front_index = 0;


    printf("Input:");
    for (int i = 0; i < input_size - 1; i++ ) {
        printf(" %d ", input[i]);
    }

    unsigned short int smallest_index = front_index;
    unsigned short int smallest_number = input[front_index];  //priming for the comparison

    for (int j = front_index; j<input_size; j++){

        for (int i = front_index + 1; i < input_size; i++) {
            if (input[i] <= smallest_number) {
                smallest_number = input[i];
                smallest_index  = i;
            }
        }

        unsigned short int temp = input[front_index];
        input[front_index] = input[smallest_index];
        input[smallest_index] =  temp;
        front_index++; //keep moving the front
        smallest_number = input[front_index]; //new primer
        smallest_index = front_index;
    }
    //Iterate over the array and find the smallest.
    //swap that smallest item with whatever is in the front.
    //Find the next smallest in the remainder of the array
    //swap that smallest with the number in the front + 1, and on
    //once there is no more swapping we're done and can exit.

    printf("Sorted:");
    for (int i = 0; i < input_size; i++ ) {
        printf(" %d ", input[i]);
    }

    return 0;
}