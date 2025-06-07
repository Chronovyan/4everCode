#include <direct.h>  // For _getcwd on Windows

#include <algorithm>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <vector>

#include "source_file.h"

namespace chronovyan {

SourceFile::SourceFile(const std::string& filename) : m_name(filename), m_path(filename) {
    std::cout << "SourceFile constructor called with filename: " << filename << std::endl;

    // Debug: Print current working directory
    char cwd[1024];
    if (_getcwd(cwd, sizeof(cwd)) != NULL) {
        std::cout << "Current working directory: " << cwd << std::endl;
    } else {
        std::cout << "Failed to get current working directory" << std::endl;
    }

    // Open the file in binary mode to properly handle encodings
    std::ifstream file(filename, std::ios::binary);
    if (!file) {
        std::cerr << "Failed to open file in SourceFile constructor: " << filename << std::endl;
        // Instead of using the filename as content, throw an exception
        throw std::runtime_error("Could not open file: " + filename);
    }

    std::cout << "Successfully opened file in SourceFile constructor" << std::endl;

    // Read the file content into a byte vector
    std::vector<char> buffer(std::istreambuf_iterator<char>(file), {});
    std::cout << "Read " << buffer.size() << " bytes from file" << std::endl;

    // Check for UTF-16 BOM (Byte Order Mark)
    if (buffer.size() >= 2) {
        // UTF-16 LE BOM (FF FE)
        if (static_cast<unsigned char>(buffer[0]) == 0xFF &&
            static_cast<unsigned char>(buffer[1]) == 0xFE) {
            // Convert UTF-16LE to UTF-8
            std::string utf8;
            for (size_t i = 2; i < buffer.size(); i += 2) {
                if (i + 1 < buffer.size()) {
                    // Get UTF-16 character (little endian)
                    uint16_t utf16Char = (static_cast<unsigned char>(buffer[i + 1]) << 8) |
                                         static_cast<unsigned char>(buffer[i]);

                    // Simple conversion for ASCII range (common for code)
                    if (utf16Char < 128) {
                        utf8 += static_cast<char>(utf16Char);
                    } else if (utf16Char < 2048) {
                        // 2-byte UTF-8
                        utf8 += static_cast<char>(0xC0 | (utf16Char >> 6));
                        utf8 += static_cast<char>(0x80 | (utf16Char & 0x3F));
                    } else {
                        // 3-byte UTF-8
                        utf8 += static_cast<char>(0xE0 | (utf16Char >> 12));
                        utf8 += static_cast<char>(0x80 | ((utf16Char >> 6) & 0x3F));
                        utf8 += static_cast<char>(0x80 | (utf16Char & 0x3F));
                    }
                }
            }
            m_source = utf8;
            std::cout << "Converted UTF-16LE file to UTF-8" << std::endl;
        } else {
            // UTF-8 BOM (EF BB BF)
            if (buffer.size() >= 3 && static_cast<unsigned char>(buffer[0]) == 0xEF &&
                static_cast<unsigned char>(buffer[1]) == 0xBB &&
                static_cast<unsigned char>(buffer[2]) == 0xBF) {
                // Skip the BOM
                m_source = std::string(buffer.begin() + 3, buffer.end());
                std::cout << "UTF-8 with BOM file detected, BOM removed" << std::endl;
            } else {
                // Regular file (ASCII or UTF-8 without BOM)
                m_source = std::string(buffer.begin(), buffer.end());
                std::cout << "Regular file content loaded (ASCII or UTF-8 without BOM)"
                          << std::endl;
            }
        }
    } else {
        // File too small to have a BOM, just copy the content
        m_source = std::string(buffer.begin(), buffer.end());
        std::cout << "Small file content loaded (no BOM)" << std::endl;
    }

    std::cout << "Final source content size: " << m_source.size() << " bytes" << std::endl;
    if (!m_source.empty()) {
        std::cout << "First 20 chars: [" << m_source.substr(0, 20) << "]" << std::endl;
    }

    indexLines();
}

SourceFile::SourceFile(std::string&& source, const std::string& sourceName)
    : m_source(std::move(source)), m_name(sourceName), m_path(sourceName) {
    indexLines();
}

const std::string& SourceFile::getSource() const { return m_source; }

const std::string& SourceFile::getName() const { return m_name; }

const std::string& SourceFile::getPath() const { return m_path; }

std::string SourceFile::getLine(size_t lineNumber) const {
    if (lineNumber < 1 || lineNumber > m_lineOffsets.size()) {
        throw std::out_of_range("Line number out of range: " + std::to_string(lineNumber));
    }

    size_t start = m_lineOffsets[lineNumber - 1];
    size_t end;

    if (lineNumber == m_lineOffsets.size()) {
        // Last line
        end = m_source.length();
    } else {
        // Not the last line
        end = m_lineOffsets[lineNumber];

        // Don't include the newline character
        if (end > 0 && (m_source[end - 1] == '\n' || m_source[end - 1] == '\r')) {
            --end;
            // Handle CRLF
            if (end > 0 && m_source[end - 1] == '\r' && m_source[end] == '\n') {
                --end;
            }
        }
    }

    return m_source.substr(start, end - start);
}

size_t SourceFile::getPosition(size_t line, size_t column) const {
    if (line < 1 || line > m_lineOffsets.size()) {
        throw std::out_of_range("Line number out of range: " + std::to_string(line));
    }

    size_t lineStart = m_lineOffsets[line - 1];
    size_t lineEnd;

    if (line == m_lineOffsets.size()) {
        // Last line
        lineEnd = m_source.length();
    } else {
        // Not the last line
        lineEnd = m_lineOffsets[line];
    }

    if (column < 1 || lineStart + column - 1 > lineEnd) {
        throw std::out_of_range("Column number out of range: " + std::to_string(column));
    }

    return lineStart + column - 1;
}

std::pair<size_t, size_t> SourceFile::getLineAndColumn(size_t position) const {
    if (position >= m_source.length()) {
        // If position is at the end of the file, return the last line and its length
        size_t lastLine = m_lineOffsets.size();
        size_t lastLineStart = m_lineOffsets[lastLine - 1];
        return {lastLine, position - lastLineStart + 1};
    }

    // Binary search to find the line containing position
    auto it = std::upper_bound(m_lineOffsets.begin(), m_lineOffsets.end(), position);

    if (it == m_lineOffsets.begin()) {
        // Position is before the first line
        return {1, position + 1};
    }

    size_t line = std::distance(m_lineOffsets.begin(), it);
    size_t lineStart = *(it - 1);

    return {line, position - lineStart + 1};
}

void SourceFile::indexLines() {
    m_lineOffsets.clear();
    m_lineOffsets.push_back(0);  // First line starts at index 0

    for (size_t i = 0; i < m_source.length(); ++i) {
        if (m_source[i] == '\n') {
            m_lineOffsets.push_back(i + 1);
        } else if (m_source[i] == '\r') {
            if (i + 1 < m_source.length() && m_source[i + 1] == '\n') {
                // CRLF - skip the CR
                i++;
            }
            m_lineOffsets.push_back(i + 1);
        }
    }

    if (m_lineOffsets.size() == 1 && !m_source.empty()) {
        // If there are no newlines, add the end of the string
        m_lineOffsets.push_back(m_source.length());
    }
}

}  // namespace chronovyan