#include "calculator.h"
#include <float.h>

static int is_finite_number(double value) {
    return value == value && value <= DBL_MAX && value >= -DBL_MAX;
}

int calculator_add(double a, double b, double *result) {
    if (result == NULL || !is_finite_number(a) || !is_finite_number(b)) {
        return CALCULATOR_INVALID_INPUT;
    }
    *result = a + b;
    return CALCULATOR_OK;
}

int calculator_sub(double a, double b, double *result) {
    if (result == NULL || !is_finite_number(a) || !is_finite_number(b)) {
        return CALCULATOR_INVALID_INPUT;
    }
    *result = a - b;
    return CALCULATOR_OK;
}

int calculator_mul(double a, double b, double *result) {
    if (result == NULL || !is_finite_number(a) || !is_finite_number(b)) {
        return CALCULATOR_INVALID_INPUT;
    }
    *result = a * b;
    return CALCULATOR_OK;
}

int calculator_div(double a, double b, double *result) {
    if (result == NULL || !is_finite_number(a) || !is_finite_number(b)) {
        return CALCULATOR_INVALID_INPUT;
    }

    if (b == 0.0) {
        return CALCULATOR_DIVISION_BY_ZERO;
    }

    *result = a / b;
    if (!is_finite_number(*result)) {
        return CALCULATOR_INVALID_INPUT;
    }

    return CALCULATOR_OK;
}

int calculator_compute(double a, double b, char op, double *result) {
    if (result == NULL || !is_finite_number(a) || !is_finite_number(b)) {
        return CALCULATOR_INVALID_INPUT;
    }

    switch (op) {
        case '+':
            return calculator_add(a, b, result);
        case '-':
            return calculator_sub(a, b, result);
        case '*':
            return calculator_mul(a, b, result);
        case '/':
            return calculator_div(a, b, result);
        default:
            return CALCULATOR_INVALID_OPERATOR;
    }
}
