#ifndef ADVANCED_HPP
#define ADVANCED_HPP
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstddef>
#include <unordered_map>
#include <map>
#include <vector>
#include <filesystem>
#include <typeinfo>
#include <memory>
#include <optional>
#define FMT_HEADER_ONLY
#include <fmt/format.h>
// #include <typeinfo>
#include <sstream>
#include <string>
#include <cstdint>

#define ERROR_EMPTY_PARAMETERS "Parameters is empty"
#define ERROR_EMPTY_EXCEPTIONS "Exceptions is empty"
#define ERROR_WRONG_TYPE "Unsupported operand type"
namespace DB
{
    using HashedDatabaseValues = std::unordered_map<std::string, std::string> ;
    using HashedEnumDatabaseValues = std::unordered_map<int, HashedDatabaseValues> ;
    using HashedEnumColDatabaseValues = std::unordered_map<int, std::string> ; // Enumerated columns
    using ArrayDatabaseValues = std::unique_ptr<std::string[]> ;

    using DatabaseValues = std::map<std::string, std::string> ;
    using EnumDatabaseValues = std::map<int, DatabaseValues> ;
    using EnumColDatabaseValues = std::map<int, std::string> ; // Enumerated columns

    // Method of make string to lower
    std::string to_lower(std::string sentence);
}

#endif