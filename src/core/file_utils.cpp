#include <algorithm>
#include <cctype>
#include <string>

/**
 * Checks if a filename has a valid .cvy extension.
 *
 * @param filename The filename to check
 * @return true if the filename has a valid .cvy extension, false otherwise
 */
bool hasValidExtension(const std::string& filename) {
    // File must be at least 5 characters (x.cvy)
    if (filename.size() < 5) {
        return false;
    }

    // Find the last dot position
    size_t dotPos = filename.find_last_of('.');
    if (dotPos == std::string::npos || dotPos == 0) {
        return false;  // No extension or just the extension
    }

    // Get the base filename (before the extension)
    std::string baseName = filename.substr(0, dotPos);
    if (baseName.size() < 2) {
        return false;  // Base filename too short (at least 2 chars)
    }

    // Check for multiple extensions
    if (baseName.find('.') != std::string::npos) {
        return false;  // Multiple extensions found
    }

    // Check if the extension is .cvy (case insensitive)
    std::string extension = filename.substr(dotPos);
    std::string lowercaseExt = extension;
    std::transform(lowercaseExt.begin(), lowercaseExt.end(), lowercaseExt.begin(),
                   [](unsigned char c) { return std::tolower(c); });

    return lowercaseExt == ".cvy";
}