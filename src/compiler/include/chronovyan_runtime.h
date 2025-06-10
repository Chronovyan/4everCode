#ifndef CHRONOVYAN_RUNTIME_H
#define CHRONOVYAN_RUNTIME_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Chronovyan Types
 */
typedef struct ChronType {
  int value;
  // Additional chronon properties
} ChronType;

typedef struct AethelType {
  int value;
  // Additional aethel properties
} AethelType;

/**
 * Runtime Structure
 */
typedef struct ChronovyanRuntime {
  int chronons_available;
  int aethel_available;
  bool chronons_borrowed;
  bool aethel_borrowed;
  // Additional runtime state
} ChronovyanRuntime;

/**
 * Runtime API
 */

// Initialize the Chronovyan runtime
ChronovyanRuntime *chronovyan_init() {
  ChronovyanRuntime *runtime =
      (ChronovyanRuntime *)malloc(sizeof(ChronovyanRuntime));
  if (runtime) {
    runtime->chronons_available = 0;
    runtime->aethel_available = 0;
    runtime->chronons_borrowed = false;
    runtime->aethel_borrowed = false;
  }
  return runtime;
}

// Clean up the Chronovyan runtime
void chronovyan_cleanup(ChronovyanRuntime *runtime) {
  if (runtime) {
    // Perform any necessary cleanup
    free(runtime);
  }
}

// Resource operations
void chronovyan_ALLOCATE_resources(ChronovyanRuntime *runtime, int chronons,
                                   int aethel) {
  if (runtime) {
    runtime->chronons_available += chronons;
    runtime->aethel_available += aethel;
    printf("Resources allocated: %d chronons, %d aethel\n", chronons, aethel);
  }
}

void chronovyan_EXPEND_resources(ChronovyanRuntime *runtime, int chronons,
                                 int aethel) {
  if (runtime) {
    if (runtime->chronons_available >= chronons &&
        runtime->aethel_available >= aethel) {
      runtime->chronons_available -= chronons;
      runtime->aethel_available -= aethel;
      printf("Resources expended: %d chronons, %d aethel\n", chronons, aethel);
    } else {
      fprintf(stderr, "Error: Insufficient resources available\n");
      exit(1);
    }
  }
}

void chronovyan_BORROW_resources(ChronovyanRuntime *runtime, int chronons,
                                 int aethel) {
  if (runtime) {
    if (!runtime->chronons_borrowed && !runtime->aethel_borrowed) {
      runtime->chronons_available += chronons;
      runtime->aethel_available += aethel;
      runtime->chronons_borrowed = chronons > 0;
      runtime->aethel_borrowed = aethel > 0;
      printf("Resources borrowed: %d chronons, %d aethel\n", chronons, aethel);
    } else {
      fprintf(stderr, "Error: Resources already borrowed\n");
      exit(1);
    }
  }
}

void chronovyan_REPAY_resources(ChronovyanRuntime *runtime, int chronons,
                                int aethel) {
  if (runtime) {
    if ((runtime->chronons_borrowed && chronons > 0) ||
        (runtime->aethel_borrowed && aethel > 0)) {
      if (runtime->chronons_available >= chronons &&
          runtime->aethel_available >= aethel) {
        runtime->chronons_available -= chronons;
        runtime->aethel_available -= aethel;
        runtime->chronons_borrowed =
            chronons > 0 ? false : runtime->chronons_borrowed;
        runtime->aethel_borrowed =
            aethel > 0 ? false : runtime->aethel_borrowed;
        printf("Resources repaid: %d chronons, %d aethel\n", chronons, aethel);
      } else {
        fprintf(stderr, "Error: Insufficient resources available to repay\n");
        exit(1);
      }
    } else {
      fprintf(stderr, "Error: No resources borrowed to repay\n");
      exit(1);
    }
  }
}

#ifdef __cplusplus
}
#endif

#endif // CHRONOVYAN_RUNTIME_H