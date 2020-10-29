#include "linux_parser.h"

#include <dirent.h>
#include <unistd.h>

#include <iostream>
#include <string>
#include <vector>

using std::stof;
using std::string;
using std::to_string;
using std::vector;

// Utils
template <typename T>
T LinuxParser::findValueByKey(std::string const &keyFilter, std::string const &filename) {
  std::string line, key;
  T value;

  std::ifstream stream(LinuxParser::kProcDirectory + filename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == keyFilter) {
          return value;
        }
      }
    }
  }
  return value;
};

template <typename T>
T LinuxParser::getValueOfFile(std::string const &filename) {
  std::string line;
  T value;

  std::ifstream stream(kProcDirectory + filename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> value;
  }
  return value;
};

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
  string os, kernel,version;// <----- Declared a new variable version 
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> kernel>>version;// <-- Extracting the version in a variable version
  }
  return version;//<---- Return the third variable
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
  string memTotal = "MemTotal:";
  string memFree = "MemFree:";
  float Total = findValueByKey<float>(memTotal, kMeminfoFilename);// "/proc/memInfo"
  float Free = findValueByKey<float>(memFree, kMeminfoFilename);
  return (Total - Free) / Total;
}

long LinuxParser::UpTime() {
  long uptime;
  string line;
  std::ifstream stream(kProcDirectory + kUptimeFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> uptime;
  }
  return uptime;
}

long LinuxParser::Jiffies() {
  vector<string> tmp = LinuxParser::CpuUtilization();
  long ans = 0;
  for (int i = kUser_; i <= kSteal_; i++) ans += std::stol(tmp[i]);
  return ans;
}

long LinuxParser::ActiveJiffies(int pid) {
  string v;
  long ret = 0;
  string line;
  std::ifstream stream(kProcDirectory + to_string(pid) + kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestring(line);
    for (int i = 0; i < 13; i++) linestring >> v;
    for (int i = 0; i < 4; i++) {
      linestring >> v;
      // if(v == "") continue;
      ret += stol(v);
    }
  }
  return ret;
}

long LinuxParser::ActiveJiffies() {
  vector<string> tmp = LinuxParser::CpuUtilization();
  long ans = 0;
  ans += std::stol(tmp[kUser_]);
  ans += std::stol(tmp[kNice_]);
  ans += std::stol(tmp[kSystem_]);
  ans += std::stol(tmp[kIRQ_]);
  ans += std::stol(tmp[kSoftIRQ_]);
  ans += std::stol(tmp[kSteal_]);
  return ans;
}

long LinuxParser::IdleJiffies() {
  vector<string> tmp = LinuxParser::CpuUtilization();
  long ans = 0;
  ans += std::stol(tmp[kIdle_]);
  ans += std::stol(tmp[kIOwait_]);
  return ans;
}

vector<string> LinuxParser::CpuUtilization() {
  vector<string> ret;
  string line, k, tmp;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestring(line);
      linestring >> k;
      if (k != "cpu") continue;
      for (int i = kUser_; i <= kGuestNice_; i++) {
        linestring >> tmp;
        ret.push_back(tmp);
      }
      return ret;
    }
  }
  return {};
}

int LinuxParser::TotalProcesses() {
  int totalProc;
  string line, k, value;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestring(line);
      linestring >> k;
      if (k == "processes") {
        linestring >> totalProc;
        return totalProc;
      }
    }
  }
  return 0;
}

int LinuxParser::RunningProcesses() {
  int totalProc;
  string line, k, value;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestring(line);
      linestring >> k;
      if (k == "procs_running") {
        linestring >> totalProc;
        return totalProc;
      }
    }
  }
  return 0;
}

string LinuxParser::Command(int pid) {
  return std::string(getValueOfFile<std::string>(std::to_string(pid) + kCmdlineFilename));
}

string LinuxParser::Ram(int pid) {
  
  string k;
  long v;
  string line;
  std::ifstream stream(kProcDirectory + to_string(pid) + kStatusFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestring(line);
      linestring >> k;
      if (k == "VmData:") {
        linestring >> v;
        return to_string(v / 1024);
      }
    }
  }
  return nullptr;
}

string LinuxParser::Uid(int pid) {
  string k;
  string v;
  string line;
  std::ifstream stream(kProcDirectory + to_string(pid) + kStatusFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestring(line);
      linestring >> k;
      if (k == "Uid:") {
        linestring >> v;
        return v;
      }
    }
  }
  return "";
}

string LinuxParser::User(int pid) {
  string uid = Uid(pid);
  // std::cout << "Strange: " << uid << std::endl;
  string user, x, tmp;
  ;
  string line;
  std::ifstream stream(kPasswordPath);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestring(line);
      linestring >> user >> x >> tmp;
      if (tmp == uid) {
        return user;
      }
    }
  }
  return string();
}

long LinuxParser::UpTime(int pid) {
  string v;
  string line;
  long sysUptime = UpTime();
  std::ifstream stream(kProcDirectory + to_string(pid) + kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestring(line);
    for (int i = 0; i < 22; i++) linestring >> v;
    return sysUptime - (std::stol(v) / sysconf(_SC_CLK_TCK));
  }
  return 0;
}