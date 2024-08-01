#include <UnitTest/ServicePostgresql.hpp>

TEST_F(ServicePostgresql, DeleteTable)
{
    bool expression;
    database->DeleteTable(Table);
    expression = database->ExistTableInDB(Table);
    EXPECT_FALSE(expression);

    database->DeleteTable("WindowsVersions");
    expression = database->ExistTableInDB("WindowsVersions");
    EXPECT_FALSE(expression);
}

TEST_F(ServicePostgresql, CreateTable)
{
    int result;
    DB::HashedDatabaseValues columns;
    columns = {{"Name", "TEXT"}, {"Windows", "TEXT"}, {"Linux", "TEXT"}, {"macOS", "TEXT"}};
    result = database->CreateTable<DB::HashedDatabaseValues>(Table, columns);
    EXPECT_EQ(result, 0);

    columns = {{"Version", "VARCHAR(50)"}, {"Channel", "VARCHAR(100)"}, {"Architecture", "VARCHAR(100)"}, {"Url", "TEXT"}, {"Url_arm64", "TEXT"}};
    result = database->CreateTable<DB::HashedDatabaseValues>("WindowsVersions", columns);
    EXPECT_EQ(result, 0);
}

TEST_F(ServicePostgresql, InsertValue)
{
    int result;
    DB::HashedDatabaseValues values;
    for(size_t i = 0;i < maxInsertRows; i++)
    {
        values = {{"Name", NameApp + std::to_string(i)}, {"Windows", Windows_Command + std::to_string(i)}, {"Linux", Linux_Command + std::to_string(i)}, {"macOS", macOS_Command + std::to_string(i)}};
        result = database->InsertRowToTable<DB::HashedDatabaseValues>(Table, values);
        EXPECT_EQ(result, 0);
    }

    values = {{"Version", "0.1"}, {"Channel", "stable"}, {"Architecture", "amd64"}, {"Url", "https://github.com/DeepForge-Tech/DeepForge-Toolset/releases/download/v0.1_linux_amd64/DeepForge-Toolset_0.1_linux_amd64.zip"}, {"Url_arm64", "Empty"}};
    result = database->InsertRowToTable("WindowsVersions", values);
    EXPECT_EQ(result, 0);

    values = {{"Version", "0.2"}, {"Channel", "stable"}, {"Architecture", "amd64"}, {"Url", "https://github.com/DeepForge-Tech/DeepForge-Toolset/releases/download/v0.2_linux_amd64/DeepForge-Toolset_0.2_linux_amd64.zip"}, {"Url_arm64", "Empty"}};
    result = database->InsertRowToTable("WindowsVersions", values);
    EXPECT_EQ(result, 0);
}

TEST_F(ServicePostgresql, GetValue)
{
    DB::HashedDatabaseValues parameters;
    std::string output;
    bool find = false;
    parameters = {{"Name", NameApp}};
    output = database->GetValueFromRow<DB::HashedDatabaseValues>(Table, "Windows", parameters, std::nullopt);
    EXPECT_STREQ(Windows_Command.c_str(), output.c_str());
}

TEST_F(ServicePostgresql, GetColumns)
{
    std::vector<DB::HashedDatabaseValues> db_rows;
    DB::HashedDatabaseValues parameters;
    std::vector<std::string> columns = {"id","Name"};
    bool find;
    parameters = {{"Windows", Windows_Command}};
    db_rows = database->GetColumnsFromTable<DB::HashedDatabaseValues>(Table, columns, parameters, std::nullopt);
    for(auto& data : db_rows)
    {
        if (data["name"] == NameApp)
        {
            find = true;
        }

    }
    EXPECT_TRUE(find);
}

TEST_F(ServicePostgresql, GetMaxValue)
{
    DB::HashedDatabaseValues parameters;
    std::string result;
    parameters = {{"Channel", "stable"}, {"Architecture", "amd64"}};
    result = database->GetMaxValueFromTable<DB::HashedDatabaseValues>("WindowsVersions", "Version", parameters,std::nullopt);
    EXPECT_STREQ("0.2", result.c_str());
}

TEST_F(ServicePostgresql, GetMaxRows)
{
    DB::HashedDatabaseValues parameters;
    std::vector<DB::HashedDatabaseValues> db_rows;
    parameters = {{"Channel", "stable"}, {"Architecture", "amd64"}};
    db_rows = database->GetMaxRowsFromTable<DB::HashedDatabaseValues>("WindowsVersions", "Version", parameters,std::nullopt);
    for(auto& data : db_rows)
    {
        EXPECT_STREQ(data["version"].c_str(), "0.2");
    }
}

// TEST_F(ServicePostgresql, GetRow)
// {
//     DB::HashedEnumDatabaseValues db_rows;
//     DB::HashedDatabaseValues parameters;
//     parameters = {{"Name", NameApp}};
//     db_rows = database.GetRowFromTable(Table, parameters, std::nullopt);
//     EXPECT_STREQ(Windows_Command.c_str(), db_rows[0]["Windows"].c_str());
// }

TEST_F(ServicePostgresql, GetRowByID)
{
    DB::HashedDatabaseValues db_values;
    bool expression;
    db_values = database->GetRowByID<DB::HashedDatabaseValues>(Table, 1);
    expression = Windows_Command == db_values["windows"] && Linux_Command == db_values["linux"] && macOS_Command == db_values["macos"];
    EXPECT_TRUE(expression);
}

// TEST_F(ServicePostgresql, GetAllRows)
// {
//     DB::HashedEnumDatabaseValues db_rows;
//     bool expression;
//     db_rows = database.GetAllRowsFromTable(Table);
//     expression = Windows_Command == db_rows[0]["Windows"];
//     EXPECT_TRUE(expression);
// }

// TEST_F(ServicePostgresql, ExecuteQuery)
// {
//     DB::HashedEnumDatabaseValues db_rows;
//     bool expression;
//     db_rows = database.ExecuteQuery("SELECT * FROM " + Table);
//     expression = Windows_Command == db_rows[0]["Windows"];
//     EXPECT_TRUE(expression);
// }

// TEST_F(ServicePostgresql, RunQuery)
// {
//     int result;
//     bool expression;
//     DB::HashedDatabaseValues db_values;
//     DB::HashedDatabaseValues values;
//     values = {
//         {"Windows", "Updated_Test_Windows_Command"},
//         {"Linux", "Updated_Test_Linux_Command"},
//         {"macOS", "Updated_Test_macOS_Command"}};
//     result = database.RunQuery("UPDATE Test SET Windows='Updated_Test_Windows_Command',macOS='Updated_Test_macOS_Command',Linux='Updated_Test_Linux_Command' WHERE Windows='Test_Windows_Command' AND macOS='Test_macOS_Command' AND Linux='Test_Linux_Command';");
//     db_values = database.GetRowByID(Table, 1);
//     expression = result == 0 && values["Windows"] == db_values["Windows"] && values["Linux"] == db_values["Linux"] && values["macOS"] == db_values["macOS"];
//     EXPECT_TRUE(expression);
// }

TEST_F(ServicePostgresql, UpdateValues)
{
    DB::HashedDatabaseValues db_values;
    DB::HashedDatabaseValues values;
    DB::HashedDatabaseValues parameters;
    values = {
        {"Windows", "Updated_Test_Windows_Command"},
        {"Linux", "Updated_Test_Linux_Command"},
        {"macOS", "Updated_Test_macOS_Command"}};
    parameters = {{"Windows", Windows_Command}, {"Linux", Linux_Command}, {"macOS", macOS_Command}};
    database->UpdateRowInTable<DB::HashedDatabaseValues>(Table, values, parameters,std::nullopt);
    db_values = database->GetRowByID<DB::HashedDatabaseValues>(Table, 1);
    for(auto& data : db_values)
    {
        EXPECT_STREQ(values["Windows"].c_str(),db_values["windows"].c_str());
        EXPECT_STREQ(values["Linux"].c_str(),db_values["linux"].c_str());
        EXPECT_STREQ(values["macOS"].c_str(),db_values["macos"].c_str());
    }
}

TEST_F(ServicePostgresql, CountRows)
{
    uint32_t result;
    DB::HashedDatabaseValues parameters;
    parameters = {{"Name", NameApp}};
    result = database->CountRowsInTable<uint32_t,DB::HashedDatabaseValues>(Table, parameters,std::nullopt);
    EXPECT_EQ(maxInsertRows, result);
}

TEST_F(ServicePostgresql, RemoveRow)
{
    int result;
    DB::HashedDatabaseValues values;
    values = {{"Name", NameApp}};
    result = database->RemoveRowFromTable<DB::HashedDatabaseValues>(Table, values,std::nullopt);
    EXPECT_EQ(0, result);
}

TEST_F(ServicePostgresql, CleanTable)
{
    int result;
    database->CleanTable(Table);
    result = database->CountRowsInTable<int,DB::HashedDatabaseValues>(Table,std::nullopt,std::nullopt);
    std::cout << result << std::endl;
    EXPECT_EQ(0, result);
    
    database->CleanTable("WindowsVersions");
    result = database->CountRowsInTable<int,DB::HashedDatabaseValues>("WindowsVersions",std::nullopt,std::nullopt);
    EXPECT_EQ(0, result);
}