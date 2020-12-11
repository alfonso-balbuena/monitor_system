#ifndef PROCESSOR_H
#define PROCESSOR_H

#include <vector>

class Processor {
 public:
    Processor();
    float Utilization();  

 private:    
    long sum_prev_;
    long sum_idle_prev_;
};

#endif