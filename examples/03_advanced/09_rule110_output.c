#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Chronovyan runtime
#include "chronovyan_runtime.h"

int main(int argc, char** argv) {
    // Initialize Chronovyan runtime
    ChronovyanRuntime* runtime = chronovyan_init();

    // Cleanup Chronovyan runtime
    chronovyan_cleanup(runtime);
    return 0;
}
