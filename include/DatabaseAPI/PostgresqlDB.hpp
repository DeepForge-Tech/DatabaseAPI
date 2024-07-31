#ifndef POSTGRESQLDB_HPP
#define POSTGRESQLDB_HPP

#include <DatabaseAPI/Advanced.hpp>
#include <pqxx/pqxx>

namespace DB
{
    class PostgresqlDB
    {
    public:
        void open(std::string *DB_Path = nullptr)
        {
        }
        int CreateTable(const std::string &NameTable, DatabaseValues Columns);

        int InsertRowToTable(const std::string &NameTable, DatabaseValues Fields);

        bool ExistTableInDB(const std::string &NameTable);

        bool ExistRowInTable(const std::string &NameTable, const std::string &NameColumn, const std::string &Value);

        std::string GetValueFromRow(const std::string &NameTable, const std::string &NameColumn, const std::optional<DatabaseValues> &Parameters, const std::optional<DatabaseValues> &Exceptions);

        DatabaseValues GetRowByID(const std::string &NameTable, const int &id);

        DatabaseValues GetTwoColumnsFromTable(const std::string &NameTable, const std::string &FirstColumn, const std::string &SecondColumn, const std::optional<DatabaseValues> &Parameters, const std::optional<DatabaseValues> &Exceptions);

        EnumColDatabaseValues GetOneColumnFromTable(const std::string &NameTable, const std::string &NameColumn, const std::optional<DatabaseValues> &Parameters, const std::optional<DatabaseValues> &Exceptions);

        ArrayDatabaseValues GetArrayOneColumnFromTable(const std::string &NameTable, const std::string &NameColumn, const std::optional<DatabaseValues> &Parameters, const std::optional<DatabaseValues> &Exceptions);

        EnumDatabaseValues GetRowFromTable(const std::string &NameTable, const std::optional<DatabaseValues> &Parameters, const std::optional<DatabaseValues> &Exceptions);

        EnumDatabaseValues GetAllRowsFromTable(const std::string &NameTable);

        DatabaseValues GetMaxRowFromTable(const std::string &NameTable, const std::string &NameColumn, const std::optional<DatabaseValues> &Parameters);

        std::string GetMaxValueFromTable(const std::string &NameTable, const std::string &NameColumn, const std::optional<DatabaseValues> &Parameters);

        int RemoveRowFromTable(const std::string &NameTable, const std::optional<DatabaseValues> &Parameters);

        int DeleteAllRows(const std::string &NameTable);

        int RunQuery(const std::string &SQL_QUERY);

        EnumDatabaseValues ExecuteQuery(const std::string &SQL_QUERY);

        int UpdateRowInTable(const std::string &NameTable, DatabaseValues Values, DatabaseValues Parameters);
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

        void AddParameters(std::string &SQL_QUERY, const DB::DatabaseValues &Parameters, int maxNum_WHERE);

        void AddExceptions(std::string &SQL_QUERY, const DatabaseValues &Exceptions, int maxNum_WHERE);

        // static int callback(void *data, int argc, char **argv, char **azColName)
        // {
        //     int i;
        //     fprintf(stderr, "%s: ", (const char *)data);

        //     for (i = 0; i < argc; i++)
        //     {
        //         printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
        //     }

        //     printf("\n");
        //     return 0;
        // }
    };
}

#endif