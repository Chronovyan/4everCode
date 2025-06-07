/**
 * @file chronovyan_stdlib.h
 * @brief Main header file for the Chronovyan Standard Library
 *
 * This file includes all the standard library components and provides
 * the main entry point for including the standard library in Chronovyan programs.
 *
 * @copyright Chronovyan Project
 */

#ifndef CHRONOVYAN_STDLIB_H
#define CHRONOVYAN_STDLIB_H

#include "exploration.h"
#include "paradox_management.h"
#include "pattern_management.h"
#include "resource_management.h"
#include "temporal_algorithms.h"
#include "temporal_manipulation.h"
#include "utility_functions.h"

namespace chronovyan {
namespace stdlib {

/**
 * @brief Initialize the Chronovyan Standard Library
 *
 * This function must be called before using any standard library functions.
 * It sets up the necessary internal state for the library to function properly.
 *
 * @return True if initialization was successful, false otherwise
 */
bool initialize();

/**
 * @brief Shutdown the Chronovyan Standard Library
 *
 * This function should be called when the program is done using the standard library.
 * It cleans up any resources that were allocated during library usage.
 *
 * @return True if shutdown was successful, false otherwise
 */
bool shutdown();

}  // namespace stdlib
}  // namespace chronovyan

#endif  // CHRONOVYAN_STDLIB_H