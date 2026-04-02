#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <mpi.h>

// MPI Tags for communication 
#define TAG_WORK_REQ    1  // Worker requesting work
#define TAG_WORK_TASK   2  // Master sending an interval
#define TAG_RESULT      3  // Worker sending partial integral
#define TAG_NEW_TASK    4  // Worker sending a split sub-interval back
#define TAG_TERMINATE   0  // Master telling worker to stop 

typedef struct {
    double L, R;
} Task;

// Function definitions as per assignment requirements 
double f(int id, double x) {
    switch(id) {
        case 0: return sin(x) + 0.5 * cos(3 * x);
        case 1: return 1.0 / (1.0 + 100.0 * pow(x - 0.3, 2));
        case 2: return sin(200 * x) * exp(-x);
        default: return 0;
    }
}

// Standard Simpson's Rule over [a, b]
double simpson(int id, double a, double b) {
    double m = (a + b) / 2.0;
    return ((b - a) / 6.0) * (f(id, a) + 4 * f(id, m) + f(id, b));
}

// Core Adaptive Logic used by all modes
double adaptive_recursive(int id, double a, double b, double tol, int *count) {
    double m = (a + b) / 2.0;
    double left_s = simpson(id, a, m);
    double right_s = simpson(id, m, b);
    double whole_s = simpson(id, a, b);

    // If the difference is within tolerance, accept 
    if (fabs(left_s + right_s - whole_s) <= 15 * tol) {
        (*count)++;
        return left_s + right_s + (left_s + right_s - whole_s) / 15.0;
    }
    // Otherwise, split and continue 
    return adaptive_recursive(id, a, m, tol/2.0, count) + 
           adaptive_recursive(id, m, b, tol/2.0, count);
}