#include "processor.h"
#include <vector>

float Processor::Utilization() {
    std::vector<long> cpuUtilization_data = LinuxParser::CpuUtilization();

    float utilization = 0.0;
    long idle, active;
    idle = cpuUtilization_data[0];
    active = cpuUtilization_data[1];

    long idle_d = idle - prev_idle;
    long active_d = active - prev_active;

    utilization = (float)active_d/(active_d + idle_d);

    prev_idle = cpuUtilization_data[0];
    prev_active = cpuUtilization_data[1];

    return utilization;
}