#include "buffer.h"
#include <algorithm> 
#include <random>

void Buffer::loadCSV() {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error opening file: " << filename << std::endl;
        return;
    }

    std::string line;
    bool isFirstLine = true; // To skip the header row

    while (std::getline(file, line)) {
        if (isFirstLine) { // Skip header row
            isFirstLine = false;
            continue;
        }

        std::stringstream ss(line);
        std::string zip, place, state, county, lat, lon;

        if (!std::getline(ss, zip, ',') || 
            !std::getline(ss, place, ',') || 
            !std::getline(ss, state, ',') || 
            !std::getline(ss, county, ',') || 
            !std::getline(ss, lat, ',') || 
            !std::getline(ss, lon, ',')) {
            
            continue;
        }

        // Validate numeric fields before conversion
        if (zip.empty() || lat.empty() || lon.empty() || 
            !std::isdigit(zip[0]) || !std::isdigit(lat[0]) || !std::isdigit(lon[0])) {
            
            continue;
        }

        try {
            int zipCode = std::stoi(zip);
            double latitude = std::stod(lat);
            double longitude = std::stod(lon);
            
            records.push_back({zipCode, place, state, county, latitude, longitude});
        } catch (const std::exception& e) {
            std::cerr << "Error converting data: " << e.what() << " | Row: " << line << std::endl;
        }
    }


    file.close();

    //shuffles records to prove its order independent, fulfilling #6 requirement
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(records.begin(), records.end(), g);
    
}
