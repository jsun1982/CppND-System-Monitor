#include "linux_parser.h"
#include <dirent.h>
#include <unistd.h>
#include <string>
#include <vector>
#include "format.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;
using namespace Format;

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
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> kernel;
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

// TODO: Read and return the system memory utilization
float LinuxParser::MemoryUtilization() {
  string line;
  string key;
  string value;
  string unit;
  float memtotal{0}, memfree{0};
  std::ifstream filestream(kProcDirectory + kMeminfoFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value >> unit) {
        if (key == "MemTotal") {
          memtotal = stof(value);
        } else if (key == "MemFree") {
          memfree = stof(value);
          float percentage = 1.0 - memfree / memtotal;
          return percentage;
        }
      }
    }
  }
  return 0.0;
}

// TODO: Read and return the system uptime
long LinuxParser::UpTime() {
  string uptime, idletime;
  string line;
  std::ifstream stream(kProcDirectory + kUptimeFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> uptime >> idletime;
  }
  return stol(uptime);
}

// TODO: Read and return CPU utilization
// check following ULR for implemtation details
// https://stackoverflow.com/questions/16726779/how-do-i-get-the-total-cpu-usage-of-an-application-from-proc-pid-stat/16736599#16736599
vector<string> LinuxParser::CpuUtilization() {
  vector<int> pids = Pids();
  vector<string> cpu_usages;
  int idx=-1;
  for (int pid : pids) {
    idx++;
    long uptime{UpTime()}, utime, stime, cutime, cstime, starttime;
    string line;
    string key;
    int i{0};
    std::ifstream filestream(kProcDirectory + std::to_string(pid) +
                             kStatFilename);
    if (filestream.is_open()) {
      std::getline(filestream, line);
      std::istringstream linestream(line);
      while (linestream && i < 22) {
        linestream >> key;
        i++;        
        if (i == 14)
          utime = std::stol(key);
        else if (i == 15)
          stime = std::stol(key);
        else if (i == 16)
          cutime = std::stol(key);
        else if (i == 17)
          cstime = std::stol(key);
        else if (i == 22)
          starttime = std::stol(key);
      }
      float total_time= (utime + stime + cutime + cstime) / sysconf(_SC_CLK_TCK);
      float spent_time = uptime - starttime / sysconf(_SC_CLK_TCK);
      float cpu_usage =0;
      if (spent_time>0){
        cpu_usage = total_time / spent_time;
      }
      cpu_usages.push_back(std::to_string(cpu_usage));
    }
  }
  return cpu_usages;
}

// TODO: Read and return the total number of processes
int LinuxParser::TotalProcesses() {
  string line;
  string key;
  int value;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      if (line.find("processes") != std::string::npos) {
        std::istringstream linestream(line);
        linestream >> key >> value;
        return value;
      }
    }
  }
  return 0;
}

// TODO: Read and return the number of running processes
int LinuxParser::RunningProcesses() {
  string line;
  string key;
  int value;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      if (line.find("procs_running") != std::string::npos) {
        std::istringstream linestream(line);
        linestream >> key >> value;
        return value;
      }
    }
  }
  return 0;
}

// TODO: Read and return the command associated with a process
string LinuxParser::Command(int pid) {
  string line;
  std::ifstream filestream(kProcDirectory + std::to_string(pid) +
                           kCmdlineFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    return line;
  }
  return string();
}

// TODO: Read and return the memory used by a process
string LinuxParser::Ram(int pid) {
  string line;
  string key;
  long value;
  string unit;
  std::ifstream filestream(kProcDirectory + std::to_string(pid) +
                           kStatusFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      if (line.find("VmSize") != std::string::npos) {
        std::istringstream linestream(line);
        linestream >> key >> value >> unit;
        // convert Megabytes
        return Format::SetFormat(float(value / 1024.0));
      }
    }
  }
  return string();
}

// TODO: Read and return the user ID associated with a process
string LinuxParser::Uid(int pid) {
  // find uid
  string line;
  string key;
  long value1, value2, value3, value4;
  string uid;
  std::ifstream filestream(kProcDirectory + std::to_string(pid) +
                           kStatusFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      if (line.find("Uid") != std::string::npos) {
        std::istringstream linestream(line);
        linestream >> key >> value1 >> value2 >> value3 >> value4;
        return std::to_string(value1);
      }
    }
  }
  return string();
}

// TODO: Read and return the user associated with a process
string LinuxParser::User(int pid) {
  // find username
  string uid = "x:" + Uid(pid);
  std::ifstream filestream(kPasswordPath);
  string username, line;
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      if (line.find(uid) != std::string::npos) {
        std::replace(line.begin(), line.end(), ':', ' ');
        std::istringstream linestream(line);
        linestream >> username;
        return username;
      }
    }
  }
  return string();
}

// TODO: Read and return the uptime of a process
long LinuxParser::UpTime(int pid) {
  string line;
  string key;
  int i{0};
  std::ifstream filestream(kProcDirectory + std::to_string(pid) +
                           kStatFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    while (linestream) {
      linestream >> key;
      i++;
      if (i == 22) return std::stol(key) / sysconf(_SC_CLK_TCK);
    }
  }
  return 0;
}