#include "processor.h"
#include "linux_parser.h"
#include <vector>
#include <iostream>

using std::vector;

// DONE: Return the aggregate CPU utilization
float Processor::Utilization() { 
    long currTotal = LinuxParser::Jiffies();
    long currIdle = LinuxParser::IdleJiffies();
    long currActive = LinuxParser::ActiveJiffies();
    
    float totald = currTotal - prevTotal ;
    float activeld = currActive - prevActive ;
    float ans = activeld / totald;

    prevTotal = currTotal;
    prevIdle = currIdle;
    prevActive = currActive;
    if(!(ans >= 0.0)){
        return 0.0;
    }
    return ans;
 }

Processor::Processor(){
    prevTotal = LinuxParser::Jiffies();
    prevIdle = LinuxParser::IdleJiffies();
    prevActive = LinuxParser::ActiveJiffies();
}