#include <DatabaseAPI/ServicePostgresqlDB.hpp>
#include <json/json.h>
#include <iostream>
#include <unordered_map>
#include <string>
#include <optional>

std::unordered_map<std::string, std::unordered_map<std::string, std::string>> jsonToMap(const Json::Value& json) {
    std::unordered_map<std::string, std::unordered_map<std::string, std::string>> map;

    if (json.isObject() && json.isMember("data") && json["data"].isArray()) {
        int index = 0;
        for (const auto& arrayValue : json["data"]) {
            std::unordered_map<std::string, std::string> innerMap;
            for (const auto& key : arrayValue.getMemberNames()) {
                innerMap[key] = arrayValue[key].asString();
            }
            map[std::to_string(index)] = innerMap;
            index++;
        }
    }

    return map;
}

int main()
{
    try
    {
    Json::Value data;
    // data["db_user"] = "deepforge-database_default_user";
    // data["db_password"] = "g7PAuarejhVzYtxXRG6bU2kK5dM8S3";
    // data["query"] = "SELECT * FROM \"Applications\";";
    const std::string url = "http://localhost:8100/execute";
    std::unordered_map<std::string,std::string> connection_data = {
        {"db_user","TestUser"},
        {"db_password","g7PAuarejhVzYtxXRG6bU2kK5dM8S3"},
        {"query","SELECT * FROM \"windowsversions\";"}
    };
    DB::ServicePostgresqlDB service;
    service.setConnectionData(connection_data).setUrl("http://localhost:8100/execute").setJsonDataStr("data");
    // Json::Value output = service.sendQuery<DB::HashedDatabaseValues>(nullptr);
    std::cout << service.ExistTableInDB("Test") << std::endl;
    // std::cout << output["count"].asString() << std::endl;
    // std::unordered_map<std::string, std::unordered_map<std::string, std::string>> map = jsonToMap(output);

    // for (const auto& pair : map) {
    //     std::cout << "Index: " << pair.first << std::endl;
    //     for (const auto& innerPair : pair.second) {
    //         std::cout << "  " << innerPair.first << ": " << innerPair.second << std::endl;
    //     }
    // }
    // for (const auto& item : output["data"]) {
    //     std::cout << "ID: " << item["id"].asString() << std::endl;
    //     std::cout << "Name: " << item["Name"].asString() << std::endl;
    //     std::cout << "Windows: " << item["Windows"].asString() << std::endl;
    //     std::cout << "Linux_amd64: " << item["Linux_amd64"].asString() << std::endl;
    //     std::cout << "Linux_arm64: " << item["Linux_arm64"].asString() << std::endl;
    //     std::cout << "macOS: " << item["macOS"].asString() << std::endl;
    //     std::cout << std::endl;
    // }
    }
    catch(std::exception &error)
    {
        std::cerr << error.what() << std::endl;
    }
    return 0;
}