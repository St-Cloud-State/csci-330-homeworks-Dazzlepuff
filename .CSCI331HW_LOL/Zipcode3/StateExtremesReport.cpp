#include "StateExtremesReport.h"
#include "LRFBuffer.h"
#include "BSFBlockBuffer.h"
#include "BSFRecordBuffer.h"
#include "BSFRecord.h"

#include <iostream>
#include <iomanip>
#include <map>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <algorithm>

// Helper structure to hold data extremes for a state.
struct StateExtremes {
    std::string easternmostZip;
    std::string westernmostZip;
    std::string northernmostZip;
    std::string southernmostZip;

    double easternmostLon;
    double westernmostLon;
    double northernmostLat;
    double southernmostLat;

    StateExtremes(const std::string& zip, double lat, double lon)
        : easternmostZip(zip), westernmostZip(zip),
        northernmostZip(zip), southernmostZip(zip),
        easternmostLon(lon), westernmostLon(lon),
        northernmostLat(lat), southernmostLat(lat) {}
};

static std::string toLower(const std::string& s) {
    std::string result = s;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

void runStateExtremesReport(const std::string& convertedFilename) {
    try {
        std::string lowerName = toLower(convertedFilename);
        std::map<std::string, StateExtremes> stateMap;

        if (lowerName.size() >= 4 && lowerName.substr(lowerName.size() - 4) == ".lrf") {
            // === LRF Mode ===
            LRFBuffer buffer(convertedFilename);
            std::vector<std::string> record;

            while (buffer.getNextRecord(record)) {
                if (record.size() < 6) continue;

                std::string zip = record[0];
                std::string state = record[2];
                double lat = std::stod(record[4]);
                double lon = std::stod(record[5]);

                auto& ext = stateMap.emplace(state, zip, lat, lon).first->second;
                if (lon > ext.easternmostLon) ext.easternmostLon = lon, ext.easternmostZip = zip;
                if (lon < ext.westernmostLon) ext.westernmostLon = lon, ext.westernmostZip = zip;
                if (lat > ext.northernmostLat) ext.northernmostLat = lat, ext.northernmostZip = zip;
                if (lat < ext.southernmostLat) ext.southernmostLat = lat, ext.southernmostZip = zip;
            }

        }
        else if (lowerName.size() >= 4 && lowerName.substr(lowerName.size() - 4) == ".bsf") {
            // === BSF Mode ===
            std::ifstream in(convertedFilename, std::ios::binary);
            if (!in.is_open()) {
                std::cerr << "Could not open BSF file: " << convertedFilename << "\n";
                return;
            }

            int rbn = 3;
            BSFBlockBuffer block;

            while (block.readBlock(in, rbn)) {
                for (const std::string& raw : block.getAllRawRecords()) {
                    BSFRecordBuffer recBuf(raw);
                    ZipCodeRecord rec = recBuf.unpack();

                    try {
                        std::string zip = rec.zip;
                        std::string state = rec.state;
                        double lat = std::stod(rec.lat);
                        double lon = std::stod(rec.lon);

                        auto& ext = stateMap.emplace(state, zip, lat, lon).first->second;
                        if (lon > ext.easternmostLon) ext.easternmostLon = lon, ext.easternmostZip = zip;
                        if (lon < ext.westernmostLon) ext.westernmostLon = lon, ext.westernmostZip = zip;
                        if (lat > ext.northernmostLat) ext.northernmostLat = lat, ext.northernmostZip = zip;
                        if (lat < ext.southernmostLat) ext.southernmostLat = lat, ext.southernmostZip = zip;
                    }
                    catch (...) {
                        continue;
                    }
                }
                rbn = block.getNextRBN();
                if (rbn <= 0) break;
            }

        }
        else {
            std::cerr << "Unknown file format. File must end in .lrf or .bsf\n";
            return;
        }

        // === Report Output ===
        std::cout << "\nState   | Easternmost Zip | Westernmost Zip | Northernmost Zip | Southernmost Zip\n";
        std::cout << "----------------------------------------------------------------------------------\n";

        for (auto it = stateMap.begin(); it != stateMap.end(); ++it) {
            const std::string& state = it->first;
            const StateExtremes& ext = it->second;
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
