#ifndef CALCULATOR_H
#define CALCULATOR_H

enum CalculatorStatus {
    CALCULATOR_OK = 0,
    CALCULATOR_DIVISION_BY_ZERO = 1,
    CALCULATOR_INVALID_INPUT = 2
};

/* op must be one of '+', '-', '*', or '/'. */
int calculator_calculate(double a, double b, char op, double *result);

#endif
