#include <random>

// random number between range 
double getRandNum ( double lower , double upper ){
    double f = (double)rand() / RAND_MAX;
    return lower + f * (upper - lower);
}