 #include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "process.h"
#include "linux_parser.h"

using std::string;
using std::to_string;
using std::vector;

Process::Process(int pid)  : pid_(pid) {
    user_ = LinuxParser::User(pid);
    string command = LinuxParser::Command(pid);    
    command_ = (command.length() < 40) ? command : command.substr(0,40) + "...";    
} 


int Process::Pid() { return pid_; }

// Alfonso Balbuena Sanchez
float Process::CpuUtilization() const { 
    long total_time = LinuxParser::ActiveJiffies(pid_);
    long seconds = UpTime();
    return ((float)(total_time / LinuxParser::Jiffies()) / seconds);
}


string Process::Command() { return command_; }


string Process::Ram() { return LinuxParser::Ram(pid_); }


string Process::User() { return user_; }


long int Process::UpTime() const { return LinuxParser::UpTime() - LinuxParser::UpTime(pid_); }


bool Process::operator<(Process const& a) const { return  a.CpuUtilization() < CpuUtilization(); }