#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "linux_parser.h"
#include "process.h"

using std::string;
using std::to_string;
using std::vector;
using namespace LinuxParser;

// TODO: Return this process's ID
int Process::Pid() { return pid_; }

// TODO: Return this process's CPU utilization
float Process::CpuUtilization() {
  vector<string> cpu = LinuxParser::CpuUtilization();  
  vector<int> pids = LinuxParser::Pids();
  int idx{-1};
  // for (int pid : pids) {
  //   idx++;
  //   if (pid == pid_) {
  //     break;
  //   }
  // }
  std::vector<int>::iterator itr = std::find(pids.begin(), pids.end(), pid_);
  if (itr != pids.cend()) {
    idx = std::distance(pids.begin(), itr);
  }
  return std::stof(cpu[idx]);
}

// TODO: Return the command that generated this process
string Process::Command() { return command_; }

// TODO: Return this process's memory utilization
string Process::Ram() { return ram_; }

// TODO: Return the user (name) that generated this process
string Process::User() { return user_; }

// TODO: Return the age of this process (in seconds)
long int Process::UpTime() { return uptime_; }

float Process::Cpu() {return cpu_;}

void Process::Pid(int pid) {
  if (pid > 0) {
    pid_ = pid;
    command_ = LinuxParser::Command(pid);
    ram_ = LinuxParser::Ram(pid);
    user_ = LinuxParser::User(pid);
    uptime_ = LinuxParser::UpTime()-LinuxParser::UpTime(pid);
    cpu_= this->CpuUtilization();

  }
}

// TODO: Overload the "less than" comparison operator for Process objects
bool Process::operator<(Process const& a) const { return cpu_ > a.cpu_; }