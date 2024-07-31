#include <UnitTest/SQLite.hpp>

TEST_F(SQLite, DeleteAllRows)
{
    bool expression;
    expression = database.DeleteAllRows(Table) == 0 && database.DeleteAllRows(DevelopmentTable) == 0 && VersionsDatabase.DeleteAllRows("WindowsVersions") == 0;
    EXPECT_TRUE(expression);
}
TEST_F(SQLite, CreateTable)
{
    int result;
    DB::HashedDatabaseValues columns;
    columns = {{"Name", "TEXT"}, {"Windows", "TEXT"}, {"Linux", "TEXT"}, {"macOS", "TEXT"}};
    result = database.CreateTable(Table, columns);
    EXPECT_EQ(result, 0);

    columns = {{"Language", "TEXT"}};
    result = database.CreateTable(DevelopmentTable, columns);
    EXPECT_EQ(result, 0);

    columns = {{"Version", "INTEGER"}, {"Channel", "TEXT"}, {"Architecture", "TEXT"}, {"Url", "TEXT"}, {"Url_arm64", "TEXT"}};
    result = VersionsDatabase.CreateTable("WindowsVersions", columns);
    EXPECT_EQ(result, 0);
}

TEST_F(SQLite, InsertValue)
{
    int result;
    DB::HashedDatabaseValues values;
    values = {{"Name", NameApp}, {"Windows", Windows_Command}, {"Linux", Linux_Command}, {"macOS", macOS_Command}};
    result = database.InsertRowToTable(Table, values);
    EXPECT_EQ(result, 0);

    values = {{"Language", "PythonDevelopmentTools"}};
    result = database.InsertRowToTable(DevelopmentTable, values);
    EXPECT_EQ(result, 0);

    values = {{"Version", "0.1"}, {"Channel", "stable"}, {"Architecture", "amd64"}, {"Url", "https://github.com/DeepForge-Tech/DeepForge-Toolset/releases/download/v0.1_linux_amd64/DeepForge-Toolset_0.1_linux_amd64.zip"}, {"Url_arm64", "Empty"}};
    result = VersionsDatabase.InsertRowToTable("WindowsVersions", values);
    EXPECT_EQ(result, 0);

    values = {{"Version", "0.2"}, {"Channel", "stable"}, {"Architecture", "amd64"}, {"Url", "https://github.com/DeepForge-Tech/DeepForge-Toolset/releases/download/v0.2_linux_amd64/DeepForge-Toolset_0.2_linux_amd64.zip"}, {"Url_arm64", "Empty"}};
    result = VersionsDatabase.InsertRowToTable("WindowsVersions", values);
    EXPECT_EQ(result, 0);
}

TEST_F(SQLite, GetValue)
{
    DB::HashedDatabaseValues parameters;
    std::string result;
    parameters = {{"Name", NameApp}};
    result = database.GetValueFromRow(Table, "Windows", parameters, std::nullopt);
    EXPECT_STREQ(Windows_Command.c_str(), result.c_str());
}

TEST_F(SQLite, GetTwoColumns)
{
    DB::HashedDatabaseValues db_rows;
    // parameters = {{"Windows", NameApp}};
    db_rows = database.GetTwoColumnsFromTable(Table, "Name", "Windows", std::nullopt, std::nullopt);
    EXPECT_STREQ(Windows_Command.c_str(), db_rows[NameApp].c_str());
}

TEST_F(SQLite, GetOneColumn)
{
    DB::HashedDatabaseValues parameters;
    DB::HashedEnumColDatabaseValues db_rows;
    parameters = {{"Name", NameApp}};
    db_rows = database.GetOneColumnFromTable(Table, "Windows", parameters, std::nullopt);
    EXPECT_STREQ(Windows_Command.c_str(), db_rows[0].c_str());
}

TEST_F(SQLite, GetArrayOneColumn)
{
    DB::HashedDatabaseValues parameters;
    DB::ArrayDatabaseValues db_rows;
    parameters = {{"Name", NameApp}};
    db_rows = database.GetArrayOneColumnFromTable(Table, "Windows", parameters, std::nullopt);
    EXPECT_STREQ(Windows_Command.c_str(), db_rows[0].c_str());
}

TEST_F(SQLite, GetMaxValue)
{
    DB::HashedDatabaseValues parameters;
    std::string result;
    parameters = {{"Channel", "stable"}, {"Architecture", "amd64"}};
    result = VersionsDatabase.GetMaxValueFromTable("WindowsVersions", "Version", parameters);
    // std::unordered_map<int, DB::HashedDatabaseValues> db_rows;
    // DB::HashedDatabaseValues parameters;
    // parameters  = {{"Name", NameApp}};
    // db_rows = database.GetRowFromTable(Table, parameters);
    EXPECT_STREQ("0.2", result.c_str());
}

TEST_F(SQLite, GetMaxRow)
{
    DB::HashedDatabaseValues parameters;
    DB::HashedDatabaseValues db_rows;
    parameters = {{"Channel", "stable"}, {"Architecture", "amd64"}};
    db_rows = VersionsDatabase.GetMaxRowFromTable("WindowsVersions", "Version", parameters);
    EXPECT_STREQ(db_rows["Version"].c_str(), "0.2");
}

TEST_F(SQLite, GetRow)
{
    DB::HashedEnumDatabaseValues db_rows;
    DB::HashedDatabaseValues parameters;
    parameters = {{"Name", NameApp}};
    db_rows = database.GetRowFromTable(Table, parameters, std::nullopt);
    EXPECT_STREQ(Windows_Command.c_str(), db_rows[0]["Windows"].c_str());
}

TEST_F(SQLite, GetRowByID)
{
    DB::HashedDatabaseValues db_values;
    bool expression;
    db_values = database.GetRowByID(Table, 1);
    expression = Windows_Command == db_values["Windows"] && Linux_Command == db_values["Linux"] && macOS_Command == db_values["macOS"];
    EXPECT_TRUE(expression);
}

TEST_F(SQLite, GetAllRows)
{
    DB::HashedEnumDatabaseValues db_rows;
    bool expression;
    db_rows = database.GetAllRowsFromTable(Table);
    expression = Windows_Command == db_rows[0]["Windows"];
    EXPECT_TRUE(expression);
}

TEST_F(SQLite, ExecuteQuery)
{
    DB::HashedEnumDatabaseValues db_rows;
    bool expression;
    db_rows = database.ExecuteQuery("SELECT * FROM " + Table);
    expression = Windows_Command == db_rows[0]["Windows"];
    EXPECT_TRUE(expression);
}

TEST_F(SQLite, RunQuery)
{
    int result;
    bool expression;
    DB::HashedDatabaseValues db_values;
    DB::HashedDatabaseValues values;
    values = {
        {"Windows", "Updated_Test_Windows_Command"},
        {"Linux", "Updated_Test_Linux_Command"},
        {"macOS", "Updated_Test_macOS_Command"}};
    result = database.RunQuery("UPDATE Test SET Windows='Updated_Test_Windows_Command',macOS='Updated_Test_macOS_Command',Linux='Updated_Test_Linux_Command' WHERE Windows='Test_Windows_Command' AND macOS='Test_macOS_Command' AND Linux='Test_Linux_Command';");
    db_values = database.GetRowByID(Table, 1);
    expression = result == 0 && values["Windows"] == db_values["Windows"] && values["Linux"] == db_values["Linux"] && values["macOS"] == db_values["macOS"];
    EXPECT_TRUE(expression);
}

TEST_F(SQLite, UpdateValues)
{
    int result;
    bool expression;
    DB::HashedDatabaseValues db_values;
    DB::HashedDatabaseValues values;
    DB::HashedDatabaseValues parameters;
    values = {
        {"Windows", "Updated_Test_Windows_Command"},
        {"Linux", "Updated_Test_Linux_Command"},
        {"macOS", "Updated_Test_macOS_Command"}};
    parameters = {{"Windows", Windows_Command}, {"Linux", Linux_Command}, {"macOS", macOS_Command}};
    result = database.UpdateRowInTable(Table, values, parameters);
    db_values = database.GetRowByID(Table, 1);
    expression = result == 0 && values["Windows"] == db_values["Windows"] && values["Linux"] == db_values["Linux"] && values["macOS"] == db_values["macOS"];
    EXPECT_TRUE(expression);
}

TEST_F(SQLite, RemoveRow)
{
    int result;
    DB::HashedDatabaseValues values;
    values = {{"Name", NameApp}};
    result = database.RemoveRowFromTable(Table, values);
    EXPECT_EQ(0, result);

    values = {{"Language", "PythonDevelopmentTools"}};
    result = database.RemoveRowFromTable(DevelopmentTable, values);
    EXPECT_EQ(0, result);
}