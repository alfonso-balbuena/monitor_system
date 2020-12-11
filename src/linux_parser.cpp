#include <dirent.h>
#include <unistd.h>
#include <string>
#include <vector>
#include <math.h>
#include <iomanip>


#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;

// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string os, kernel,version;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}


float LinuxParser::MemoryUtilization() { 
  std::ifstream stream(kProcDirectory + kMeminfoFilename);    
  if(stream.is_open()) {    
    float memTotal = getNumMem(stream);    
    float memFree = getNumMem(stream);
    return (memTotal - memFree) / memTotal;
  }
  return -1;
}

float LinuxParser::getNumMem(std::ifstream &stream) {
  string line, memTitle;
  float mem;
  std::getline(stream,line);
  std::istringstream linestream(line);
  linestream >> memTitle >> mem;
  return mem;
}


long LinuxParser::UpTime() { 
  std::ifstream stream(kProcDirectory + kUptimeFilename);
  string line;
  long totalUptime{0};
  long idleTime{0};
  if(stream.is_open()) {
    std::getline(stream,line);
    std::istringstream lineStream(line);
    lineStream >> totalUptime >> idleTime;
  }
  return totalUptime + idleTime;
}

// TODO: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() { 
  return sysconf(_SC_CLK_TCK);
}

// TODO: Read and return the number of active jiffies for a PID
long LinuxParser::ActiveJiffies(int pid) { 
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatFilename);
  string line;
  string data;
  long time;
  if(stream.is_open()) {
    
  }
  return 0; 
}

// TODO: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() { return 0; }

// TODO: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() { return 0; }


vector<long> LinuxParser::CpuUtilization() { 
  vector<long> cpu;
  std::ifstream stream(kProcDirectory + kStatFilename);
  string current_cpu = "cpu";
  string line,key;
  long val;
  if(stream.is_open()) {
    while(std::getline(stream,line)){
      std::istringstream linestream(line);
      linestream >> key;
      if(key == current_cpu) {
        while(linestream >> val) {
          cpu.push_back(val);          
        }
      }
    }
  }
  return cpu;
}

int LinuxParser::searchProcStat(std::string attribute) {
  std::ifstream stream(kProcDirectory + kStatFilename);
  string line, title;
  int num_processes{0};
  if(stream.is_open()) {
    while(std::getline(stream,line)) {
      std::istringstream lineStream(line);
      lineStream >> title >> num_processes;
      if(title == attribute) {
        return num_processes;
      }
    }
  }
  return 0; 
}

int LinuxParser::TotalProcesses() { 
  return LinuxParser::searchProcStat("processes");  
}

int LinuxParser::RunningProcesses() { 
  return LinuxParser::searchProcStat("procs_running");    
}


string LinuxParser::Command(int pid) { 
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kCmdlineFilename);
  string line;
  if(stream.is_open()) {
    std::getline(stream,line);
  }
  return line; 
}


string LinuxParser::Ram(int pid) { 
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatusFilename);
  string line;
  string title;
  std::ostringstream streamoutput;
  int memory;
  if(stream.is_open()) {
    while(std::getline(stream,line)) {
      std::istringstream linestream(line);
      linestream >> title;
      if(title == "VmSize:") {
        linestream >> memory;
        float memoryMB = (float)memory / 1024;        
        streamoutput << std::fixed << std::setprecision(2) << memoryMB;
        return streamoutput.str();
      }
    }
  }
  return string(); 
}


string LinuxParser::Uid(int pid) {
  string path(kProcDirectory);   
  string line;
  string key, value;
  std::ifstream stream(path.append(std::to_string(pid)) + kStatusFilename);
  if(stream.is_open()) {
    while(std::getline(stream,line)) {
      std::istringstream lineStrean(line);
      lineStrean >> key >> value;
      if(key == "Uid:") {
        return value;
      }
    }
  }
  return string(); 
}


string LinuxParser::User(int pid) { 
  string user, line, userPass;
  string uid = LinuxParser::Uid(pid);
  string userPid;

  std::ifstream stream(kPasswordPath);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::replace(line.begin(),line.end(),' ','_');
      std::replace(line.begin(),line.end(),':',' ');
      std::istringstream lineStream(line);
      lineStream >> user >> userPass >> userPid;
      if(userPid == uid)
      return user;
    }
  }
  return string(); 
}



// TODO: Read and return the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::UpTime(int pid[[maybe_unused]]) { 
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatFilename);
  string line;
  long time{0};
  if(stream.is_open()) {
    for(int i = 0; i < 22; i++)
      std::getline(stream,line);
    std::istringstream linestream(line);
    return time / LinuxParser::Jiffies();
  }
  return time;
}