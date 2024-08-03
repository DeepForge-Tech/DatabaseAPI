#include <DatabaseAPI/ServicePostgresqlDB.hpp>
#include <curl/curl.h>
#include <json/json.h>
#include <unordered_map>
#include <string>
#include <stdexcept>
#include <fmt/core.h>

// DB::ServicePostgresqlDB::ServicePostgresqlDB(const std::optional<HashedDatabaseValues> &connection_data)
// {
//     if (connection_data.has_value())
//         connectionData = connection_data.value();
// }
// template class DB::ServicePostgresqlDB<DB::HashedDatabaseValues>;
// template class DB::ServicePostgresqlDB<std::map<std::string,std::string>>;

size_t DB::WriteCallback(void *contents, size_t size, size_t nmemb, void *userp)
{
    ((std::string *)userp)->append((char *)contents, size * nmemb);
    return size * nmemb;
}

template <typename DataType>
Json::Value DB::ServicePostgresqlDB::sendQuery(DataType *data)
{
    try
    {
        CURL *curl = curl_easy_init();
        CURLcode res;
        std::string readBuffer;
        Json::Value jsonResponse;
        Json::Value jsonData;
        Json::Reader jsonReader;
        std::string dataStr;
        uint16_t http_code;

        if (!connectionData.empty())
        {
            for (const auto &element : connectionData)
            {
                jsonData[element.first] = element.second;
            }
        }

        if (data != nullptr)
        {
            for (const auto &element : *data)
            {
                jsonData[element.first] = element.second;
            }
        }
        dataStr = jsonData.toStyledString();
        curl_easy_setopt(curl, CURLOPT_URL, connectionUrl.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, dataStr.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, curl_slist_append(NULL, "Content-Type: application/json"));

        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "POST");
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
        curl_easy_setopt(curl, CURLOPT_FILETIME, 1L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
        curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1);

        res = curl_easy_perform(curl);
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
        jsonReader.parse(readBuffer, jsonResponse);
        switch (http_code)
        {
        case 404:
            throw std::runtime_error(jsonData.isMember("message") ? fmt::format("404 Page Not Found.{}", jsonData["message"].asString()) : "404 Page Not Found");
        case 400:
            throw std::runtime_error(jsonData.isMember("message") ? fmt::format("400 Bad Request.{}", jsonData["message"].asString()) : "400 Bad Request");
        case 500:
            throw std::runtime_error(jsonData.isMember("message") ? fmt::format("500 Internal Server Error.{}", jsonData["message"].asString()) : "500 Internal Server Error");
        case 401:
            throw std::runtime_error(jsonData.isMember("message") ? fmt::format("401 Unauthorized.{}", jsonData["message"].asString()) : "401 Unauthorized");
        case 403:
            throw std::runtime_error(jsonData.isMember("message") ? fmt::format("403 Forbidden.{}", jsonData["message"].asString()) : "403 Forbidden");
        case 405:
            throw std::runtime_error(jsonData.isMember("message") ? fmt::format("405 Method Not Allowed.{}", jsonData["message"].asString()) : "405 Method Not Allowed");
        case 408:
            throw std::runtime_error(jsonData.isMember("message") ? fmt::format("408 Request Timeout.{}", jsonData["message"].asString()) : "408 Request Timeout");
        default:
            if (res != CURLE_OK)
                throw std::runtime_error(std::string(curl_easy_strerror(res)));
        }

        curl_easy_cleanup(curl);

        return jsonResponse;
    }
    catch (std::exception &error)
    {
        throw std::runtime_error(fmt::format("DatabaseAPI.ServicePostgresqlDB.sendQuery.{}", error.what()));
    }
}

DB::ServicePostgresqlDB &DB::ServicePostgresqlDB::setConnectionData(std::unordered_map<std::string, std::string> &connection_data)
{
    connectionData = std::move(connection_data);
    return *this;
}

DB::ServicePostgresqlDB &DB::ServicePostgresqlDB::setUrl(const char *connection_url)
{
    connectionUrl = std::move(connection_url);
    return *this;
}

DB::ServicePostgresqlDB &DB::ServicePostgresqlDB::setJsonDataStr(const char *json_data_str)
{
    jsonDataStr = std::move(json_data_str);
    return *this;
}

template <typename DataType>
Json::Value DB::ServicePostgresqlDB::handleQuery(const std::string SQL_QUERY)
{
    try
    {
        DataType inputData;
        inputData.insert(std::make_pair(jsonQueryStr, std::move(SQL_QUERY)));
        Json::Value outputData = sendQuery<DataType>(&inputData);
        if (outputData.isMember(jsonDataStr))
        {
            // std::string test = outputData[jsonDataStr].isArray() && outputData[jsonDataStr].size() == 1 ? outputData[jsonDataStr][0].toStyledString() : outputData[jsonDataStr].toStyledString();
            // std::cout << test << std::endl;
            // return outputData[jsonDataStr].isArray() ? outputData[jsonDataStr][0] : outputData[jsonDataStr];
            return outputData[jsonDataStr];
        }
        else
        {
            throw std::runtime_error(JSON_DATA_NOT_FOUND);
        }
        return outputData;
    }
    catch (std::exception &error)
    {
        throw std::runtime_error(fmt::format("DatabaseAPI.ServicePostgresqlDB.handleQuery.{}", error.what()));
    }
}

template <typename DataType>
int DB::ServicePostgresqlDB::CreateTable(const std::string &NameTable, const DataType &Columns)
{
    try
    {
        std::string SQL_QUERY;
        SQL_QUERY = fmt::format("CREATE TABLE IF NOT EXISTS {} (id SERIAL PRIMARY KEY,\n", NameTable);
        for (size_t i = 1; const auto &element : Columns)
        {
            SQL_QUERY += element.first + " " + to_upper(element.second);
            if (i != Columns.size())
            {
                SQL_QUERY += ",";
            }
            i++;
        }
        SQL_QUERY += ")";
        handleQuery<DataType>(SQL_QUERY);
        return 0;
    }
    catch (std::exception &error)
    {
        throw std::runtime_error(fmt::format("DatabaseAPI.ServicePostgresqlDB.CreateTable.{}", error.what()));
    }
}

template <typename DataType>
int DB::ServicePostgresqlDB::InsertRowToTable(const std::string &NameTable, const DataType &Fields)
{
    try
    {
        std::string SQL_QUERY;
        SQL_QUERY = fmt::format("INSERT INTO \"{}\" ", to_lower(NameTable));
        std::string Columns = "(";
        std::string Values = "(";
        for (int i = 1; const auto &element : Fields)
        {
            Columns += "\"" + to_lower(element.first) + "\"";
            Values += "'" + element.second + "'";
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
        handleQuery<DataType>(SQL_QUERY);
        return 0;
    }
    catch (std::exception &error)
    {
        throw std::runtime_error(fmt::format("DatabaseAPI.ServicePostgresqlDB.InsertRowToTable.{}", error.what()));
    }
}

bool DB::ServicePostgresqlDB::ExistTableInDB(const std::string &NameTable)
{
    try
    {
        DB::HashedDatabaseValues data;
        std::string SQL_QUERY;
        SQL_QUERY = fmt::format("SELECT EXISTS (SELECT FROM pg_tables WHERE tablename  = '{}');", to_lower(NameTable));
        // data.insert(std::make_pair("query", SQL_QUERY));
        Json::Value output = handleQuery<DB::HashedDatabaseValues>(SQL_QUERY);
        return output[0]["exists"].asBool();
    }
    catch (std::exception &error)
    {
        throw std::runtime_error(fmt::format("DatabaseAPI.ServicePostgresqlDB.ExistTableInDB.{}", error.what()));
    }
}

template <typename DataType>
std::string DB::ServicePostgresqlDB::GetValueFromRow(const std::string &NameTable, const std::string &NameColumn, const std::optional<DataType> &Parameters, const std::optional<DataType> &Exceptions)
{
    try
    {
        std::vector<DataType> outputData;
        Json::Value data;
        std::string SQL_QUERY;
        // HashedDatabaseValues values;

        // Create SQL statement
        SQL_QUERY = fmt::format("SELECT {} FROM \"{}\"", to_lower(NameColumn), to_lower(NameTable));

        Parameters.has_value() ? AddParameters<DataType>(SQL_QUERY, Parameters.value(), 1) : throw std::logic_error(ERROR_EMPTY_PARAMETERS);

        if (Exceptions.has_value())
            AddExceptions<DataType>(SQL_QUERY, Exceptions.value(), 1);

        SQL_QUERY += ";";
        // Execute SQL statement
        data = handleQuery<DataType>(SQL_QUERY);
        return data[0][to_lower(NameColumn)].asString();
    }
    catch (std::exception &error)
    {
        throw std::runtime_error(fmt::format("DatabaseAPI.ServicePostgresqlDB.GetValueFromRow.{}", error.what()));
    }
}

template <typename DataType>
DataType DB::ServicePostgresqlDB::GetRowByID(const std::string &NameTable, const int &id)
{
    try
    {
        std::string SQL_QUERY;
        Json::Value data;
        DataType outputData;

        // Create SQL statement
        SQL_QUERY = fmt::format("SELECT * FROM \"{}\" WHERE id='{}'", to_lower(NameTable), std::to_string(id));
        // Execute SQL statement
        data = handleQuery<DataType>(SQL_QUERY);
        for (const auto &key : data[0].getMemberNames())
        {
            outputData.insert(std::make_pair(key, data[0][key].asString()));
        }
        return outputData;
    }
    catch (std::exception &error)
    {
        throw std::runtime_error(fmt::format("DatabaseAPI.ServicePostgresqlDB.GetRowByID.{}", error.what()));
    }
}

template <typename SizeType, typename DataType>
SizeType DB::ServicePostgresqlDB::CountRowsInTable(const std::string &NameTable, const std::optional<DataType> &Parameters, const std::optional<DataType> &Exceptions)
{
    try
    {
        std::string SQL_QUERY;
        Json::Value data;
        SizeType result;
        SQL_QUERY = fmt::format("SELECT count(*) FROM \"{}\"", to_lower(NameTable));

        if (Parameters.has_value())
            AddParameters<DataType>(SQL_QUERY, Parameters.value(), 1);

        if (Exceptions.has_value())
            AddExceptions<DataType>(SQL_QUERY, Exceptions.value(), 1);

        SQL_QUERY += ";";
        data = handleQuery<DataType>(SQL_QUERY);
        std::stringstream ss(data[0]["count"].asString());
        ss >> result;
        return result;
    }
    catch (std::exception &error)
    {
        throw std::runtime_error(fmt::format("DatabaseAPI.ServicePostgresqlDB.CountRowsInTable.{}", error.what()));
    }
}

template <typename DataType>
int DB::ServicePostgresqlDB::RemoveRowFromTable(const std::string &NameTable, const std::optional<DataType> &Parameters, const std::optional<DataType> &Exceptions)
{
    try
    {
        std::string SQL_QUERY;

        SQL_QUERY = fmt::format("DELETE FROM {}", NameTable);

        if (Parameters.has_value())
            AddParameters<DataType>(SQL_QUERY, Parameters.value(), 1);

        if (Exceptions.has_value())
            AddExceptions<DataType>(SQL_QUERY, Exceptions.value(), 1);

        SQL_QUERY += ";";
        handleQuery<DataType>(SQL_QUERY);
        return 0;
    }
    catch (std::exception &error)
    {
        throw std::runtime_error(fmt::format("DatabaseAPI.ServicePostgresqlDB.RemoveRowFromTable.{}", error.what()));
    }
}

template <typename DataType>
std::vector<DataType> DB::ServicePostgresqlDB::GetColumnsFromTable(const std::string &NameTable, const std::vector<std::string> &Columns, const std::optional<DataType> &Parameters, const std::optional<DataType> &Exceptions)
{
    try
    {
        std::string SQL_QUERY;
        Json::Value data;
        std::vector<DataType> outputData;

        // Create SQL statement
        SQL_QUERY = "SELECT "; // fmt::format("SELECT {}, {} FROM {}", FirstColumn, SecondColumn, NameTable);
        for (size_t i = 1; const auto &column : Columns)
        {
            SQL_QUERY += to_lower(column);
            if (i != Columns.size())
                SQL_QUERY += ", ";
            i++;
        }
        SQL_QUERY += fmt::format(" FROM \"{}\"", to_lower(NameTable));
        if (Parameters.has_value())
            AddParameters<DataType>(SQL_QUERY, Parameters.value(), 1);

        if (Exceptions.has_value())
            AddExceptions<DataType>(SQL_QUERY, Exceptions.value(), 1);

        SQL_QUERY += ";";
        // Execute SQL statement
        data = handleQuery<DataType>(SQL_QUERY);
        for (const auto &arrayValue : data)
        {
            DataType tempData;
            for (const auto &key : arrayValue.getMemberNames())
            {
                tempData.insert(std::make_pair(key, arrayValue[key].asString()));
            }
            outputData.push_back(tempData);
        }
        return outputData;
    }
    catch (std::exception &error)
    {
        throw std::runtime_error(fmt::format("DatabaseAPI.ServicePostgresqlDB.GetTwoColumnsFromTable.{}", error.what()));
    }
}

template <typename DataType>
std::string DB::ServicePostgresqlDB::GetMaxValueFromTable(const std::string &NameTable, const std::string &NameColumn, const std::optional<DataType> &Parameters, const std::optional<DataType> &Exceptions)
{
    try
    {
        std::string SQL_QUERY;
        Json::Value data;

        // Create SQL statement
        SQL_QUERY = fmt::format("SELECT max({}) FROM \"{}\"", to_lower(NameColumn), to_lower(NameTable));

        Parameters.has_value() ? AddParameters(SQL_QUERY, Parameters.value(), 1) : throw std::logic_error(ERROR_EMPTY_PARAMETERS);
        if (Exceptions.has_value())
            AddExceptions<DataType>(SQL_QUERY, Exceptions.value(), 1);

        SQL_QUERY += ";";
        // Execute SQL statement
        data = handleQuery<DataType>(SQL_QUERY);
        return data[0]["max"].asString();
    }
    catch (std::exception &error)
    {
        throw std::runtime_error(fmt::format("DatabaseAPI.ServicePostgresqlDB.GetMaxValueFromTable.{}", error.what()));
    }
}

template <typename DataType>
std::vector<DataType> DB::ServicePostgresqlDB::GetMaxRowsFromTable(const std::string &NameTable, const std::string &NameColumn, const std::optional<DataType> &Parameters, const std::optional<DataType> &Exceptions)
{
    try
    {
        std::string SQL_QUERY;
        Json::Value data;
        std::vector<DataType> outputData;

        // Create SQL statement
        SQL_QUERY = fmt::format("SELECT * FROM \"{}\" WHERE {} = (SELECT max({}) FROM \"{}\"", to_lower(NameTable), to_lower(NameColumn), to_lower(NameColumn), to_lower(NameTable));

        if (Parameters.has_value())
            AddParameters(SQL_QUERY, Parameters.value(), 2);

        if (Exceptions.has_value())
            AddExceptions<DataType>(SQL_QUERY, Exceptions.value(), 2);
        SQL_QUERY += ");";
        data = handleQuery<DataType>(SQL_QUERY);
        // Execute SQL statement
        for (const auto &arrayValue : data)
        {
            DataType tempData;
            for (const auto &key : arrayValue.getMemberNames())
            {
                tempData.insert(std::make_pair(key, arrayValue[key].asString()));
            }
            outputData.push_back(tempData);
        }
        return outputData;
    }
    catch (std::exception &error)
    {
        throw std::runtime_error(fmt::format("DatabaseAPI.ServicePostgresqlDB.GetMaxRowFromTable.{}", error.what()));
    }
}

template <typename DataType>
DataType DB::ServicePostgresqlDB::GetRowFromTable(const std::string &NameTable, const std::optional<DataType> &Parameters, const std::optional<DataType> &Exceptions)
{
    try
    {
        std::string SQL_QUERY;
        Json::Value data;
        DataType outputData;

        // Create SQL statement
        SQL_QUERY = fmt::format("SELECT * FROM \"{}\"", to_lower(NameTable));

        if (Parameters.has_value())
            AddParameters(SQL_QUERY, Parameters.value(), 1);

        if (Exceptions.has_value())
            AddExceptions(SQL_QUERY, Exceptions.value(), 1);

        SQL_QUERY += ";";
        data = handleQuery<DataType>(SQL_QUERY);
        // Execute SQL statement
        // for (const auto &arrayValue : data)
        // {
        //     DataType tempData;
        //     for (const auto &key : arrayValue.getMemberNames())
        //     {
        //         tempData.insert(std::make_pair(key, arrayValue[key].asString()));
        //     }
        //     outputData.push_back(tempData);
        // }
        for (const auto &key : data[0].getMemberNames())
        {
            outputData.insert(std::make_pair(key, data[0][key].asString()));
        }
        return outputData;
    }
    catch (std::exception &error)
    {
        throw std::runtime_error(fmt::format("DatabaseAPI.ServicePostgresqlDB.GetRowFromTable.{}", error.what()));
    }
}

template <typename DataType>
std::vector<DataType> DB::ServicePostgresqlDB::GetRowsFromTable(const std::string &NameTable, const std::optional<DataType> &Parameters, const std::optional<DataType> &Exceptions)
{
    try
    {
        std::string SQL_QUERY;
        Json::Value data;
        std::vector<DataType> outputData;

        SQL_QUERY = fmt::format("SELECT * FROM \"{}\"", to_lower(NameTable));
        if (Parameters.has_value())
            AddParameters(SQL_QUERY, Parameters.value(), 1);

        if (Exceptions.has_value())
            AddExceptions<DataType>(SQL_QUERY, Exceptions.value(), 1);

        SQL_QUERY += ";";
        data = handleQuery<DataType>(SQL_QUERY);
        // Execute SQL statement
        for (const auto &arrayValue : data)
        {
            DataType tempData;
            for (const auto &key : arrayValue.getMemberNames())
            {
                tempData.insert(std::make_pair(key, arrayValue[key].asString()));
            }
            outputData.push_back(tempData);
        }
        return outputData;
    }
    catch (std::exception &error)
    {
        throw std::runtime_error(fmt::format("DatabaseAPI.ServicePostgresqlDB.GetRowsFromTable.{}", error.what()));
    }
}

template <typename DataType>
int DB::ServicePostgresqlDB::UpdateRowInTable(const std::string &NameTable, const DataType &Values, const DataType &Parameters, const std::optional<DataType> &Exceptions)
{
    try
    {
        std::string SQL_QUERY;
        SQL_QUERY = fmt::format("UPDATE \"{}\" SET ", to_lower(NameTable));
        for (int i = 1; const auto &element : Values)
        {
            SQL_QUERY += element.first + "='" + element.second + "'";
            if (i != Values.size())
            {
                SQL_QUERY += ",";
            }
            i++;
        }

        AddParameters<DataType>(SQL_QUERY, Parameters, 1);
        if (Exceptions.has_value())
            AddExceptions<DataType>(SQL_QUERY, Exceptions.value(), 1);
        SQL_QUERY += ";";
        handleQuery<DataType>(SQL_QUERY);
        return 0;
    }
    catch (std::exception &error)
    {
        throw std::runtime_error(fmt::format("DatabaseAPI.ServicePostgresqlDB.UpdateRowInTable.{}", error.what()));
    }
}

int DB::ServicePostgresqlDB::DeleteTable(const std::string &NameTable)
{
    try
    {
        std::string SQL_QUERY;
        SQL_QUERY = fmt::format("DROP TABLE IF EXISTS \"{}\";", to_lower(NameTable));
        // Execute SQL statement
        handleQuery<DB::HashedDatabaseValues>(SQL_QUERY);
        return 0;
    }
    catch (std::exception &error)
    {
        throw std::runtime_error(fmt::format("DatabaseAPI.ServicePostgresqlDB.DeleteTable.{}", error.what()));
    }
}

int DB::ServicePostgresqlDB::CleanTable(const std::string &NameTable)
{
    try
    {
        std::string SQL_QUERY;
        bool existTable;
        existTable = ExistTableInDB(NameTable);
        if (existTable)
        {
            SQL_QUERY = fmt::format("TRUNCATE \"{}\";", to_lower(NameTable));
            handleQuery<DB::HashedDatabaseValues>(SQL_QUERY);
        }
        return 0;
    }
    catch (std::exception &error)
    {
        throw std::runtime_error(fmt::format("DatabaseAPI.ServicePostgresqlDB.CleanTable.{}", error.what()));
    }
}

template <typename DataType>
std::vector<DataType> DB::ServicePostgresqlDB::ExecuteQuery(const std::string &SQL_QUERY)
{
    try
    {
        Json::Value data;
        std::vector<DataType> outputData;
        data = handleQuery<DataType>(SQL_QUERY);
        // Execute SQL statement
        for (const auto &arrayValue : data)
        {
            DataType tempData;
            for (const auto &key : arrayValue.getMemberNames())
            {
                tempData.insert(std::make_pair(key, arrayValue[key].asString()));
            }
            outputData.push_back(tempData);
        }
        return outputData;
        
    }
    catch (std::exception &error)
    {
        throw std::runtime_error(fmt::format("DatabaseAPI.ServicePostgresqlDB.ExecuteQuery.{}", error.what()));
    }
}

template <typename DataType>
void DB::ServicePostgresqlDB::AddParameters(std::string &SQL_QUERY, const DataType &Parameters, int maxNum_WHERE)
{
    if (Parameters.size() == 0)
    {
        throw std::runtime_error(ERROR_EMPTY_PARAMETERS);
    }

    countSubstr(SQL_QUERY, "WHERE") < maxNum_WHERE ? SQL_QUERY += " WHERE " : SQL_QUERY += " AND ";

    for (size_t i = 1; const auto &element : Parameters)
    {
        SQL_QUERY += fmt::format(" {}='{}'", to_lower(element.first), element.second);
        if (i != Parameters.size())
        {
            SQL_QUERY += " AND ";
        }
        i++;
    }
}

template <typename DataType>
void DB::ServicePostgresqlDB::AddExceptions(std::string &SQL_QUERY, const DataType &Exceptions, int maxNum_WHERE)
{
    if (Exceptions.size() == 0)
    {
        throw std::runtime_error(ERROR_EMPTY_EXCEPTIONS);
    }

    countSubstr(SQL_QUERY, "WHERE") < maxNum_WHERE ? SQL_QUERY += " WHERE " : SQL_QUERY += " AND ";

    for (size_t i = 1; const auto &element : Exceptions)
    {
        SQL_QUERY += fmt::format(" NOT {}='{}'", to_lower(element.first), element.second);
        if (i != Exceptions.size())
        {
            SQL_QUERY += " AND ";
        }
        i++;
    }
}

int DB::ServicePostgresqlDB::countSubstr(const std::string str, const std::string substr)
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

template Json::Value DB::ServicePostgresqlDB::sendQuery<DB::HashedDatabaseValues>(DB::HashedDatabaseValues *);
template Json::Value DB::ServicePostgresqlDB::sendQuery<DB::DatabaseValues>(DB::DatabaseValues *);

template Json::Value DB::ServicePostgresqlDB::handleQuery<DB::HashedDatabaseValues>(const std::string);
template Json::Value DB::ServicePostgresqlDB::handleQuery<DB::DatabaseValues>(const std::string);

template int DB::ServicePostgresqlDB::CreateTable<DB::HashedDatabaseValues>(const std::string &, const DB::HashedDatabaseValues &);
template int DB::ServicePostgresqlDB::CreateTable<DB::DatabaseValues>(const std::string &, const DB::DatabaseValues &);

template int DB::ServicePostgresqlDB::InsertRowToTable<DB::HashedDatabaseValues>(const std::string &, const DB::HashedDatabaseValues &);
template int DB::ServicePostgresqlDB::InsertRowToTable<DB::DatabaseValues>(const std::string &, const DB::DatabaseValues &);

template std::string DB::ServicePostgresqlDB::GetValueFromRow<DB::HashedDatabaseValues>(const std::string &, const std::string &, const std::optional<DB::HashedDatabaseValues> &, const std::optional<DB::HashedDatabaseValues> &);
template std::string DB::ServicePostgresqlDB::GetValueFromRow<DB::DatabaseValues>(const std::string &, const std::string &, const std::optional<DB::DatabaseValues> &, const std::optional<DB::DatabaseValues> &);

template void DB::ServicePostgresqlDB::AddParameters<DB::HashedDatabaseValues>(std::string &, const DB::HashedDatabaseValues &, int);
template void DB::ServicePostgresqlDB::AddParameters<DB::DatabaseValues>(std::string &, const DB::DatabaseValues &, int);

template void DB::ServicePostgresqlDB::AddExceptions<DB::HashedDatabaseValues>(std::string &, const DB::HashedDatabaseValues &, int);
template void DB::ServicePostgresqlDB::AddExceptions<DB::DatabaseValues>(std::string &, const DB::DatabaseValues &, int);

template DB::HashedDatabaseValues DB::ServicePostgresqlDB::GetRowByID<DB::HashedDatabaseValues>(const std::string &, const int &);
template DB::DatabaseValues DB::ServicePostgresqlDB::GetRowByID<DB::DatabaseValues>(const std::string &, const int &);

template std::vector<DB::HashedDatabaseValues> DB::ServicePostgresqlDB::GetColumnsFromTable<DB::HashedDatabaseValues>(const std::string &, const std::vector<std::string> &, const std::optional<DB::HashedDatabaseValues> &, const std::optional<DB::HashedDatabaseValues> &);
template std::vector<DB::DatabaseValues> DB::ServicePostgresqlDB::GetColumnsFromTable<DB::DatabaseValues>(const std::string &, const std::vector<std::string> &, const std::optional<DB::DatabaseValues> &, const std::optional<DB::DatabaseValues> &);

template int DB::ServicePostgresqlDB::RemoveRowFromTable<DB::HashedDatabaseValues>(const std::string &, const std::optional<DB::HashedDatabaseValues> &, const std::optional<DB::HashedDatabaseValues> &);
template int DB::ServicePostgresqlDB::RemoveRowFromTable<DB::DatabaseValues>(const std::string &, const std::optional<DB::DatabaseValues> &, const std::optional<DB::DatabaseValues> &);

template int DB::ServicePostgresqlDB::UpdateRowInTable<DB::HashedDatabaseValues>(const std::string &, const DB::HashedDatabaseValues &, const DB::HashedDatabaseValues &, const std::optional<DB::HashedDatabaseValues> &);
template int DB::ServicePostgresqlDB::UpdateRowInTable<DB::DatabaseValues>(const std::string &, const DB::DatabaseValues &, const DB::DatabaseValues &, const std::optional<DB::DatabaseValues> &);

template std::string DB::ServicePostgresqlDB::GetMaxValueFromTable<DB::HashedDatabaseValues>(const std::string &, const std::string &, const std::optional<DB::HashedDatabaseValues> &, const std::optional<DB::HashedDatabaseValues> &);
template std::string DB::ServicePostgresqlDB::GetMaxValueFromTable<DB::DatabaseValues>(const std::string &, const std::string &, const std::optional<DB::DatabaseValues> &, const std::optional<DB::DatabaseValues> &);

template std::vector<DB::HashedDatabaseValues> DB::ServicePostgresqlDB::GetMaxRowsFromTable<DB::HashedDatabaseValues>(const std::string &, const std::string &, const std::optional<DB::HashedDatabaseValues> &, const std::optional<DB::HashedDatabaseValues> &);
template std::vector<DB::DatabaseValues> DB::ServicePostgresqlDB::GetMaxRowsFromTable<DB::DatabaseValues>(const std::string &, const std::string &, const std::optional<DB::DatabaseValues> &, const std::optional<DB::DatabaseValues> &);

template int8_t DB::ServicePostgresqlDB::CountRowsInTable<int8_t, DB::HashedDatabaseValues>(const std::string &, const std::optional<DB::HashedDatabaseValues> &, const std::optional<DB::HashedDatabaseValues> &);
template int16_t DB::ServicePostgresqlDB::CountRowsInTable<int16_t, DB::HashedDatabaseValues>(const std::string &, const std::optional<DB::HashedDatabaseValues> &, const std::optional<DB::HashedDatabaseValues> &);
template int DB::ServicePostgresqlDB::CountRowsInTable<int, DB::HashedDatabaseValues>(const std::string &, const std::optional<DB::HashedDatabaseValues> &, const std::optional<DB::HashedDatabaseValues> &);
template int32_t DB::ServicePostgresqlDB::CountRowsInTable<int32_t, DB::HashedDatabaseValues>(const std::string &, const std::optional<DB::HashedDatabaseValues> &, const std::optional<DB::HashedDatabaseValues> &);
template int64_t DB::ServicePostgresqlDB::CountRowsInTable<int64_t, DB::HashedDatabaseValues>(const std::string &, const std::optional<DB::HashedDatabaseValues> &, const std::optional<DB::HashedDatabaseValues> &);
template unsigned int DB::ServicePostgresqlDB::CountRowsInTable<unsigned int, DB::HashedDatabaseValues>(const std::string &, const std::optional<DB::HashedDatabaseValues> &, const std::optional<DB::HashedDatabaseValues> &);
template unsigned long DB::ServicePostgresqlDB::CountRowsInTable<unsigned long, DB::HashedDatabaseValues>(const std::string &, const std::optional<DB::HashedDatabaseValues> &, const std::optional<DB::HashedDatabaseValues> &);
template unsigned long long DB::ServicePostgresqlDB::CountRowsInTable<unsigned long long, DB::HashedDatabaseValues>(const std::string &, const std::optional<DB::HashedDatabaseValues> &, const std::optional<DB::HashedDatabaseValues> &);
template uint8_t DB::ServicePostgresqlDB::CountRowsInTable<uint8_t, DB::HashedDatabaseValues>(const std::string &, const std::optional<DB::HashedDatabaseValues> &, const std::optional<DB::HashedDatabaseValues> &);
template uint16_t DB::ServicePostgresqlDB::CountRowsInTable<uint16_t, DB::HashedDatabaseValues>(const std::string &, const std::optional<DB::HashedDatabaseValues> &, const std::optional<DB::HashedDatabaseValues> &);
template uint32_t DB::ServicePostgresqlDB::CountRowsInTable<uint32_t, DB::HashedDatabaseValues>(const std::string &, const std::optional<DB::HashedDatabaseValues> &, const std::optional<DB::HashedDatabaseValues> &);
template uint64_t DB::ServicePostgresqlDB::CountRowsInTable<uint64_t, DB::HashedDatabaseValues>(const std::string &, const std::optional<DB::HashedDatabaseValues> &, const std::optional<DB::HashedDatabaseValues> &);

template int8_t DB::ServicePostgresqlDB::CountRowsInTable<int8_t, DB::DatabaseValues>(const std::string &, const std::optional<DB::DatabaseValues> &, const std::optional<DB::DatabaseValues> &);
template int16_t DB::ServicePostgresqlDB::CountRowsInTable<int16_t, DB::DatabaseValues>(const std::string &, const std::optional<DB::DatabaseValues> &, const std::optional<DB::DatabaseValues> &);
template int32_t DB::ServicePostgresqlDB::CountRowsInTable<int32_t, DB::DatabaseValues>(const std::string &, const std::optional<DB::DatabaseValues> &, const std::optional<DB::DatabaseValues> &);
template int64_t DB::ServicePostgresqlDB::CountRowsInTable<int64_t, DB::DatabaseValues>(const std::string &, const std::optional<DB::DatabaseValues> &, const std::optional<DB::DatabaseValues> &);
template int DB::ServicePostgresqlDB::CountRowsInTable<int, DB::DatabaseValues>(const std::string &, const std::optional<DB::DatabaseValues> &, const std::optional<DB::DatabaseValues> &);
template unsigned int DB::ServicePostgresqlDB::CountRowsInTable<unsigned int, DB::DatabaseValues>(const std::string &, const std::optional<DB::DatabaseValues> &, const std::optional<DB::DatabaseValues> &);
template unsigned long DB::ServicePostgresqlDB::CountRowsInTable<unsigned long, DB::DatabaseValues>(const std::string &, const std::optional<DB::DatabaseValues> &, const std::optional<DB::DatabaseValues> &);
template unsigned long long DB::ServicePostgresqlDB::CountRowsInTable<unsigned long long, DB::DatabaseValues>(const std::string &, const std::optional<DB::DatabaseValues> &, const std::optional<DB::DatabaseValues> &);
template uint8_t DB::ServicePostgresqlDB::CountRowsInTable<uint8_t, DB::DatabaseValues>(const std::string &, const std::optional<DB::DatabaseValues> &, const std::optional<DB::DatabaseValues> &);
template uint16_t DB::ServicePostgresqlDB::CountRowsInTable<uint16_t, DB::DatabaseValues>(const std::string &, const std::optional<DB::DatabaseValues> &, const std::optional<DB::DatabaseValues> &);
template uint32_t DB::ServicePostgresqlDB::CountRowsInTable<uint32_t, DB::DatabaseValues>(const std::string &, const std::optional<DB::DatabaseValues> &, const std::optional<DB::DatabaseValues> &);
template uint64_t DB::ServicePostgresqlDB::CountRowsInTable<uint64_t, DB::DatabaseValues>(const std::string &, const std::optional<DB::DatabaseValues> &, const std::optional<DB::DatabaseValues> &);

template std::vector<DB::HashedDatabaseValues> DB::ServicePostgresqlDB::GetRowsFromTable<DB::HashedDatabaseValues>(const std::string &, const std::optional<DB::HashedDatabaseValues> &, const std::optional<DB::HashedDatabaseValues> &);
template std::vector<DB::DatabaseValues> DB::ServicePostgresqlDB::GetRowsFromTable<DB::DatabaseValues>(const std::string &, const std::optional<DB::DatabaseValues> &, const std::optional<DB::DatabaseValues> &);

template DB::HashedDatabaseValues DB::ServicePostgresqlDB::GetRowFromTable<DB::HashedDatabaseValues>(const std::string &NameTable, const std::optional<DB::HashedDatabaseValues> &Parameters, const std::optional<DB::HashedDatabaseValues> &Exceptions);
template DB::DatabaseValues DB::ServicePostgresqlDB::GetRowFromTable<DB::DatabaseValues>(const std::string &NameTable, const std::optional<DB::DatabaseValues> &Parameters, const std::optional<DB::DatabaseValues> &Exceptions);

template std::vector<DB::HashedDatabaseValues> DB::ServicePostgresqlDB::ExecuteQuery(const std::string &SQL_QUERY);
template std::vector<DB::DatabaseValues> DB::ServicePostgresqlDB::ExecuteQuery(const std::string &SQL_QUERY);