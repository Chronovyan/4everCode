#ifndef FILE_UTILS_H
#define FILE_UTILS_H

#include <string>

/**
 * Checks if a filename has a valid .cvy extension.
 *
 * @param filename The filename to check
 * @return true if the filename has a valid .cvy extension, false otherwise
 */
bool hasValidExtension(const std::string& filename);

#endif  // FILE_UTILS_H