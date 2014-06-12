/**
 * This file is part of the "FnordStream" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * Licensed under the MIT license (see LICENSE).
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <sys/fcntl.h>
#include <unistd.h>
#include "queryparser.h"
//#include "../agent.h"
//#include "../clock.h"
//#include "../query/query.h"
//#include "../database/database.h"

namespace fnordmetric {
namespace query {

class QueryTest {
public:
  QueryTest() {}

  void run() {
    testQueryParser();
  }

  void testQueryParser() {
    {
      QueryParser parser;
      const char* test_qry = " SELECT  fnord,sum(blah) from fubar blah.id = 'fnor\\'dbar' + 123;";
      auto bytes_consumed = parser.parse(test_qry, strlen(test_qry));
      assert(bytes_consumed == strlen(test_qry));
      auto tl = parser.token_list_;
      assert(tl.size() == 17);
      assert(tl[0].type == QueryParser::T_SELECT);
      assert(tl[1].type == QueryParser::T_STRING);
      assert(tl[1] == "fnord");
      assert(tl[2].type == QueryParser::T_COMMA);
      assert(tl[3].type == QueryParser::T_STRING);
      assert(tl[3] == "sum");
      assert(tl[4].type == QueryParser::T_LPAREN);
      assert(tl[5].type == QueryParser::T_STRING);
      assert(tl[5] == "blah");
      assert(tl[6].type == QueryParser::T_RPAREN);
      assert(tl[7].type == QueryParser::T_FROM);
      assert(tl[8].type == QueryParser::T_STRING);
      assert(tl[8] == "fubar");
      assert(tl[9].type == QueryParser::T_STRING);
      assert(tl[9] == "blah");
      assert(tl[10].type == QueryParser::T_DOT);
      assert(tl[11].type == QueryParser::T_STRING);
      assert(tl[11] == "id");
      assert(tl[12].type == QueryParser::T_EQUAL);
      assert(tl[13].type == QueryParser::T_STRING);
      assert(tl[13] == "fnord'bar");
      assert(tl[14].type == QueryParser::T_PLUS);
      assert(tl[15].type == QueryParser::T_STRING);
      assert(tl[15] == "123");
      assert(tl[16].type == QueryParser::T_SEMICOLON);
    }

    {
      QueryParser parser;
      const char* test_qry = " SELECT  fnord,sum(blah) from fubar blah.id = \"fn'o=,rdbar\" + 123;";
      auto bytes_consumed = parser.parse(test_qry, strlen(test_qry));
      assert(bytes_consumed == strlen(test_qry));
      auto tl = parser.token_list_;

      assert(tl[0].type == QueryParser::T_SELECT);
      assert(tl[1].type == QueryParser::T_STRING);
      assert(tl[1] == "fnord");
      assert(tl[2].type == QueryParser::T_COMMA);
      assert(tl[3].type == QueryParser::T_STRING);
      assert(tl[3] == "sum");
      assert(tl[4].type == QueryParser::T_LPAREN);
      assert(tl[5].type == QueryParser::T_STRING);
      assert(tl[5] == "blah");
      assert(tl[6].type == QueryParser::T_RPAREN);
      assert(tl[7].type == QueryParser::T_FROM);
      assert(tl[8].type == QueryParser::T_STRING);
      assert(tl[8] == "fubar");
      assert(tl[9].type == QueryParser::T_STRING);
      assert(tl[9] == "blah");
      assert(tl[10].type == QueryParser::T_DOT);
      assert(tl[11].type == QueryParser::T_STRING);
      assert(tl[11] == "id");
      assert(tl[12].type == QueryParser::T_EQUAL);
      assert(tl[13].type == QueryParser::T_STRING);
      assert(tl[13] == "fn'o=,rdbar");
      assert(tl[14].type == QueryParser::T_PLUS);
      assert(tl[15].type == QueryParser::T_STRING);
      assert(tl[15] == "123");
      assert(tl[16].type == QueryParser::T_SEMICOLON);
    }
  }

/*
  Agent getAgent() {
    auto database = fnordmetric::database::Database::openFile(
        "/tmp/__fnordmetric_testQuery",
        database::MODE_CONSERVATIVE |
        database::FILE_TRUNCATE |
        database::FILE_AUTODELETE);

    return Agent("testagent", std::move(database));
  }
*/
/*
  void testQuery() {
    auto agent = getAgent();
    auto stream = agent.openStream(
        "mystream",
        fnordmetric::IntegerField("seq"));
    auto raw_stream = agent.database_->openStream(
        stream->getKey().getKeyString());

    / * insert test records * /
    auto base_time = WallClock::getUnixMillis();
    RecordWriter record_writer(stream->getSchema());
    for (int i = 0; i < 1000; ++i) {
      record_writer.setIntegerField(0, i);
      record_writer.reset();
      raw_stream->appendRow(record_writer, base_time + i);
    }

    / * test query 1 * /
    auto results = agent.executeQuery("SELECT time, seq FROM mystream;");
  }
*/
};

}
}

int main() {
  fnordmetric::query::QueryTest test;
  test.run();
  printf("all tests passed! :)\n");
}
