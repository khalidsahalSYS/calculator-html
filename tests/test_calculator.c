#include "../src/calculator.h"
#include <stdio.h>

static int check_add(void) {
    double result = 0.0;
    int status = calculator_add(2.0, 3.0, &result);
    return status == CALCULATOR_OK && result == 5.0;
}

static int check_sub(void) {
    double result = 0.0;
    int status = calculator_sub(10.0, 4.0, &result);
    return status == CALCULATOR_OK && result == 6.0;
}

static int check_mul(void) {
    double result = 0.0;
    int status = calculator_mul(7.0, 8.0, &result);
    return status == CALCULATOR_OK && result == 56.0;
}

static int check_div(void) {
    double result = 0.0;
    int status = calculator_div(20.0, 4.0, &result);
    return status == CALCULATOR_OK && result == 5.0;
}

static int check_div_zero(void) {
    double result = 0.0;
    int status = calculator_div(10.0, 0.0, &result);
    return status == CALCULATOR_DIVISION_BY_ZERO;
}

static int check_invalid_operator(void) {
    double result = 0.0;
    int status = calculator_compute(5.0, 2.0, '%', &result);
    return status == CALCULATOR_INVALID_OPERATOR;
}

int main(void) {
    int failed = 0;

    if (!check_add()) {
        fprintf(stderr, "FAIL: add\n");
        failed++;
    }

    if (!check_sub()) {
        fprintf(stderr, "FAIL: sub\n");
        failed++;
    }

    if (!check_mul()) {
        fprintf(stderr, "FAIL: mul\n");
        failed++;
    }

    if (!check_div()) {
        fprintf(stderr, "FAIL: div\n");
        failed++;
    }

    if (!check_div_zero()) {
        fprintf(stderr, "FAIL: div_zero\n");
        failed++;
    }

    if (!check_invalid_operator()) {
        fprintf(stderr, "FAIL: invalid_operator\n");
        failed++;
    }

    if (failed != 0) {
        fprintf(stderr, "Total failed tests: %d\n", failed);
        return 1;
    }

    printf("All calculator tests passed.\n");
    return 0;
}
