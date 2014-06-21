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
#include "token.h"
#include "tokenize.h"
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
    testTokenizerSimple();
    testSelectMustBeFirstAssert();
    testSelectWildcard();
    testSelectTableWildcard();
  }

  QueryParser parseTestQuery(const char* query) {
    QueryParser parser;
    parser.parse(query, strlen(query));
    return parser;
  }

  void testSelectWildcard() {
    auto parser = parseTestQuery("SELECT * FROM sometable;");
    assert(parser.getErrors().size() == 0);
    assert(parser.statements_.size() == 1);
    const auto& stmt = parser.statements_[0];
    assert(stmt->is_wildcard == true);
  }

  void testSelectTableWildcard() {
    auto parser = parseTestQuery("SELECT mytablex.* FROM sometable;");
    assert(parser.getErrors().size() == 0);
    assert(parser.statements_.size() == 1);
    const auto& stmt = parser.statements_[0];
    assert(stmt->is_wildcard == false);
    assert(stmt->select_list.size() == 1);
    const auto& sl = stmt->select_list[0];
    assert(sl != nullptr);
    assert(sl->is_table_wildcard == true);
    assert(sl->sublist.table_wildcard_name != nullptr);
    assert(*sl->sublist.table_wildcard_name == "mytablex");
  }

  void testSelectMustBeFirstAssert() {
    auto parser = parseTestQuery("GROUP BY SELECT");
    assert(parser.getErrors().size() == 1);
    assert(parser.getErrors()[0].type == QueryParser::ERR_UNEXPECTED_TOKEN);
  }

  void testTokenizerSimple() {
    {
      auto parser = parseTestQuery(" SELECT  fnord,sum(blah) from fubar blah.id"
          "= 'fnor\\'dbar' + 123;");
      const auto& tl = parser.token_list_;
      assert(tl.size() == 17);
      assert(tl[0].type_ == Token::T_SELECT);
      assert(tl[1].type_ == Token::T_STRING);
      assert(tl[1] == "fnord");
      assert(tl[2].type_ == Token::T_COMMA);
      assert(tl[3].type_ == Token::T_STRING);
      assert(tl[3] == "sum");
      assert(tl[4].type_ == Token::T_LPAREN);
      assert(tl[5].type_ == Token::T_STRING);
      assert(tl[5] == "blah");
      assert(tl[6].type_ == Token::T_RPAREN);
      assert(tl[7].type_ == Token::T_FROM);
      assert(tl[8].type_ == Token::T_STRING);
      assert(tl[8] == "fubar");
      assert(tl[9].type_ == Token::T_STRING);
      assert(tl[9] == "blah");
      assert(tl[10].type_ == Token::T_DOT);
      assert(tl[11].type_ == Token::T_STRING);
      assert(tl[11] == "id");
      assert(tl[12].type_ == Token::T_EQUAL);
      assert(tl[13].type_ == Token::T_STRING);
      //assert(tl[13] == "fnord'bar"); // FIXPAUL
      assert(tl[14].type_ == Token::T_PLUS);
      assert(tl[15].type_ == Token::T_STRING);
      assert(tl[15] == "123");
      assert(tl[16].type_ == Token::T_SEMICOLON);
    }

    {
      auto parser = parseTestQuery(" SELECT  fnord,sum(blah) from fubar blah.id"
          "= \"fn'o=,rdbar\" + 123;");
      auto tl = &parser.token_list_;
      assert((*tl)[0].type_ == Token::T_SELECT);
      assert((*tl)[1].type_ == Token::T_STRING);
      assert((*tl)[1] == "fnord");
      assert((*tl)[2].type_ == Token::T_COMMA);
      assert((*tl)[3].type_ == Token::T_STRING);
      assert((*tl)[3] == "sum");
      assert((*tl)[4].type_ == Token::T_LPAREN);
      assert((*tl)[5].type_ == Token::T_STRING);
      assert((*tl)[5] == "blah");
      assert((*tl)[6].type_ == Token::T_RPAREN);
      assert((*tl)[7].type_ == Token::T_FROM);
      assert((*tl)[8].type_ == Token::T_STRING);
      assert((*tl)[8] == "fubar");
      assert((*tl)[9].type_ == Token::T_STRING);
      assert((*tl)[9] == "blah");
      assert((*tl)[10].type_ == Token::T_DOT);
      assert((*tl)[11].type_ == Token::T_STRING);
      assert((*tl)[11] == "id");
      assert((*tl)[12].type_ == Token::T_EQUAL);
      assert((*tl)[13].type_ == Token::T_STRING);
      assert((*tl)[13] == "fn'o=,rdbar");
      assert((*tl)[14].type_ == Token::T_PLUS);
      assert((*tl)[15].type_ == Token::T_STRING);
      assert((*tl)[15] == "123");
      assert((*tl)[16].type_ == Token::T_SEMICOLON);
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
