#ifndef STATEEXTREMESREPORT_H
#define STATEEXTREMESREPORT_H

#include <string>

/**
 * @brief Reads the LRF file and prints a table to standard output.
 *        The table lists, for each state (alphabetically),
 *        the Easternmost, Westernmost, Northernmost, and Southernmost Zip Code.
 *
 * Assumes the LRF file records have at least these fields:
 *   0: Zip_Code, 1: Place_Name, 2: State, 3: County, 4: Lat, 5: Long.
 *
 * @param convertedFilename The name of the LRF file to process.
 */
void runStateExtremesReport(const std::string& convertedFilename);

#endif // STATEEXTREMESREPORT_H
