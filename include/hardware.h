#ifndef HARDWARE_H
#define HARDWARE_H

#include "menu.h"

typedef enum {
    CPU_AMD,
    CPU_INTEL
} CpuVendor;

typedef enum {
    GPU_AMD,
    GPU_NVIDIA_PROPRIETARY,
    GPU_NVIDIA_OPEN,
    GPU_INTEL
} GpuVendor;

typedef struct {
    CpuVendor cpu;
    GpuVendor gpu;
} HwSelection;

/* Show an ncurses arrow-key selector for CPU then GPU.
   Returns 0 on success, -1 if user cancelled.          */
int hw_select(HwSelection *out);

/* Run the hardware-aware install flow */
void hw_run_flow(HwFlow flow, const HwSelection *hw);

#endif /* HARDWARE_H */
