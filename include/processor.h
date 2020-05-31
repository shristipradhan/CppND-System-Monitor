#ifndef PROCESSOR_H
#define PROCESSOR_H

#include "linux_parser.h"

class Processor {
 public:
  double Utilization();

 private:
    long prev_active;
    long prev_idle;
};

#endif