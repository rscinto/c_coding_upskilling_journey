void calculator_two()
{
 printf("~~~Calculator Program~~~\n");
    while (1){
        const unsigned int input_len = 30;
        char input[input_len];
        char* digit_one_char;
        char* digit_two_char;
        char* operator_char;
        int newLineLocation = 0;
        char operator;
        bool user_exit = false;

        printf("Enter your equation\n");
        printf("(operator) (number) (number)\n");
        //get user input
        fgets(input, input_len, stdin); //Grabs the input line from the user.
        newLineLocation = strcspn(input, "\n");
        input[newLineLocation] = '\0';  //clobber the new line with the null terminator

        //must be called before strtok or else the input is modded
        //must be called before clobbering the newline, or else you get 'qui' and will never quit.
        //check to see if they would like to quit
        if (strcmp(input, "quit") == 0) {
            printf("Goodbye user\n");
            user_exit = true;
            //break; //The user is finished so we break out.
        }else if (strcmp(input, "help") == 0) {
            printf("~~~~~~\n");
            printf("Examples\n");
            printf("add 5 5\n");
            printf("10\n");
            printf("mul 3 15\n");
            printf("45\n");
            printf("div 3 0\n");
            printf("Cannot divide by zero\n");
            printf("~~~~~~\n");
        }
        else {
            operator_char = strtok(input, " ");
            digit_one_char = strtok(NULL, " ");
            digit_two_char = strtok(NULL, " ");

            if (operator_char == NULL || digit_one_char == NULL || digit_two_char == NULL) {
                printf("Invalid input. Use: Add 5 5\n");
                printf("The user must enter an operator followed by two digits, separated by spaces\n");
            }
            else {
                double digit_one = strtod(digit_one_char, NULL);
                double digit_two = strtod(digit_two_char, NULL);
                double result = 0;

                if (strcmp(operator_char, "add") == 0) {
                    result = digit_one + digit_two;
                    printf("%g\n", result);
                }
                else if (strcmp(operator_char, "sub") == 0) {
                    result = digit_one - digit_two;
                    printf("%g\n", result);
                }
                else if (strcmp(operator_char, "mul") == 0) {
                    result = digit_one * digit_two;
                    printf("%g\n", result);
                }
                else if (strcmp(operator_char, "div") == 0) {
                    //check for div by zero
                    if (digit_two == 0) {
                        printf("Cannot divide by zero\n");
                    }

                    else {
                        result = digit_one / digit_two;
                        printf("%g\n", result);
                    }
                }
                else {
                    //invalid operator used.
                    printf("Invalid Operation\n");
                }
            }
        }
        if (user_exit) {
            break;
        }
    }
}


int main(void) {

calculator_two();

    return 0;
}
