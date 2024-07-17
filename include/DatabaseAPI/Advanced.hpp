#ifndef ADVANCED_HPP
#define ADVANCED_HPP
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <sqlite3/sqlite3.h>
#include <unordered_map>
#include <filesystem>
#include <typeinfo>
#include <memory>
#include <optional>
#define FMT_HEADER_ONLY
#include <fmt/format.h>

namespace DB
{
    typedef std::unordered_map<std::string, std::string> DatabaseValues;
    typedef std::unordered_map<int, DatabaseValues> EnumDatabaseValues;
    typedef std::unordered_map<int, std::string> EnumColDatabaseValues; // Enumerated columns
    typedef std::unique_ptr<std::string[]> ArrayDatabaseValues;

    // Method of make string to lower
    std::string to_lower(std::string sentence);
}

#endif