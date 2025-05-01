#include "BSFHeaderRecordBuffer.h"
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <cstring>

// Constructor
BSFHeaderRecordBuffer::BSFHeaderRecordBuffer(const std::string& filePath)
    : filePath(filePath)
{
    // Nothing else to do here
}

// Get the header
const BSFFileHeader& BSFHeaderRecordBuffer::getHeader() const {
    return header;
}

// Set the header
void BSFHeaderRecordBuffer::setHeader(const BSFFileHeader& hdr) {
    header = hdr;
}

// Helper function to split a string by a delimiter
std::vector<std::string> BSFHeaderRecordBuffer::split(const std::string& s, char delim) {
    std::vector<std::string> tokens;
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        tokens.push_back(item);
    }
    return tokens;
}

// Read the header from the BSF file
void BSFHeaderRecordBuffer::readHeader() {
    std::ifstream in(filePath, std::ios::binary);
    if (!in.is_open()) {
        throw std::runtime_error("Cannot open BSF file: " + filePath);
    }

    // Read the fixed-size header block
    std::vector<char> buf(header.headerSize);
    in.read(buf.data(), header.headerSize);
    if (in.gcount() != static_cast<std::streamsize>(header.headerSize)) {
        throw std::runtime_error("Failed to read full BSF header block");
    }
    in.close();

    // Convert to string and split into lines
    std::string all(buf.begin(), buf.end());
    std::istringstream lines(all);
    std::string line;

    // ---- Line 1: general header ----
    if (!std::getline(lines, line)) {
        throw std::runtime_error("BSF header: missing general header line");
    }
    auto parts = split(line, ',');
    if (parts.size() < 8) {
        throw std::runtime_error("BSF header: invalid general header fields");
    }
    header.fileStructureType = parts[0];
    header.version = parts[1];
    header.headerSize = std::stoul(parts[2]);
    header.recordLengthFieldSize = std::stoul(parts[3]);
    header.sizeFormatType = parts[4];
    header.blockSize = std::stoul(parts[5]);
    header.minBlockFill = std::stof(parts[6]);
    header.indexFileName = parts[7];

    // ---- Line 2: field definitions ----
    if (!std::getline(lines, line)) {
        throw std::runtime_error("BSF header: missing field definitions line");
    }
    auto fldTokens = split(line, ',');
    header.fields.clear();
    for (auto& tok : fldTokens) {
        auto nv = split(tok, ':');
        if (nv.size() != 2) continue;
        BSFFieldInfo fi;
        fi.fieldName = nv[0];
        fi.typeSchema = nv[1];
        fi.isPrimaryKey = false;  // we'll mark the real PK in next line
        header.fields.push_back(fi);
    }
    header.fieldCount = header.fields.size();

    // ---- Line 3: primary key info ----
    if (!std::getline(lines, line)) {
        throw std::runtime_error("BSF header: missing primary key info line");
    }
    auto pk = split(line, ',');
    if (pk.size() < 4) {
        throw std::runtime_error("BSF header: invalid primary key line");
    }
    header.primaryKey.name = pk[0];
    header.primaryKey.ordinal = std::stoi(pk[1]);
    header.primaryKey.rbnLength = std::stoul(pk[2]);
    header.primaryKey.staleFlagPosition = std::stoi(pk[3]);

    // Mark the correct field as primary key
    for (size_t i = 0; i < header.fields.size(); ++i) {
        header.fields[i].isPrimaryKey = (static_cast<int>(i) == header.primaryKey.ordinal);
    }
}

// Write the header to the BSF file
void BSFHeaderRecordBuffer::writeHeader() {
    std::ofstream out(filePath, std::ios::binary | std::ios::in | std::ios::out);
    if (!out.is_open()) {
        // If file doesn't exist yet, create it
        out.open(filePath, std::ios::binary | std::ios::trunc);
    }
    if (!out) {
        throw std::runtime_error("Cannot open BSF file for writing: " + filePath);
    }

    // Build the three header lines
    std::ostringstream oss;

    // Line 1
    oss << header.fileStructureType << ","
        << header.version << ","
        << header.headerSize << ","
        << header.recordLengthFieldSize << ","
        << header.sizeFormatType << ","
        << header.blockSize << ","
        << header.minBlockFill << ","
        << header.indexFileName << ",\n";

    // Line 2: fields
    for (auto& f : header.fields) {
        oss << f.fieldName << ":" << f.typeSchema << ",";
    }
    // Replace trailing comma with newline
    {
        std::string s = oss.str();
        s.back() = '\n';
        oss.str(""); oss.clear();
        oss << s;
    }

    // Line 3: primary key info
    oss << header.primaryKey.name << ","
        << header.primaryKey.ordinal << ","
        << header.primaryKey.rbnLength << ","
        << header.primaryKey.staleFlagPosition << "\n";

    // Pad to headerSize
    std::string h = oss.str();
    if (h.size() > header.headerSize) {
        throw std::runtime_error("BSF header data exceeds headerSize");
    }
    h.resize(header.headerSize, ' ');

    // Write out
    out.seekp(0);
    out.write(h.c_str(), header.headerSize);
    out.close();
}

// Getter functions
std::string BSFHeaderRecordBuffer::getFileType() const {
    return header.fileStructureType;
}

std::string BSFHeaderRecordBuffer::getVersion() const {
    return header.version;
}

size_t BSFHeaderRecordBuffer::getRecordLengthFieldSize() const {
    return header.recordLengthFieldSize;
}

std::string BSFHeaderRecordBuffer::getSizeFormatType() const {
    return header.sizeFormatType;
}

std::string BSFHeaderRecordBuffer::getPrimaryKeyIndexFileName() const {
    return header.indexFileName;
}

size_t BSFHeaderRecordBuffer::getRecordCount() const {
    // Placeholder value for now
    return header.fieldCount;
}

size_t BSFHeaderRecordBuffer::getFieldCount() const {
    return header.fieldCount;
}

// Set the file type
void BSFHeaderRecordBuffer::setFileType(const std::string& newFileType) {
    header.fileStructureType = newFileType;
    writeHeader();  // Optionally update the header in the file immediately
}

// Set the version
void BSFHeaderRecordBuffer::setVersion(const std::string& newVersion) {
    header.version = newVersion;
    writeHeader();  // Optionally update the header in the file immediately
}

std::vector<std::string> BSFHeaderRecordBuffer::getFieldDefinitions() const {
    std::vector<std::string> fieldDefs;
    for (const auto& field : header.fields) {
        fieldDefs.push_back(field.fieldName + ":" + field.typeSchema);
    }
    return fieldDefs;
}
