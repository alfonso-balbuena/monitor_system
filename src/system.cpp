#include <unistd.h>
#include <cstddef>
#include <set>
#include <string>
#include <vector>

#include "process.h"
#include "processor.h"
#include "system.h"
#include "linux_parser.h"

using namespace std;

System::System() {
    kernel_ = LinuxParser::Kernel();
    operating_system_ = LinuxParser::OperatingSystem();
    vector<int> number_pids = LinuxParser::Pids();
    for(auto &p : number_pids) {
        processes_.emplace_back(p);        
    }
}


Processor& System::Cpu() { return cpu_; }

vector<Process>& System::Processes() { 
    std::sort(processes_.begin(),processes_.end());
    return processes_; 
}

string System::Kernel() { return kernel_; }

float System::MemoryUtilization() { return LinuxParser::MemoryUtilization(); }

string System::OperatingSystem() { return operating_system_; }

int System::RunningProcesses() { return LinuxParser::RunningProcesses(); }

int System::TotalProcesses() { return LinuxParser::TotalProcesses(); }

long int System::UpTime() { return LinuxParser::UpTime(); }