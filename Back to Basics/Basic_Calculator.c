void calculator() {
    double digit_one = 0;
    double digit_two = 0;
    char restart_prompt;
    double result = 0;
    char operator;
    bool result_valid =  1;

    //add looping for multiple operations.
    printf("~~~Calculator Program~~~\n");
    do{
        printf("Enter your equation\n");
        printf("(number) (operator) (number)\n");

        scanf("%lf %c %lf",&digit_one, &operator, &digit_two);
        //printf("Is this what you entered? \n");
        //printf("%d %c %d \n", digit_one, operator, digit_two);

        switch (operator) {
            case '+':
                result = digit_one + digit_two;
                break;
            case '-':
                result = digit_one - digit_two;
                break;
            case '*':
                result = digit_one * digit_two;
                break;
            case '/':
                if (digit_two == 0) {
                    printf("Cannot divide by zero\n");
                    result_valid = 0;
                }
                else {
                    result = digit_one / digit_two;
                }
                break;
            default:
                printf("Invalid Operator Entered");
                result_valid = 0;
        }

        if (result_valid) {
            printf("%g%c%g = %g \n\n", digit_one, operator, digit_two, result);
        }

        //resetting bool
        result_valid = 1;
        printf("Another? (y/n)\n");
        scanf(" %c", &restart_prompt); //space added to handle /n still in buffer from entering equation

    }while (restart_prompt == 'y');
}


int main(void) {
calculator();
    return 0;
}

