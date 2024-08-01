#ifndef TEST_SERVICE_POSTGRESQL_HPP
#define TEST_SERVICE_POSTGRESQL_HPP

#include <Advanced.hpp>
#include <DatabaseAPI/ServicePostgresqlDB.hpp>

const std::string rand_int = [](){
    srand(time(0)); 
    return std::to_string(rand());}();

class ServicePostgresql : public ::testing::Test
{
protected:
    int maxInsertRows;
    std::string ProjectFolder;
    std::string NameApp;
    std::string Windows_Command;
    std::string macOS_Command;
    std::string Linux_Command;
    std::string Table;
    std::string DevelopmentTable;
    DB::HashedDatabaseValues database_conn_data = {
        {"db_user","TestUser"},
        {"db_password","g7PAuarejhVzYtxXRG6bU2kK5dM8S3"}
    };
    // DB::ServicePostgresqlDB<DB::HashedDatabaseValues> database;
    // database.setConnectionData(database_conn_data);
    std::unique_ptr<DB::ServicePostgresqlDB> database_ptr;
    // database.setConnectionData(database_conn_data);
    // DB::ServicePostgresqlDB VersionsDatabase();
    DB::ServicePostgresqlDB *database;

    void SetUp()
    {
        try
        {
            maxInsertRows = 1000000;
            database_ptr = std::make_unique<DB::ServicePostgresqlDB>("http://localhost:8100/execute",&database_conn_data,"query","data");
            database = database_ptr.get();
            ProjectFolder = std::filesystem::current_path().generic_string();
            NameApp = "TestApp_" + rand_int;
            Windows_Command = "Test_Windows_Command";
            macOS_Command = "Test_macOS_Command";
            Linux_Command = "Test_Linux_Command";
            Table = "Test";
            DevelopmentTable = "DevelopmentPacks";
        }
        catch(std::exception& error)
        {
            std::cout << error.what() << std::endl;
        }
    }
};

#endif