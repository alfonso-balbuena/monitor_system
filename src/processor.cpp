#include "processor.h"
#include "linux_parser.h"
#include <iostream>

Processor::Processor(){    
    sum_idle_prev_ = LinuxParser::IdleJiffies();
    sum_prev_ = sum_idle_prev_ + LinuxParser::ActiveJiffies();
}

float Processor::Utilization() {     
    long sum_idle = LinuxParser::IdleJiffies();
    long sum_noidle = LinuxParser::ActiveJiffies();
    long sum = sum_idle + sum_noidle;    
    long total = sum - sum_prev_;
    long idled = sum_idle - sum_idle_prev_;
    sum_idle_prev_ = sum_idle;
    sum_prev_ = sum;
    if(total == 0)
        return 0;
    return  (float)(total - idled) / (float)total ;
    
}