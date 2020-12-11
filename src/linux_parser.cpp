#include <dirent.h>
#include <unistd.h>
#include <string>
#include <vector>
#include <math.h>
#include <iomanip>


#include <iostream>

#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;

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
        if (key == kPrettyName) {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

string LinuxParser::Kernel() {
  string os;
  string kernel;
  string version;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    if (file->d_type == DT_DIR) {      
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
  string line;
  string memTitle;
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

long LinuxParser::Jiffies() { 
  return sysconf(_SC_CLK_TCK);
}

long LinuxParser::ActiveJiffies(int pid) { 
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatFilename);
  string line;
  string data;
  long time;
  long total_time{0};
  if(stream.is_open()) {
    std::getline(stream,line);
    std::istringstream lineStream(line);
    for(int i = 0; i < 13; i++)
      lineStream >> data;
    for(int i = 0; i < 4; i++){
      lineStream >> time;
      total_time += time;
    }
  }
  return total_time; 
}

long LinuxParser::ActiveJiffies() { 
  auto time = LinuxParser::CpuUtilization();
  return time[ActiveTime::USER] + time[ActiveTime::NICE] + time[ActiveTime::SYSTEM] + time[ActiveTime::IRQ] + time[ActiveTime::SOFTIRQ] + time[ActiveTime::STEAL]; 
}


long LinuxParser::IdleJiffies() { 
  auto time = LinuxParser::CpuUtilization();
  return time[IdleTime::IDLE] + time[IdleTime::IOWAT];
}


vector<long> LinuxParser::CpuUtilization() { 
  vector<long> cpu;
  std::ifstream stream(kProcDirectory + kStatFilename);  
  string line;
  string key;
  long val;
  if(stream.is_open()) {
    while(std::getline(stream,line)){
      std::istringstream linestream(line);
      linestream >> key;
      if(key == kCpu) {
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
  return LinuxParser::searchProcStat(kProcesses);  
}

int LinuxParser::RunningProcesses() { 
  return LinuxParser::searchProcStat(kProcessRunning);    
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
  string data;
  std::ostringstream streamoutput;
  
  if(stream.is_open()) {
    while(std::getline(stream,line)) {
      std::istringstream linestream(line);
      linestream >> data;
      if(data == kRamProcess) {
        linestream >> data;        
        return (data.length() < 4) ? "0" : data.substr(0, data.length() - 3);
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
      if(key == kUid) {
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

long LinuxParser::UpTime(int pid) { 
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatFilename);
  string line;
  string data;
  long time{0};
  if(stream.is_open()) {
    std::getline(stream,line);
    std::istringstream linestream(line);
    for(int i = 0; i < 21; i++)
      linestream >> data;    
    linestream >> time;    
    return time / LinuxParser::Jiffies();
  }
  return time;
}