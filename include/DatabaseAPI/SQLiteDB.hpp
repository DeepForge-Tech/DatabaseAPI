#ifndef SQLITEDB_HPP
#define SQLITEDB_HPP

#include <DatabaseAPI/Advanced.hpp>
#include <sqlite3/sqlite3.h>

namespace DB
{
    class SQLiteDB
    {
    public:
        sqlite3 *db;
        sqlite3_stmt *statement;
        bool isOpen = false;
        // std::string AnswerDB;
        std::string DefaultDatabesePath = std::filesystem::current_path().generic_string() + "/DB/AppInstaller.db";
        void open(std::string *DB_Path = nullptr)
        {
            int RESULT_SQL = sqlite3_open(DB_Path != nullptr ? DB_Path->c_str() : DefaultDatabesePath.c_str(), &db);

            // if result of open database != SQLITE_OK, that send error
            if (RESULT_SQL != SQLITE_OK)
            {
                throw std::runtime_error("Failed to connect to database");
            }
            isOpen = true;
        }
        // Database();
        ~SQLiteDB()
        {
            close();
        }
        void close()
        {
            if (isOpen)
                sqlite3_close(db);
        }
        int CreateTable(const std::string &NameTable, HashedDatabaseValues Columns);

        int InsertRowToTable(const std::string &NameTable, HashedDatabaseValues Fields);

        bool ExistTableInDB(const std::string &NameTable);

        bool ExistRowInTable(const std::string &NameTable, const std::string &NameColumn, const std::string &Value);

        std::string GetValueFromRow(const std::string &NameTable, const std::string &NameColumn, const std::optional<HashedDatabaseValues> &Parameters, const std::optional<HashedDatabaseValues> &Exceptions);

        HashedDatabaseValues GetRowByID(const std::string &NameTable, const int &id);

        HashedDatabaseValues GetTwoColumnsFromTable(const std::string &NameTable, const std::string &FirstColumn, const std::string &SecondColumn, const std::optional<HashedDatabaseValues> &Parameters, const std::optional<HashedDatabaseValues> &Exceptions);

        HashedEnumColDatabaseValues GetOneColumnFromTable(const std::string &NameTable, const std::string &NameColumn, const std::optional<HashedDatabaseValues> &Parameters, const std::optional<HashedDatabaseValues> &Exceptions);

        ArrayDatabaseValues GetArrayOneColumnFromTable(const std::string &NameTable, const std::string &NameColumn, const std::optional<HashedDatabaseValues> &Parameters, const std::optional<HashedDatabaseValues> &Exceptions);

        HashedEnumDatabaseValues GetRowFromTable(const std::string &NameTable, const std::optional<HashedDatabaseValues> &Parameters, const std::optional<HashedDatabaseValues> &Exceptions);

        HashedEnumDatabaseValues GetAllRowsFromTable(const std::string &NameTable);

        HashedDatabaseValues GetMaxRowFromTable(const std::string &NameTable, const std::string &NameColumn, const std::optional<HashedDatabaseValues> &Parameters);

        std::string GetMaxValueFromTable(const std::string &NameTable, const std::string &NameColumn, const std::optional<HashedDatabaseValues> &Parameters);

        int RemoveRowFromTable(const std::string &NameTable, const std::optional<HashedDatabaseValues> &Parameters);

        int DeleteAllRows(const std::string &NameTable);

        int RunQuery(const std::string &SQL_QUERY);

        HashedEnumDatabaseValues ExecuteQuery(const std::string &SQL_QUERY);

        int UpdateRowInTable(const std::string &NameTable, HashedDatabaseValues Values, HashedDatabaseValues Parameters);
        // Method of make string to upper
        std::string to_upper(const std::string &sentence)
        {
            std::string new_sentence = "";
            for (int i = 0; i < sentence.length(); i++)
            {
                char ch = sentence[i];
                // cout << ch << endl;
                ch = toupper(ch);
                new_sentence += ch;
            }
            return new_sentence;
        }
        int GetArraySize(const std::string &NameTable, const std::string &NameColumn);

    protected:
        int countSubstr(const std::string str, const std::string substr);

        void AddParameters(std::string &SQL_QUERY, const DB::HashedDatabaseValues &Parameters, int maxNum_WHERE);

        void AddExceptions(std::string &SQL_QUERY, const HashedDatabaseValues &Exceptions, int maxNum_WHERE);

        static int callback(void *data, int argc, char **argv, char **azColName)
        {
            int i;
            fprintf(stderr, "%s: ", (const char *)data);

            for (i = 0; i < argc; i++)
            {
                printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
            }

            printf("\n");
            return 0;
        }
    };
}

#endif