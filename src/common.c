#include "common.h"
#include "emulator.h"

void panic() {
    bochs_shutdown();
}
