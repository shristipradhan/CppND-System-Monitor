#include <dirent.h>
#include <unistd.h>
#include <string>
#include <vector>
#include <iostream>

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
  string os, kernel;
  string line;
  string version;
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

// Read and return the system memory utilization
float LinuxParser::MemoryUtilization() {
  string line;
  string key;
  string value;
  string memTotal;
  string memFree;

  std::ifstream filestream(kProcDirectory+kMeminfoFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
     std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "MemTotal:") {
          memTotal = value;
        }
        if (key == "MemFree:") {
          memFree = value;
        }
      }
    }
  }

  float memUtil;
  memUtil = (std::stof(memTotal) - std::stof(memFree))/std::stof(memTotal);
  return memUtil;
}

// Read and return the system uptime
long LinuxParser::UpTime() {
    string line;
    string uptime;
    string idletime;
    std::ifstream filestream(kProcDirectory + kUptimeFilename);
    if (filestream.is_open())
    {
        std::getline(filestream, line);
        std::istringstream linestream(line);
        linestream >> uptime >>  idletime;
    }
  return std::stol(uptime);
}

// Read and return the number of jiffies for the system
long LinuxParser::Jiffies() {
  string line;
  string user, nice, system, idle, iowait, irq, softirq, steal, guest, guest_nice;
  std::ifstream filestream(kProcDirectory+kStatFilename);
  if (filestream.is_open())
  {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    linestream >> user >> nice >> system >> idle >> iowait >> irq >> softirq >> steal
               >> guest >> guest_nice;
  }

  long activeJiffies;
  long idleJiffies;

  activeJiffies = stol(user) + stol(nice) + stol(system) + stol(irq) + stol(softirq) + stol(steal);
  idleJiffies = stol(idle) + stol(iowait);
  return (activeJiffies - idleJiffies);
}

// Read and return the number of active jiffies for a PID
long LinuxParser::ActiveJiffies(int pid) {
  string uptime;
  string line;
  std::ifstream filestream1(kProcDirectory+kUptimeFilename);
  if (filestream1.is_open())
  {
    std::getline(filestream1, line);
    std::istringstream linestream(line);
    linestream >> uptime;
  }

  string token;
  vector<string> tokenVector;

  std::ifstream filestream2(kProcDirectory+to_string(pid)+kStatFilename);
  if (filestream2.is_open()) {
    while (std::getline(filestream2, token, ' ')) {
        tokenVector.push_back(token);
        if (tokenVector.size() == 22)
        {
          break;
        }
    }
  }

  string utime = tokenVector[13];
  string stime = tokenVector[14];
  string cutime = tokenVector[15];
  string cstime = tokenVector[16];
  string starttime = tokenVector[21];

  int totaltime = stoi(utime) + stoi(stime) + stoi(cutime) + stoi(cstime);

  int seconds = stoi(uptime) - (stoi(starttime)/sysconf(_SC_CLK_TCK));

  int cpuusage = 100* ((totaltime/sysconf(_SC_CLK_TCK))/seconds);

  return cpuusage;
}

// Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() {
  string line;
  string cpu;
  string user, nice, system, idle, iowait, irq, softirq, steal, guest, guest_nice;
  std::ifstream filestream(kProcDirectory+kStatFilename);
  if (filestream.is_open())
  {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    linestream >> cpu >> user >> nice >> system >> idle >> iowait >> irq >> softirq >> steal
               >> guest >> guest_nice;
  }

  long activeJiffies;

  activeJiffies = stol(user) + stol(nice) + stol(system) + stol(irq) + stol(softirq) + stol(steal);
  return activeJiffies;
}

// Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() {
  string line;
  string cpu;
  string user, nice, system, idle, iowait, irq, softirq, steal, guest, guest_nice;
  std::ifstream filestream(kProcDirectory+kStatFilename);
  if (filestream.is_open())
  {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    linestream >> cpu >> user >> nice >> system >> idle >> iowait >> irq >> softirq >> steal
               >> guest >> guest_nice;
  }

  long idleJiffies;

  idleJiffies = stol(idle) + stol(iowait);
  return idleJiffies;
}

// Read and return CPU utilization
vector<long> LinuxParser::CpuUtilization() {
  vector<long> cpuUtilizationVector;

  cpuUtilizationVector.push_back(IdleJiffies());
  cpuUtilizationVector.push_back(ActiveJiffies());
  return cpuUtilizationVector;
}

// Read and return the total number of processes
int LinuxParser::TotalProcesses() {
  return LinuxParser::Pids().size();
}

// Read and return the number of running processes
int LinuxParser::RunningProcesses() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
     std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "procs_running") {
          return std::stoi(value);
        }
      }
    }
  }
  return std::stoi(value);
}

// Read and return the command associated with a process
string LinuxParser::Command(int pid) {
  string command;
  std::ifstream stream(kProcDirectory+to_string(pid)+kCmdlineFilename);
  if (stream.is_open())
  {
    std::getline(stream,command);
  }
  return command;
}

// Read and return the memory used by a process
string LinuxParser::Ram(int pid) {
  string line;
  string key;
  string value;
  string memTotal;

  std::ifstream filestream(kProcDirectory+to_string(pid)+kStatusFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
     std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "VmSize:") {
          memTotal = value;
          break;
        }
      }
    }
  }

  int mem_dec = 0;
  if (!memTotal.empty())
  {
    mem_dec = (float)stoi(memTotal) * 0.001;
  }

  return to_string(mem_dec);
}

// Read and return the user ID associated with a process
string LinuxParser::Uid(int pid) {
  string line;
  string key;
  string value;
  std::ifstream filestream(kProcDirectory+to_string(pid)+kStatusFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "Uid:") {
          return value;
        }
      }
    }
  }

  return value;
}

// Read and return the user associated with a process
string LinuxParser::User(int pid) {
  string token;
  vector<string> tokenVector;

  std::ifstream filestream(kPasswordPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, token, ':')) {
        tokenVector.push_back(token);
        if (tokenVector.size() == 3)
        {
          tokenVector[2] == LinuxParser::Uid(pid);
          return tokenVector[0];
        }
    }
  }

  return token;
}

// Read and return the uptime of a process
long LinuxParser::UpTime(int pid) {
  vector<string> tokenVector;
  string token;
  string uptime;
  std::ifstream filestream(kProcDirectory+to_string(pid)+kStatFilename);
  if (filestream.is_open())
  {
    while(std::getline(filestream, token, ' '))
    {
      tokenVector.push_back(token);
      if (tokenVector.size() == 22)
      {
        uptime = tokenVector[21];
        break;
      }
    }
  }

  return stol(uptime)/sysconf(_SC_CLK_TCK);
}