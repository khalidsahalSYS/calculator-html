#include "calculator.h"
#include <float.h>

static int is_finite_number(double value) {
    return value == value && value <= DBL_MAX && value >= -DBL_MAX;
}

int calculator_calculate(double a, double b, char op, double *result) {
    if (!result || !is_finite_number(a) || !is_finite_number(b)) {
        return CALCULATOR_INVALID_INPUT;
    }

    switch (op) {
        case '+': *result = a + b; break;
        case '-': *result = a - b; break;
        case '*': *result = a * b; break;
        case '/':
            if (b == 0.0) return CALCULATOR_DIVISION_BY_ZERO;
            *result = a / b;
            break;
        default: return CALCULATOR_INVALID_INPUT;
    }

    return is_finite_number(*result) ? CALCULATOR_OK : CALCULATOR_INVALID_INPUT;
}
