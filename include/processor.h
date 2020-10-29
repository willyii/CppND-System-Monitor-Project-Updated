#ifndef PROCESSOR_H
#define PROCESSOR_H

#include <vector>

using std::vector;

class Processor {
 public:
  Processor();
  float Utilization();  // TODO: See src/processor.cpp

  // DONE: Declare any necessary private members
 private:
  long prevIdle = 0, prevActive = 0, prevTotal = 0;
};

#endif