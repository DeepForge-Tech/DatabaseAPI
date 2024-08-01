#ifndef SERVICE_POSTGRESQLDB_HPP
#define SERVICE_POSTGRESQLDB_HPP

#include <DatabaseAPI/Advanced.hpp>
#include <curl/curl.h>
#include <json/json.h>

#define JSON_DATA_NOT_FOUND "Object data not found into json response from server"

namespace DB
{
    size_t WriteCallback(void *contents, size_t size, size_t nmemb, void *userp);

    // template <typename DataType>
    class ServicePostgresqlDB
    {
    public:
        ServicePostgresqlDB(const char *connection_url = nullptr, 
                            HashedDatabaseValues *connection_data = nullptr, 
                            const char *json_query_str = nullptr, 
                            const char *json_data_str = nullptr)
        {
            if (connection_data != nullptr) connectionData = std::move(*connection_data);
            if (connection_url != nullptr) connectionUrl = std::move(connection_url);
            if (json_query_str != nullptr) jsonQueryStr = std::move(json_query_str);
            if (json_data_str != nullptr) jsonDataStr = std::move(json_data_str);
        }
        ServicePostgresqlDB& setConnectionData(std::unordered_map<std::string, std::string>& connection_data);
        ServicePostgresqlDB& setUrl(const char* connection_url);
        ServicePostgresqlDB& setJsonDataStr(const char *json_data_str);

        template<typename DataType>
        int CreateTable(const std::string &NameTable,const DataType &Columns);

        template<typename DataType>
        int InsertRowToTable(const std::string &NameTable,const DataType &Fields);

        bool ExistTableInDB(const std::string &NameTable);

        bool ExistRowInTable(const std::string &NameTable, const std::string &NameColumn, const std::string &Value);

        template<typename DataType>
        std::string GetValueFromRow(const std::string &NameTable, const std::string &NameColumn, const std::optional<DataType> &Parameters, const std::optional<DataType> &exceptions);

        template<typename DataType>
        DataType GetRowByID(const std::string &NameTable, const int &id);

        template<typename DataType>
        std::vector<DataType> GetColumnsFromTable(const std::string &NameTable, const std::vector<std::string> &Columns,const std::optional<DataType> &Parameters, const std::optional<DataType> &Exceptions);

        // ArrayDatabaseValues GetArrayOneColumnFromTable(const std::string &NameTable, const std::string &NameColumn, const std::optional<DatabaseValues> &Parameters, const std::optional<DatabaseValues> &Exceptions);

        // EnumDatabaseValues GetRowFromTable(const std::string &NameTable, const std::optional<DatabaseValues> &Parameters, const std::optional<DatabaseValues> &Exceptions);

        // EnumDatabaseValues GetAllRowsFromTable(const std::string &NameTable);

        template<typename DataType>
        std::vector<DataType> GetMaxRowsFromTable(const std::string &NameTable, const std::string &NameColumn, const std::optional<DataType> &Parameters,const std::optional<DataType> &Exceptions);
        
        template<typename DataType>
        std::string GetMaxValueFromTable(const std::string &NameTable, const std::string &NameColumn, const std::optional<DataType> &Parameters,const std::optional<DataType> &Exceptions);
        
        template<typename DataType>
        int RemoveRowFromTable(const std::string &NameTable, const std::optional<DataType> &Parameters,const std::optional<DataType> &Exceptions);

        int DeleteTable(const std::string &NameTable);

        int CleanTable(const std::string &NameTable);

        // int RunQuery(const std::string &SQL_QUERY);

        // EnumDatabaseValues ExecuteQuery(const std::string &SQL_QUERY);

        template<typename SizeType,typename DataType>
        SizeType CountRowsInTable(const std::string &NameTable,const std::optional<DataType> &Parameters,const std::optional<DataType> &Exceptions);

        template<typename DataType>
        int UpdateRowInTable(const std::string &NameTable,const DataType &Values,const DataType &Parameters,const std::optional<DataType> &Exceptions);
        // Method of make string to upper
        inline std::string to_upper(const std::string &sentence)
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
        // int GetArraySize(const std::string &NameTable, const std::string &NameColumn);
    protected:
        template<typename DataType>
        Json::Value handleQuery(const std::string SQL_QUERY);

        template <typename DataType>
        Json::Value sendQuery(DataType *data = nullptr);

        int countSubstr(const std::string str, const std::string substr);

        template <typename DataType>
        void AddParameters(std::string &SQL_QUERY, const DataType &Parameters, int maxNum_WHERE);

        template <typename DataType>
        void AddExceptions(std::string &SQL_QUERY, const DataType &exceptions, int maxNum_WHERE);
    private:
        std::string connectionUrl;
        std::string jsonQueryStr;
        std::string jsonDataStr;
        std::unordered_map<std::string, std::string> connectionData;
    };
}

#endif