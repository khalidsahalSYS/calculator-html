#include "calculator.h"
#include <stdio.h>
#include <stdlib.h>

static void print_usage(const char *program_name) {
    fprintf(stderr, "Usage: %s <a> <op> <b>\n", program_name);
    fprintf(stderr, "Example: %s 10 + 5\n", program_name);
}

int main(int argc, char *argv[]) {
    double a, b, result;
    char op;
    int status;

    if (argc != 4) {
        print_usage(argv[0]);
        return 1;
    }

    a = strtod(argv[1], NULL);
    op = argv[2][0];
    b = strtod(argv[3], NULL);

    status = calculator_compute(a, b, op, &result);

    if (status != CALCULATOR_OK) {
        if (status == CALCULATOR_DIVISION_BY_ZERO) {
            fprintf(stderr, "Error: Division by zero is not allowed.\n");
        } else if (status == CALCULATOR_INVALID_OPERATOR) {
            fprintf(stderr, "Error: Invalid operator. Use: +, -, *, /.\n");
        } else {
            fprintf(stderr, "Error: Invalid input.\n");
        }
        return 2;
    }

    printf("%.15g\n", result);
    return 0;
}
