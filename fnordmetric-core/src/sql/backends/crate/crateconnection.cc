/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */

#include <fnordmetric/sql/backends/crate/crateconnection.h>
#include <fnordmetric/environment.h>
#include <curl/curl.h>

#include <fstream>
#include <sstream>
#include <iostream>
#include <functional>

namespace fnordmetric {
namespace query {
namespace crate_backend {

static size_t WriteCallback(char *data, size_t size, size_t nmemb, std::string *buffer)
{
   int result = 0;
   if (buffer != NULL) {
     buffer->append(data, size * nmemb);
     result = size * nmemb;
   }
   return result;
}

CrateConnection::CrateConnection(const util::URI& uri) : _uri(uri){
#ifdef FNORD_ENABLE_CRATE
   _url = "http://";
   _url.append(_uri.host());
   _url.append(":");
   _url.append(std::to_string(_uri.port()));
   _url.append("/_sql");
   env()->logger()->printf(
           "INFO",
           "uri %s", _uri.toString().c_str());
#else
   RAISE(kRuntimeError, "FnordMetric was compiled without libcurl or without rapidjson. Crate is not available");
#endif
}

std::string CrateConnection::executeQuery(
    std::string query,
    std::vector<SValue> args
    ) const {
#ifdef FNORD_ENABLE_CRATE
    rapidjson::Document json;
    json.SetObject();

    rapidjson::Document::AllocatorType& allocator = json.GetAllocator();
    rapidjson::Value query_val;
    const char * query_char = query.c_str();
    query_val.SetString(query_char, allocator);
    json.AddMember("stmt", query_val, allocator);

    if(args.size() > 0){
        rapidjson::Value _args(rapidjson::kArrayType);
        for(SValue arg : args) {
            rapidjson::Value arg_val;
            SValue::kSValueType type = arg.getType();
            if(type == SValue::kSValueType::T_FLOAT){
                arg_val = arg.getFloat();
            } else if(type == SValue::kSValueType::T_INTEGER){
                arg_val = arg.getInteger();
            } else if(type == SValue::kSValueType::T_BOOL){
                arg_val = arg.getBool();
            } else if(type == SValue::kSValueType::T_TIMESTAMP) {
                uint64_t ts = (uint64_t)arg.getTimestamp();
                arg_val.SetUint64(ts);
            } else if(type == SValue::kSValueType::T_NULL) {
                arg_val.SetNull();
            } else {
                const char * arg_char = arg.getString().c_str();
                arg_val.SetString(arg_char, allocator);
            }
            _args.PushBack(arg_val, allocator);
        }
        json.AddMember("args", _args, allocator);
    }

    // Convert JSON document to string
    rapidjson::StringBuffer strbuf;
    rapidjson::Writer<rapidjson::StringBuffer> writer(strbuf);
	json.Accept(writer);
    std::string json_str = strbuf.GetString();

    CURL *curl;
    CURLcode res;
    std::string readBuffer;

    curl = curl_easy_init();
    if(curl) {
      env()->logger()->printf(
              "INFO",
              "request to %s", _url.c_str());
      curl_easy_setopt(curl, CURLOPT_URL, _url.c_str());
      curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
      curl_easy_setopt(curl, CURLOPT_POSTFIELDS, json_str.c_str());
      curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
      curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
 
      /* Perform the request, res will get the return code */ 
      res = curl_easy_perform(curl);
      /* Check for errors */ 
      if(res != CURLE_OK) {
        env()->logger()->printf(
                "WARNING",
                "Performing request failed");
        fprintf(stderr, "curl_easy_perform() failed: %s\n",
                curl_easy_strerror(res));
      }
      curl_easy_cleanup(curl);
    }
    return readBuffer;
#else
   RAISE(kRuntimeError, "FnordMetric was compiled without libcurl or without rapidjson. Crate is not available");
#endif
}

void CrateConnection::executeQuery(
        std::string query,
        std::vector<SValue> args,
        std::function<bool (const rapidjson::Value&)> row_callback) const {
#ifdef FNORD_ENABLE_CRATE
    std::string response = executeQuery(query, args);
    rapidjson::Document d;
    d.Parse(response.c_str());
    const rapidjson::Value& rows = d["rows"];

    for(rapidjson::SizeType i = 0; i < rows.Size(); i++) {
        std::vector<rapidjson::Value> row;
        const rapidjson::Value& row_v = rows[i];
        if(!row_callback(row_v)){
            break;
        }
    }
#else
   RAISE(kRuntimeError, "FnordMetric was compiled without libcurl or without rapidjson. Crate is not available");
#endif
}

void CrateConnection::executeQuery(
        std::string query,
        std::function<bool (const rapidjson::Value&)> row_callback) const {
#ifdef FNORD_ENABLE_CRATE
    std::vector<SValue> args;
    std::string response = executeQuery(query, args);
    rapidjson::Document d;
    d.Parse(response.c_str());
    const rapidjson::Value& rows = d["rows"];

    for(rapidjson::SizeType i = 0; i < rows.Size(); i++) {
        std::vector<rapidjson::Value> row;
        const rapidjson::Value& row_v = rows[i];
        if(!row_callback(row_v)){
            break;
        }
    }
#else
   RAISE(kRuntimeError, "FnordMetric was compiled without libcurl or without rapidjson. Crate is not available");
#endif
}

std::vector<std::vector<std::string>> CrateConnection::describeTable(const std::string& table_name) {
#ifdef FNORD_ENABLE_CRATE
  std::vector<std::vector<std::string>> columns;

  std::vector<SValue> args;
  args.emplace_back(table_name);
  std::string response = executeQuery("select column_name, data_type from information_schema.columns where table_name = ?",
                                      args);

  rapidjson::Document d;
  d.Parse(response.c_str());
  const rapidjson::Value &rows = d["rows"];
  for(rapidjson::SizeType i = 0; i < rows.Size(); i++){
    std::string column = rows[i][0].GetString();
    std::string type = rows[i][1].GetString();
    std::vector<std::string> col = {column, type};
    columns.emplace_back(col);
  }
  return columns;
#else
   RAISE(kRuntimeError, "FnordMetric was compiled without libcurl or without rapidjson. Crate is not available");
#endif
}

}
}
}
