#ifndef CHRONOVYAN_STDLIB_CONFIG_H
#define CHRONOVYAN_STDLIB_CONFIG_H

// Platform-specific export/import macros for shared library support
#if defined(_WIN32) || defined(_WIN64)
#ifdef CHRONOVYAN_STDLIB_BUILDING_DLL
#define CHRONOVYAN_STDLIB_API __declspec(dllexport)
#else
#define CHRONOVYAN_STDLIB_API __declspec(dllimport)
#endif
#else
#if __GNUC__ >= 4
#define CHRONOVYAN_STDLIB_API __attribute__((visibility("default")))
#else
#define CHRONOVYAN_STDLIB_API
#endif
#endif

#endif  // CHRONOVYAN_STDLIB_CONFIG_H
