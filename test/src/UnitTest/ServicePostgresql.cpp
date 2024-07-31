#include <UnitTest/ServicePostgresql.hpp>

TEST_F(ServicePostgresql, DeleteTable)
{
    int result;
    result = database->DeleteTable(Table);
    EXPECT_EQ(0, result);
}

TEST_F(ServicePostgresql, CreateTable)
{
    int result;
    DB::HashedDatabaseValues columns;
    columns = {{"Name", "TEXT"}, {"Windows", "TEXT"}, {"Linux", "TEXT"}, {"macOS", "TEXT"}};
    result = database->CreateTable<DB::HashedDatabaseValues>(Table, columns);
    EXPECT_EQ(result, 0);
}

TEST_F(ServicePostgresql, InsertValue)
{
    int result;
    DB::HashedDatabaseValues values;
    values = {{"Name", NameApp}, {"Windows", Windows_Command}, {"Linux", Linux_Command}, {"macOS", macOS_Command}};
    result = database->InsertRowToTable<DB::HashedDatabaseValues>(Table, values);
    EXPECT_EQ(result, 0);
}

TEST_F(ServicePostgresql, GetValue)
{
    DB::HashedDatabaseValues parameters;
    std::vector<DB::HashedDatabaseValues> output;
    bool find = false;
    parameters = {{"name", NameApp}};
    output = database->GetValueFromRow<DB::HashedDatabaseValues>(Table, "windows", parameters, std::nullopt);
    for(auto& data : output)
    {
        if (data["windows"] == Windows_Command)
        {
            find = true;
        }
    }
    EXPECT_TRUE(find);
}

TEST_F(ServicePostgresql, GetColumns)
{
    std::vector<DB::HashedDatabaseValues> db_rows;
    DB::HashedDatabaseValues parameters;
    std::vector<std::string> columns = {"id","name"};
    bool find;
    parameters = {{"windows", Windows_Command}};
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

// TEST_F(ServicePostgresql, GetOneColumn)
// {
//     DB::HashedDatabaseValues parameters;
//     DB::HashedEnumColDatabaseValues db_rows;
//     parameters = {{"Name", NameApp}};
//     db_rows = database.GetOneColumnFromTable(Table, "Windows", parameters, std::nullopt);
//     EXPECT_STREQ(Windows_Command.c_str(), db_rows[0].c_str());
// }

// TEST_F(ServicePostgresql, GetArrayOneColumn)
// {
//     DB::HashedDatabaseValues parameters;
//     DB::ArrayDatabaseValues db_rows;
//     parameters = {{"Name", NameApp}};
//     db_rows = database.GetArrayOneColumnFromTable(Table, "Windows", parameters, std::nullopt);
//     EXPECT_STREQ(Windows_Command.c_str(), db_rows[0].c_str());
// }

// TEST_F(ServicePostgresql, GetMaxValue)
// {
//     DB::HashedDatabaseValues parameters;
//     std::string result;
//     parameters = {{"Channel", "stable"}, {"Architecture", "amd64"}};
//     result = database.GetMaxValueFromTable("WindowsVersions", "Version", parameters);
//     // std::unordered_map<int, DB::HashedDatabaseValues> db_rows;
//     // DB::HashedDatabaseValues parameters;
//     // parameters  = {{"Name", NameApp}};
//     // db_rows = database.GetRowFromTable(Table, parameters);
//     EXPECT_STREQ("0.2", result.c_str());
// }

// TEST_F(ServicePostgresql, GetMaxRow)
// {
//     DB::HashedDatabaseValues parameters;
//     DB::HashedDatabaseValues db_rows;
//     parameters = {{"Channel", "stable"}, {"Architecture", "amd64"}};
//     db_rows = database.GetMaxRowFromTable("WindowsVersions", "Version", parameters);
//     EXPECT_STREQ(db_rows["Version"].c_str(), "0.2");
// }

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

// TEST_F(ServicePostgresql, UpdateValues)
// {
//     int result;
//     bool expression;
//     DB::HashedDatabaseValues db_values;
//     DB::HashedDatabaseValues values;
//     DB::HashedDatabaseValues parameters;
//     values = {
//         {"Windows", "Updated_Test_Windows_Command"},
//         {"Linux", "Updated_Test_Linux_Command"},
//         {"macOS", "Updated_Test_macOS_Command"}};
//     parameters = {{"Windows", Windows_Command}, {"Linux", Linux_Command}, {"macOS", macOS_Command}};
//     result = database.UpdateRowInTable(Table, values, parameters);
//     db_values = database.GetRowByID(Table, 1);
//     expression = result == 0 && values["Windows"] == db_values["Windows"] && values["Linux"] == db_values["Linux"] && values["macOS"] == db_values["macOS"];
//     EXPECT_TRUE(expression);
// }

// TEST_F(ServicePostgresql, RemoveRow)
// {
//     int result;
//     DB::HashedDatabaseValues values;
//     values = {{"Name", NameApp}};
//     result = database.RemoveRowFromTable(Table, values);
//     EXPECT_EQ(0, result);

//     values = {{"Language", "PythonDevelopmentTools"}};
//     result = database.RemoveRowFromTable(DevelopmentTable, values);
//     EXPECT_EQ(0, result);
// }

TEST_F(ServicePostgresql, CleanTable)
{
    int result;
    result = database->CleanTable(Table);
    EXPECT_EQ(0, result);
}

// int main(int argc, char **argv)
// {
//     std::filesystem::path current_dir = argv[0];
//     std::filesystem::current_path(current_dir.parent_path().generic_string());
//     ::testing::InitGoogleTest(&argc, argv);
//     return RUN_ALL_TESTS();
// }