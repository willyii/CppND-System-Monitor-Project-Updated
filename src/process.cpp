#include "process.h"

#include <unistd.h>

#include <cctype>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "linux_parser.h"

using std::string;
using std::to_string;
using std::vector;

Process::Process(int pid) {
  pid_ = pid;
  user_ = LinuxParser::User(pid_);
  cmd_ = LinuxParser::Command(pid_);
  ram_ = LinuxParser::Ram(pid_);
  uptime_ = LinuxParser::UpTime(pid_);
  active_ = LinuxParser::ActiveJiffies(pid_);
  util_ = ((double)active_ / sysconf(_SC_CLK_TCK)) / (double)uptime_;
}

int Process::Pid() { return pid_; }

float Process::CpuUtilization() { return util_; }

string Process::Command() { return cmd_.substr(0,40); }

string Process::Ram() { return ram_; }

string Process::User() { return user_; }

long int Process::UpTime() { return uptime_; }

bool Process::operator<(Process const& a) const { return util_ >= a.util_; }