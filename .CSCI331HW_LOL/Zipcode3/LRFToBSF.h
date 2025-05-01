#ifndef LRFTOBSF_H
#define LRFTOBSF_H

#include <string>

/// @brief Converts a CSV file to a structured file format with header metadata and length-indicated records. New file is formatted as our custome Length Record File or .lrf.
/// @param csvFile Path to the input CSV file.
/// @param outputFile Path to the output structured file or .lrf file.
void LRFToBSF(const std::string& csvFile, const std::string& outputFile);

#endif