#include <DatabaseAPI/SQLiteDB.hpp>

int DB::SQLiteDB::CreateTable(const std::string &NameTable, DB::HashedDatabaseValues Columns)
{
    try
    {
        std::string SQL_QUERY;
        SQL_QUERY = fmt::format("CREATE TABLE IF NOT EXISTS {} (id INTEGER PRIMARY KEY AUTOINCREMENT NOT NULL UNIQUE,\n", NameTable);
        for (int i = 1; const auto &element : Columns)
        {
            SQL_QUERY = SQL_QUERY + element.first + " " + to_upper(element.second);
            if (i != Columns.size())
            {
                SQL_QUERY += ",";
            }
            i++;
        }
        SQL_QUERY += ")";
        int RESULT_SQL = sqlite3_exec(db, SQL_QUERY.c_str(), callback, NULL, NULL);
        if (RESULT_SQL != SQLITE_OK)
        {
            std::string errorMessage = sqlite3_errmsg(db);
            throw std::runtime_error(fmt::format("Error in execution {} command: {}", SQL_QUERY, errorMessage));
        }
        return 0;
    }
    catch (std::exception &error)
    {
        throw std::runtime_error(fmt::format("DatabaseAPI.SQLiteDB.CreateTable.{}", error.what()));
    }
}
int DB::SQLiteDB::InsertRowToTable(const std::string &NameTable, DB::HashedDatabaseValues Fields)
{
    try
    {
        std::string SQL_QUERY;
        SQL_QUERY = fmt::format("INSERT INTO {} ", NameTable);
        std::string Columns = "(";
        std::string Values = "(";
        for (int i = 1; const auto &element : Fields)
        {
            Columns += "\"" + element.first + "\"";
            Values += "\"" + element.second + "\"";
            if (i != Fields.size())
            {
                Columns += ", ";
                Values += ", ";
            }
            i++;
        }
        Columns += ")";
        Values += ")";
        SQL_QUERY += Columns + " VALUES " + Values + ";";
        int RESULT_SQL = sqlite3_exec(db, SQL_QUERY.c_str(), callback, NULL, NULL);
        if (RESULT_SQL != SQLITE_OK)
        {
            std::string errorMessage = sqlite3_errmsg(db);
            throw std::runtime_error(fmt::format("Error in execution {} command: {}", SQL_QUERY, errorMessage));
        }
        return 0;
    }
    catch (std::exception &error)
    {
        throw std::runtime_error(fmt::format("DatabaseAPI.SQLiteDB.InsertRowToTable.{}", error.what()));
    }
}

bool DB::SQLiteDB::ExistTableInDB(const std::string &NameTable)
{
    try
    {
        std::string SQL_QUERY;
        int countTables;
        // Create SQL statement
        SQL_QUERY = fmt::format("SELECT count(*) FROM sqlite_master WHERE type='table' AND name='{}';", NameTable);
        // SQL_QUERY = "SELECT count(Windows) FROM Applications";
        // Execute SQL statement
        int RESULT_SQL = sqlite3_prepare_v2(db, SQL_QUERY.c_str(), SQL_QUERY.length(), &statement, nullptr);
        // if result of execute sql statement != SQLITE_OK, that send error
        if (RESULT_SQL != SQLITE_OK)
        {
            std::string errorMessage = sqlite3_errmsg(db);
            throw std::runtime_error(fmt::format("Error in execution {} command: ", SQL_QUERY, errorMessage));
        }
        // Loop through the results, a row at a time.
        while ((RESULT_SQL = sqlite3_step(statement)) == SQLITE_ROW)
        {
            // printf("%s\n", sqlite3_column_text(statement, 0));
            countTables = sqlite3_column_int(statement, 0);
        }
        // Free the statement when done.
        sqlite3_finalize(statement);
        return countTables >= 1;
    }
    catch (std::exception &error)
    {
        throw std::runtime_error(fmt::format("DatabaseAPI.SQLiteDB.ExistTableInDB.{}", error.what()));
    }
}

bool DB::SQLiteDB::ExistRowInTable(const std::string &NameTable, const std::string &NameColumn, const std::string &Value)
{
    try
    {
        std::string SQL_QUERY;
        // Create SQL statement
        SQL_QUERY = fmt::format("SELECT * FROM {} WHERE {}='{}'", NameTable, NameColumn, Value);
        // Execute SQL statement
        int RESULT_SQL = sqlite3_prepare_v2(db, SQL_QUERY.c_str(), SQL_QUERY.length(), &statement, nullptr);
        // if result of execute sql statement != SQLITE_OK, that send error
        if (sqlite3_step(statement) == SQLITE_ROW)
        {
            sqlite3_finalize(statement);
            return true;
        }
        // Free the statement when done.
        sqlite3_finalize(statement);
        return false;
    }
    catch (std::exception &error)
    {
        throw std::runtime_error(fmt::format("DatabaseAPI.SQLiteDB.ExistRowInTable.{}", error.what()));
    }
}

std::string DB::SQLiteDB::GetValueFromRow(const std::string &NameTable, const std::string &NameColumn, const std::optional<HashedDatabaseValues> &Parameters, const std::optional<HashedDatabaseValues> &Exceptions)
{
    try
    {
        std::string AnswerDB;
        std::string SQL_QUERY;
        // HashedDatabaseValues values;

        // Create SQL statement
        SQL_QUERY = fmt::format("SELECT {} FROM {}", NameColumn, NameTable);

        Parameters.has_value() ? AddParameters(SQL_QUERY, Parameters.value(), 1) : throw std::logic_error("Parameters is empty");

        if (Exceptions.has_value())
            AddExceptions(SQL_QUERY, Exceptions.value(), 1);

        SQL_QUERY += ";";
        // Execute SQL statement
        int RESULT_SQL = sqlite3_prepare_v2(db, SQL_QUERY.c_str(), SQL_QUERY.length(), &statement, nullptr);
        // if result of execute sql statement != SQLITE_OK, that send error
        if (RESULT_SQL != SQLITE_OK)
        {
            std::string errorMessage = sqlite3_errmsg(db);
            throw std::runtime_error(fmt::format("Error in execution {} command: {}", SQL_QUERY, errorMessage));
        }
        // Loop through the results, a row at a time.
        while ((RESULT_SQL = sqlite3_step(statement)) == SQLITE_ROW)
        {
            // printf("%s\n", sqlite3_column_text(statement, 0));
            AnswerDB = (std::string(reinterpret_cast<const char *>(
                sqlite3_column_text(statement, 0))));
        }
        // Free the statement when done.
        sqlite3_finalize(statement);
        return AnswerDB;
    }
    catch (std::exception &error)
    {
        throw std::runtime_error(fmt::format("DatabaseAPI.SQLiteDB.GetValueFromRow.{}", error.what()));
    }
}

DB::HashedDatabaseValues DB::SQLiteDB::GetRowByID(const std::string &NameTable, const int &id)
{
    try
    {
        int num_columns;
        int RESULT_SQL;
        std::string Key;
        std::string Value;
        std::string SQL_QUERY;
        DB::HashedDatabaseValues WriteMap;

        // Create SQL statement
        SQL_QUERY = fmt::format("SELECT * FROM {} WHERE id='{}'", NameTable, std::to_string(id));
        // Execute SQL statement
        RESULT_SQL = sqlite3_prepare_v2(db, SQL_QUERY.c_str(), SQL_QUERY.length(), &statement, nullptr);
        // if result of execute sql statement != SQLITE_OK, that send error
        if (RESULT_SQL != SQLITE_OK)
        {
            std::string errorMessage = sqlite3_errmsg(db);
            throw std::runtime_error("Error in execution SELECT * FROM command: " + errorMessage);
        }
        // Execute the SQL statement
        RESULT_SQL = sqlite3_step(statement);
        if (RESULT_SQL != SQLITE_ROW)
        {
            throw std::runtime_error("No rows returned");
        }

        // Fetch all rows from the result set
        num_columns = sqlite3_column_count(statement);
        while (RESULT_SQL == SQLITE_ROW)
        {
            for (int i = 0; i < num_columns; i++)
            {
                Key = sqlite3_column_name(statement, i);
                Value = reinterpret_cast<const char *>(sqlite3_column_text(statement, i));
                WriteMap.insert(std::pair<std::string, std::string>(Key, Value));
            }
            RESULT_SQL = sqlite3_step(statement);
        }
        // Free the statement when done.
        sqlite3_finalize(statement);
        // return Names,Commands;
        return WriteMap;
    }
    catch (std::exception &error)
    {
        throw std::runtime_error(fmt::format("DatabaseAPI.SQLiteDB.GetRowByID.{}", error.what()));
    }
}

DB::HashedEnumDatabaseValues DB::SQLiteDB::GetRowFromTable(const std::string &NameTable, const std::optional<HashedDatabaseValues> &Parameters, const std::optional<HashedDatabaseValues> &Exceptions)
{
    try
    {
        int n = 0;
        int RESULT_SQL;
        int num_columns;
        std::string SQL_QUERY;
        std::string id;
        std::string Key;
        std::string Value;
        HashedDatabaseValues values;
        HashedEnumDatabaseValues WriteMap;

        // Create SQL statement
        SQL_QUERY = fmt::format("SELECT * FROM {}", NameTable);

        if (Parameters.has_value())
            AddParameters(SQL_QUERY, Parameters.value(), 1);

        if (Exceptions.has_value())
            AddExceptions(SQL_QUERY, Exceptions.value(), 1);

        SQL_QUERY += ";";
        RESULT_SQL = sqlite3_prepare_v2(db, SQL_QUERY.c_str(), SQL_QUERY.length(), &statement, nullptr);

        if (RESULT_SQL != SQLITE_OK)
        {
            std::string errorMessage = sqlite3_errmsg(db);
            throw std::runtime_error(fmt::format("Error in execution {} command: ", SQL_QUERY, errorMessage));
        }
        num_columns = sqlite3_column_count(statement);
        while ((RESULT_SQL = sqlite3_step(statement)) == SQLITE_ROW)
        {
            id = (const char *)sqlite3_column_text(statement, 0);
            DB::HashedDatabaseValues row;
            row.insert(std::pair<std::string, std::string>("id", id));
            for (int i = 1; i < num_columns; i++)
            {
                Key = sqlite3_column_name(statement, i);
                Value = (const char *)sqlite3_column_text(statement, i);
                row.insert(std::pair<std::string, std::string>(Key, Value));
            }
            WriteMap.insert(std::pair<int, DB::HashedDatabaseValues>(n, row));
            n++;
        }
        /*example returned WriteMap:
        {0, {
                {"id",1},{"Name","VSCode"},{"Windows","winget install vscode"...}
            }
        },
        {1, {
                {"id",2},{"Name","CMake"},{"Windows","winget install cmake"...}
            }
        }
        */
        sqlite3_finalize(statement);
        return WriteMap;
    }
    catch (std::exception &error)
    {
        throw std::runtime_error(fmt::format("DatabaseAPI.SQLiteDB.GetRowFromTable.{}", error.what()));
    }
}

DB::HashedDatabaseValues DB::SQLiteDB::GetTwoColumnsFromTable(const std::string &NameTable, const std::string &FirstColumn, const std::string &SecondColumn, const std::optional<HashedDatabaseValues> &Parameters, const std::optional<HashedDatabaseValues> &Exceptions)
{
    try
    {
        int num_columns;
        int RESULT_SQL;
        std::string Key;
        std::string Value;
        std::string SQL_QUERY;
        DB::HashedDatabaseValues WriteMap;

        // Create SQL statement
        SQL_QUERY = fmt::format("SELECT {}, {} FROM {}", FirstColumn, SecondColumn, NameTable);

        if (Parameters.has_value())
            AddParameters(SQL_QUERY, Parameters.value(), 1);

        if (Exceptions.has_value())
            AddExceptions(SQL_QUERY, Exceptions.value(), 1);

        SQL_QUERY += ";";
        // Execute SQL statement
        RESULT_SQL = sqlite3_prepare_v2(db, SQL_QUERY.c_str(), SQL_QUERY.length(), &statement, nullptr);
        // if result of execute sql statement != SQLITE_OK, that send error
        if (RESULT_SQL != SQLITE_OK)
        {
            std::string errorMessage = sqlite3_errmsg(db);
            throw std::runtime_error(fmt::format("Error in execution {} command: {}", SQL_QUERY, errorMessage));
        }
        // Execute the SQL statement
        RESULT_SQL = sqlite3_step(statement);
        if (RESULT_SQL != SQLITE_ROW)
        {
            throw std::runtime_error("No rows returned");
        }

        // Fetch all rows from the result set
        num_columns = sqlite3_column_count(statement);
        while (RESULT_SQL == SQLITE_ROW)
        {
            for (int i = 0; i < num_columns; i++)
            {
                Key = (const char *)sqlite3_column_text(statement, 0);
                Value = (const char *)sqlite3_column_text(statement, 1);
                WriteMap.insert(std::pair<std::string, std::string>(Key, Value));
            }
            RESULT_SQL = sqlite3_step(statement);
        }
        // Free the statement when done.
        sqlite3_finalize(statement);
        // return Names,Commands;
        return WriteMap;
    }
    catch (std::exception &error)
    {
        throw std::runtime_error(fmt::format("DatabaseAPI.SQLiteDB.GetTwoColumnsFromTable.{}", error.what()));
    }
}

DB::HashedEnumColDatabaseValues DB::SQLiteDB::GetOneColumnFromTable(const std::string &NameTable, const std::string &NameColumn, const std::optional<HashedDatabaseValues> &Parameters, const std::optional<HashedDatabaseValues> &Exceptions)
{
    try
    {
        // int num_columns;
        int RESULT_SQL;
        HashedEnumColDatabaseValues WriteMap;
        std::string Value;
        std::string SQL_QUERY;

        // Create SQL statement
        SQL_QUERY = fmt::format("SELECT {} FROM {}", NameColumn, NameTable);

        if (Parameters.has_value())
            AddParameters(SQL_QUERY, Parameters.value(), 1);

        if (Exceptions.has_value())
            AddExceptions(SQL_QUERY, Exceptions.value(), 1);

        SQL_QUERY += ";";
        // Execute SQL statement
        RESULT_SQL = sqlite3_prepare_v2(db, SQL_QUERY.c_str(), SQL_QUERY.length(), &statement, nullptr);
        // if result of execute sql statement != SQLITE_OK, that send error
        if (RESULT_SQL != SQLITE_OK)
        {
            std::string errorMessage = sqlite3_errmsg(db);
            throw std::runtime_error(fmt::format("Error in execution {} command: {}", SQL_QUERY, errorMessage));
        }

        // Fetch all rows from the result set
        // num_columns = sqlite3_column_count(statement);
        int i = 0;
        while (sqlite3_step(statement) == SQLITE_ROW)
        {
            Value = (const char *)sqlite3_column_text(statement, 0);
            WriteMap.insert(std::pair<int, std::string>(i, Value));
            i++;
        }
        // Free the statement when done.
        sqlite3_finalize(statement);
        return WriteMap;
    }
    catch (std::exception &error)
    {
        throw std::runtime_error(fmt::format("DatabaseAPI.SQLiteDB.GetOneColumnFromTable.{}", error.what()));
    }
}

DB::ArrayDatabaseValues DB::SQLiteDB::GetArrayOneColumnFromTable(const std::string &NameTable, const std::string &NameColumn, const std::optional<HashedDatabaseValues> &Parameters, const std::optional<HashedDatabaseValues> &Exceptions)
{
    try
    {
        // int num_columns;
        int RESULT_SQL;

        std::string Value;
        std::string SQL_QUERY;
        ArrayDatabaseValues OutputArray(new std::string[GetArraySize(NameTable, NameColumn)]);

        // Create SQL statement
        SQL_QUERY = fmt::format("SELECT {} FROM {}", NameColumn, NameTable);

        if (Parameters.has_value())
            AddParameters(SQL_QUERY, Parameters.value(), 1);

        if (Exceptions.has_value())
            AddExceptions(SQL_QUERY, Exceptions.value(), 1);

        SQL_QUERY += ";";
        // Execute SQL statement
        RESULT_SQL = sqlite3_prepare_v2(db, SQL_QUERY.c_str(), SQL_QUERY.length(), &statement, nullptr);
        // if result of execute sql statement != SQLITE_OK, that send error
        if (RESULT_SQL != SQLITE_OK)
        {
            std::string errorMessage = sqlite3_errmsg(db);
            throw std::runtime_error(fmt::format("Error in execution {} command: {}", SQL_QUERY, errorMessage));
        }

        // Fetch all rows from the result set
        // num_columns = sqlite3_column_count(statement);
        int i = 0;
        while (sqlite3_step(statement) == SQLITE_ROW)
        {
            Value = (const char *)sqlite3_column_text(statement, 0);
            OutputArray[i] = Value;
            i++;
        }
        // Free the statement when done.
        sqlite3_finalize(statement);
        return OutputArray;
    }
    catch (std::exception &error)
    {
        throw std::runtime_error(fmt::format("DatabaseAPI.SQLiteDB.GetArrayOneColumnFromTable.{}", error.what()));
    }
}

DB::HashedEnumDatabaseValues DB::SQLiteDB::GetAllRowsFromTable(const std::string &NameTable)
{
    try
    {
        HashedEnumDatabaseValues WriteMap;
        int n = 0;
        int RESULT_SQL;
        int num_columns;
        std::string Key;
        std::string Value;
        std::string SQL_QUERY;
        std::string id;

        SQL_QUERY = fmt::format("SELECT * FROM {}", NameTable);
        RESULT_SQL = sqlite3_prepare_v2(db, SQL_QUERY.c_str(), SQL_QUERY.length(), &statement, nullptr);

        if (RESULT_SQL != SQLITE_OK)
        {
            std::string errorMessage = sqlite3_errmsg(db);
            throw std::runtime_error(fmt::format("Error in execution {} command: {}", SQL_QUERY, errorMessage));
        }
        num_columns = sqlite3_column_count(statement);
        while ((RESULT_SQL = sqlite3_step(statement)) == SQLITE_ROW)
        {
            id = (const char *)sqlite3_column_text(statement, 0);
            HashedDatabaseValues row;
            for (int i = 1; i < num_columns; i++)
            {
                Key = sqlite3_column_name(statement, i);
                Value = (const char *)sqlite3_column_text(statement, i);
                row.insert(std::pair<std::string, std::string>("id", id));
                row.insert(std::pair<std::string, std::string>(Key, Value));
            }
            WriteMap.insert(std::pair<int, DB::HashedDatabaseValues>(n, row));
            n++;
        }
        /*example returned WriteMap:
        {0, {
                {"id",1},{"Name","VSCode"},{"Windows","winget install vscode"...}
            }
        },
        {1, {
                {"id",2},{"Name","CMake"},{"Windows","winget install cmake"...}
            }
        }
        */
        sqlite3_finalize(statement);
        return WriteMap;
    }
    catch (std::exception &error)
    {
        throw std::runtime_error(fmt::format("DatabaseAPI.SQLiteDB.GetAllRowsFromTable.{}", error.what()));
    }
}

DB::HashedDatabaseValues DB::SQLiteDB::GetMaxRowFromTable(const std::string &NameTable, const std::string &NameColumn, const std::optional<HashedDatabaseValues> &Parameters)
{
    try
    {
        int num_columns;
        int RESULT_SQL;
        int tableCount = GetArraySize(NameTable, NameColumn);
        std::string Key;
        std::string Value;
        std::string SQL_QUERY;
        std::string id;
        DB::HashedDatabaseValues WriteMap;

        // Create SQL statement
        SQL_QUERY = fmt::format("SELECT * FROM {} WHERE {} =(SELECT max({}) FROM {}", NameTable, NameColumn, NameColumn, NameTable);

        if (Parameters.has_value())
            AddParameters(SQL_QUERY, Parameters.value(), 2);

        SQL_QUERY += ");"; // add closing parenthesis
        // Execute SQL statement
        RESULT_SQL = sqlite3_prepare_v2(db, SQL_QUERY.c_str(), SQL_QUERY.length(), &statement, nullptr);
        // if result of execute sql statement != SQLITE_OK, that send error
        if (RESULT_SQL != SQLITE_OK)
        {
            std::string errorMessage = sqlite3_errmsg(db);
            throw std::runtime_error(fmt::format("Error in execution {} command: {}", SQL_QUERY, errorMessage));
        }
        // Fetch all rows from the result set
        num_columns = sqlite3_column_count(statement);
        while ((RESULT_SQL = sqlite3_step(statement)) == SQLITE_ROW)
        {
            id = (const char *)sqlite3_column_text(statement, 0);
            for (int i = 1; i < num_columns; i++)
            {
                Key = sqlite3_column_name(statement, i);
                Value = (const char *)sqlite3_column_text(statement, i);
                WriteMap.insert(std::pair<std::string, std::string>("id", id));
                WriteMap.insert(std::pair<std::string, std::string>(Key, Value));
            }
        }
        // Free the statement when done.
        sqlite3_finalize(statement);
        return WriteMap;
    }
    catch (std::exception &error)
    {
        throw std::runtime_error(fmt::format("DatabaseAPI.SQLiteDB.GetMaxRowFromTable.{}", error.what()));
    }
}

std::string DB::SQLiteDB::GetMaxValueFromTable(const std::string &NameTable, const std::string &NameColumn, const std::optional<HashedDatabaseValues> &Parameters)
{
    try
    {
        std::string AnswerDB;
        std::string SQL_QUERY;

        // Create SQL statement
        SQL_QUERY = fmt::format("SELECT max({}) FROM {}", NameColumn, NameTable);

        Parameters.has_value() ? AddParameters(SQL_QUERY, Parameters.value(), 1) : throw std::logic_error("Parameters is empty");

        SQL_QUERY += ";";
        // Execute SQL statement
        int RESULT_SQL = sqlite3_prepare_v2(db, SQL_QUERY.c_str(), SQL_QUERY.length(), &statement, nullptr);
        // if result of execute sql statement != SQLITE_OK, that send error
        if (RESULT_SQL != SQLITE_OK)
        {
            std::string errorMessage = sqlite3_errmsg(db);
            throw std::runtime_error(fmt::format("Error in execution {} command: {}", SQL_QUERY, errorMessage));
        }
        // Loop through the results, a row at a time.
        while ((RESULT_SQL = sqlite3_step(statement)) == SQLITE_ROW)
        {
            // printf("%s\n", sqlite3_column_text(statement, 0));
            AnswerDB = (std::string(reinterpret_cast<const char *>(
                sqlite3_column_text(statement, 0))));
        }
        // Free the statement when done.
        sqlite3_finalize(statement);
        return AnswerDB;
    }
    catch (std::exception &error)
    {
        throw std::runtime_error(fmt::format("DatabaseAPI.SQLiteDB.GetMaxValueFromTable.{}", error.what()));
    }
}

int DB::SQLiteDB::GetArraySize(const std::string &NameTable, const std::string &NameColumn)
{
    try
    {
        int ArraySize;
        std::string SQL_QUERY;
        // Create SQL statement
        SQL_QUERY = fmt::format("SELECT count({}) FROM {}", NameColumn, NameTable);
        // SQL_QUERY = "SELECT count(Windows) FROM Applications";
        // Execute SQL statement
        int RESULT_SQL = sqlite3_prepare_v2(db, SQL_QUERY.c_str(), SQL_QUERY.length(), &statement, nullptr);
        // if result of execute sql statement != SQLITE_OK, that send error
        if (RESULT_SQL != SQLITE_OK)
        {
            std::string errorMessage = sqlite3_errmsg(db);
            throw std::runtime_error(fmt::format("Error in execution {} command: {}", SQL_QUERY, errorMessage));
        }
        // Loop through the results, a row at a time.
        while ((RESULT_SQL = sqlite3_step(statement)) == SQLITE_ROW)
        {
            // printf("%s\n", sqlite3_column_text(statement, 0));
            ArraySize = sqlite3_column_int(statement, 0);
        }
        // Free the statement when done.
        sqlite3_finalize(statement);
        return ArraySize;
    }
    catch (std::exception &error)
    {
        throw std::runtime_error(fmt::format("DatabaseAPI.SQLiteDB.GetArraySize.{}", error.what()));
    }
}

int DB::SQLiteDB::RemoveRowFromTable(const std::string &NameTable, const std::optional<HashedDatabaseValues> &Parameters)
{
    try
    {
        std::string SQL_QUERY;

        SQL_QUERY = fmt::format("DELETE FROM {}", NameTable);

        Parameters.has_value() ? AddParameters(SQL_QUERY, Parameters.value(), 1) : throw std::logic_error("Parameters is empty");

        SQL_QUERY += ";";

        int RESULT_SQL = sqlite3_exec(db, SQL_QUERY.c_str(), callback, NULL, NULL);
        if (RESULT_SQL != SQLITE_OK)
        {
            std::string errorMessage = sqlite3_errmsg(db);
            throw std::runtime_error(fmt::format("Error in execution {} command: {}", SQL_QUERY, errorMessage));
        }
        return 0;
    }
    catch (std::exception &error)
    {
        throw std::runtime_error(fmt::format("DatabaseAPI.SQLiteDB.RemoveRowFromTable.{}", error.what()));
    }
}

int DB::SQLiteDB::DeleteAllRows(const std::string &NameTable)
{
    try
    {
        std::string SQL_QUERY;
        if (ExistTableInDB(NameTable))
        {
            SQL_QUERY = fmt::format("DELETE FROM {} WHERE EXISTS (SELECT 1 FROM {});\nUPDATE SQLITE_SEQUENCE SET seq = 0 WHERE name = '{}';\nVACUUM;", NameTable, NameTable, NameTable);
            int RESULT_SQL = sqlite3_exec(db, SQL_QUERY.c_str(), callback, NULL, NULL);
            if (RESULT_SQL != SQLITE_OK)
            {
                std::string errorMessage = sqlite3_errmsg(db);
                throw std::runtime_error(fmt::format("Error in execution {} command: {}", SQL_QUERY, errorMessage));
            }
        }
        return 0;
    }
    catch (std::exception &error)
    {
        throw std::runtime_error(fmt::format("DatabaseAPI.SQLiteDB.DeleteAllRows.{}", error.what()));
    }
}

int DB::SQLiteDB::RunQuery(const std::string &SQL_QUERY)
{
    try
    {
        int RESULT_SQL = sqlite3_exec(db, SQL_QUERY.c_str(), callback, NULL, NULL);
        if (RESULT_SQL != SQLITE_OK)
        {
            std::string errorMessage = sqlite3_errmsg(db);
            throw std::runtime_error(fmt::format("Error in execution {} command: {}", SQL_QUERY, errorMessage));
        }
        return 0;
    }
    catch (std::exception &error)
    {
        throw std::runtime_error(fmt::format("DatabaseAPI.SQLiteDB.RunQuery.{}", error.what()));
    }
}

DB::HashedEnumDatabaseValues DB::SQLiteDB::ExecuteQuery(const std::string &SQL_QUERY)
{
    try
    {
        std::string id;
        std::string Key;
        std::string Value;
        HashedEnumDatabaseValues WriteMap;
        int n = 0;
        int RESULT_SQL;
        int num_columns;

        RESULT_SQL = sqlite3_prepare_v2(db, SQL_QUERY.c_str(), SQL_QUERY.length(), &statement, nullptr);

        if (RESULT_SQL != SQLITE_OK)
        {
            std::string errorMessage = sqlite3_errmsg(db);
            throw std::runtime_error(fmt::format("Error in execution {} command: {}", SQL_QUERY, errorMessage));
        }
        num_columns = sqlite3_column_count(statement);
        while ((RESULT_SQL = sqlite3_step(statement)) == SQLITE_ROW)
        {
            id = (const char *)sqlite3_column_text(statement, 0);
            DB::HashedDatabaseValues row;
            row.insert(std::pair<std::string, std::string>("id", id));
            for (int i = 1; i < num_columns; i++)
            {
                Key = sqlite3_column_name(statement, i);
                Value = (const char *)sqlite3_column_text(statement, i);
                row.insert(std::pair<std::string, std::string>(Key, Value));
            }
            WriteMap.insert(std::pair<int, DB::HashedDatabaseValues>(n, row));
            n++;
        }
        /*example returned WriteMap:
        {
            {0, {
                    {"id",1},{"Name","VSCode"},{"Windows","winget install vscode"...}
                }
            },
            {1, {
                    {"id",2},{"Name","CMake"},{"Windows","winget install cmake"...}
                }
            }
        }
        */
        sqlite3_finalize(statement);
        return WriteMap;
    }
    catch (std::exception &error)
    {
        throw std::runtime_error(fmt::format("DatabaseAPI.SQLiteDB.ExecuteQuery.{}", error.what()));
    }
}

int DB::SQLiteDB::UpdateRowInTable(const std::string &NameTable, DB::HashedDatabaseValues Values, DB::HashedDatabaseValues Parameters)
{
    try
    {
        std::string SQL_QUERY;
        SQL_QUERY = fmt::format("UPDATE {} SET ", NameTable);
        for (int i = 1; const auto &element : Values)
        {
            SQL_QUERY += element.first + "='" + element.second + "'";
            if (i != Values.size())
            {
                SQL_QUERY += ",";
            }
            i++;
        }

        AddExceptions(SQL_QUERY, Parameters, 1);
        SQL_QUERY += ";";
        int RESULT_SQL = sqlite3_exec(db, SQL_QUERY.c_str(), callback, NULL, NULL);
        if (RESULT_SQL != SQLITE_OK)
        {
            std::string errorMessage = sqlite3_errmsg(db);
            throw std::runtime_error(fmt::format("Error in execution {} command: {}", SQL_QUERY, errorMessage));
        }
        return 0;
    }
    catch (std::exception &error)
    {
        throw std::runtime_error(fmt::format("DatabaseAPI.SQLiteDB.UpdateRowInTable.{}", error.what()));
    }
}

int DB::SQLiteDB::countSubstr(const std::string str, const std::string substr)
{
    int count = 0;
    size_t pos = 0;
    while ((pos = str.find(substr, pos)) != std::string::npos)
    {
        count++;
        pos += substr.size();
    }
    return count;
}

void DB::SQLiteDB::AddParameters(std::string &SQL_QUERY, const DB::HashedDatabaseValues &Parameters, int maxNum_WHERE)
{
    if (Parameters.size() == 0)
    {
        throw std::runtime_error("Parameters is empty");
    }

    countSubstr(SQL_QUERY, "WHERE") < maxNum_WHERE ? SQL_QUERY += " WHERE " : SQL_QUERY += " AND ";

    for (int i = 1; const auto &element : Parameters)
    {
        SQL_QUERY += fmt::format(" {}='{}'", element.first, element.second);
        if (i != Parameters.size())
        {
            SQL_QUERY += " AND ";
        }
        i++;
    }
}

void DB::SQLiteDB::AddExceptions(std::string &SQL_QUERY, const DB::HashedDatabaseValues &Exceptions, int maxNum_WHERE)
{
    if (Exceptions.size() == 0)
    {
        throw std::runtime_error("Exceptions is empty");
    }

    countSubstr(SQL_QUERY, "WHERE") < maxNum_WHERE ? SQL_QUERY += " WHERE " : SQL_QUERY += " AND ";

    for (int i = 1; const auto &element : Exceptions)
    {
        SQL_QUERY += fmt::format(" NOT {}='{}'", element.first, element.second);
        if (i != Exceptions.size())
        {
            SQL_QUERY += " AND ";
        }
        i++;
    }
}