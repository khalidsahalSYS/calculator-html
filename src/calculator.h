#ifndef CALCULATOR_H
#define CALCULATOR_H

enum CalculatorStatus {
    CALCULATOR_OK = 0,
    CALCULATOR_DIVISION_BY_ZERO = 1,
    CALCULATOR_INVALID_INPUT = 2,
    CALCULATOR_INVALID_OPERATOR = 3
};

int calculator_add(double a, double b, double *result);
int calculator_sub(double a, double b, double *result);
int calculator_mul(double a, double b, double *result);
int calculator_div(double a, double b, double *result);
int calculator_compute(double a, double b, char op, double *result);

#endif
