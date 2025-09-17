#include <random>

// random number between range 
double getRandNum ( double lower , double upper ){
   std::uniform_real_distribution<double> unif(lower,upper);
   std::default_random_engine re;
   return unif(re);
}