#pragma once
#ifndef BSFRECORD_H
#define BSFRECORD_H

#include <string>

struct ZipCodeRecord {
    std::string zip;
    std::string place;
    std::string state;
    std::string county;
    std::string lat;
    std::string lon;
};

#endif