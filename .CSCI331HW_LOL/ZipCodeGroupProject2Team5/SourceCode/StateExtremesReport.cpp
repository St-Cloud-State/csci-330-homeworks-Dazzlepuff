#include "StateExtremesReport.h"
#include "LRFBuffer.h"
#include <iostream>
#include <iomanip>
#include <map>
#include <vector>
#include <string>

// Helper structure to hold data extremes for a state.
struct StateExtremes {
    std::string easternmostZip;
    std::string westernmostZip;
    std::string northernmostZip;
    std::string southernmostZip;

    double easternmostLon; // For easternmost: maximum longitude
    double westernmostLon; // For westernmost: minimum longitude
    double northernmostLat; // For northernmost: maximum latitude
    double southernmostLat; // For southernmost: minimum latitude

    // Initializes all extremes to the first record's values.
    StateExtremes(const std::string& zip, double lat, double lon)
        : easternmostZip(zip), westernmostZip(zip),
        northernmostZip(zip), southernmostZip(zip),
        easternmostLon(lon), westernmostLon(lon),
        northernmostLat(lat), southernmostLat(lat)
    { }
};

void runStateExtremesReport(const std::string& lrfFilename) {
    try {
        // Open the LRF file using our LRFBuffer.
        LRFBuffer buffer(lrfFilename);

        // Map state (key) to its extremes.
        std::map<std::string, StateExtremes> stateMap;
        std::vector<std::string> record;

        // Process each record.
        while (buffer.getNextRecord(record)) {
            // Ensure record has at least 6 fields.
            if (record.size() < 6)
                continue;

            std::string zip = record[0];
            std::string state = record[2];
            double lat = std::stod(record[4]);
            double lon = std::stod(record[5]);

            // Insert new state if not already present.
            auto it = stateMap.find(state);
            if (it == stateMap.end()) {
                stateMap.emplace(state, StateExtremes(zip, lat, lon));
            }
            else {
                StateExtremes& ext = it->second;
                // Easternmost: greater longitude.
                if (lon > ext.easternmostLon) {
                    ext.easternmostLon = lon;
                    ext.easternmostZip = zip;
                }
                // Westernmost: smaller longitude.
                if (lon < ext.westernmostLon) {
                    ext.westernmostLon = lon;
                    ext.westernmostZip = zip;
                }
                // Northernmost: greater latitude.
                if (lat > ext.northernmostLat) {
                    ext.northernmostLat = lat;
                    ext.northernmostZip = zip;
                }
                // Southernmost: smaller latitude.
                if (lat < ext.southernmostLat) {
                    ext.southernmostLat = lat;
                    ext.southernmostZip = zip;
                }
            }
        }

        std::cout << "\nState   | Easternmost Zip | Westernmost Zip | Northernmost Zip | Southernmost Zip\n";
        std::cout << "--------------------------------------------------------------------------\n";

        // Iterate through the states in alphabetically
        for (const auto& pair : stateMap) {
            const std::string& state = pair.first;
            const StateExtremes& ext = pair.second;
            std::cout << std::left << std::setw(8) << state
                << std::setw(17) << ext.easternmostZip
                << std::setw(17) << ext.westernmostZip
                << std::setw(18) << ext.northernmostZip
                << std::setw(18) << ext.southernmostZip << "\n";
        }
    }
    catch (const std::exception& e) {
        std::cerr << "Error in runStateExtremesReport: " << e.what() << std::endl;
    }
}
