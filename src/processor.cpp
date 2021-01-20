#include "processor.h"
#include "linux_parser.h"

using namespace LinuxParser;
using namespace std;

// TODO: Return the aggregate CPU utilization
float Processor::Utilization() {
  vector<string> cpu = LinuxParser::CpuUtilization();
  float sum=0;
  for(string cpu_:cpu){
    sum =+ stof(cpu_);
  }
  return sum;
}