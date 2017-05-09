/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *   Copyright (c) 2016 Paul Asmuth, FnordCorp B.V. <paul@asmuth.com>
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <assert.h>
#include <iostream>
#include "libtransport/json/json.h"
#include "libtransport/json/json_reader.h"
#include "libtransport/json/json_writer.h"
#include "libtransport/json/json_object.h"

namespace libtransport {
namespace json {

bool readJSON(
    JSONStorage* storage,
    JSONReader* reader) {
  std::vector<JSONElement*> stack;

  for (;;) {
    kTokenType token;
    std::string token_str;
    if (!reader->readNextToken(&token, &token_str)) {
      break;
    }

    /* push next token to stack */
    switch (token) {
      case JSON_OBJECT_BEGIN:
        stack.emplace_back(storage->allocObject());
        continue;
      case JSON_OBJECT_END:
        break;
      case JSON_ARRAY_BEGIN:
        stack.emplace_back(storage->allocArray());
        continue;
      case JSON_ARRAY_END:
        break;
      case JSON_STRING:
        stack.emplace_back(storage->allocString(token_str));
        break;
      case JSON_NUMBER:
        stack.emplace_back(storage->allocNumber(token_str));
        break;
      case JSON_TRUE:
        stack.emplace_back(storage->allocBoolean(true));
        break;
      case JSON_FALSE:
        stack.emplace_back(storage->allocBoolean(false));
        break;
      case JSON_NULL:
        stack.emplace_back(storage->allocNull());
        break;
    }

    /* pop stack */
    if (stack.empty()) {
      return false;
    }

    if (stack.size() == 1) {
      storage->setRoot(stack.back());
      return true;
    }

    if (stack[stack.size() - 2]->isArray()) {
      stack[stack.size() - 2]->getAsArray()->append(std::move(stack.back()));
      stack.pop_back();
      continue;
    }

    if (stack[stack.size() - 2]->isObject()) {
      continue;
    }

    if (stack.size() > 2 && stack[stack.size() - 3]->isObject()) {
      stack[stack.size() - 3]->getAsObject()->set(
          stack[stack.size() - 2]->toString(),
          std::move(stack.back()));
      stack.pop_back();
      stack.pop_back();
      continue;
    }

    break;
  }

  return false;
}

bool readJSON(
    JSONStorage* storage,
    ReadCallback read_cb) {
  JSONReader reader(read_cb);
  return readJSON(storage, &reader);
}

bool readJSON(
    JSONStorage* storage,
    const std::string* str) {
  JSONReader reader(str);
  return readJSON(storage, &reader);
}

bool writeJSON(
    const JSONElement* root,
    JSONWriter* writer) {
  struct StackFrame {
    const JSONElement* elem;
    bool complete;
  };

  std::stack<StackFrame> stack;
  {
    StackFrame f;
    f.elem = root;
    f.complete = false;

    stack.push(f);
  }

  while (!stack.empty()) {
    auto& cur = stack.top();

    if (cur.elem->isObject()) {
      if (cur.complete) {
        writer->endObject();
        stack.pop();
      } else {
        writer->beginObject();
        cur.complete = true;

        for (size_t i = cur.elem->getAsObject()->size(); i-- > 0; ) {
          const JSONElement* key;
          const JSONElement* value;
          cur.elem->getAsObject()->get(i, &key, &value);

          {
            StackFrame f;
            f.elem = value;
            f.complete = false;

            stack.push(f);
          }

          {
            StackFrame f;
            f.elem = key;
            f.complete = false;

            stack.push(f);
          }
        }
      }

      continue;
    }

    if (cur.elem->isArray()) {
      if (cur.complete) {
        writer->endArray();
        stack.pop();
      } else {
        writer->beginArray();
        cur.complete = true;

        for (size_t i = cur.elem->getAsArray()->size(); i-- > 0; ) {
          StackFrame f;
          f.elem = cur.elem->getAsArray()->get(i);
          f.complete = false;

          stack.push(f);
        }
      }

      continue;
    }

    if (cur.elem->isString()) {
      writer->addString(cur.elem->getAsString()->getString());
      stack.pop();
      continue;
    }

    if (cur.elem->isNumber()) {
      writer->addToken(JSON_NUMBER, cur.elem->toString());
      stack.pop();
      continue;
    }

    if (cur.elem->isBoolean()) {
      if (cur.elem->getAsBoolean()->getBoolean()) {
        writer->addTrue();
      } else {
        writer->addFalse();
      }

      stack.pop();
      continue;
    }

    if (cur.elem->isNull()) {
      writer->addNull();
      stack.pop();
      continue;
    }

    return false;
  }

  return true;
}

bool writeJSON(
    const JSONElement* root,
    WriteCallback write_cb) {
  JSONWriter writer(write_cb);
  return writeJSON(root, &writer);
}

bool writeJSON(
    const JSONElement* root,
    std::string* str) {
  JSONWriter writer(str);
  return writeJSON(root, &writer);
}

std::string escapeString(const std::string& string) {
  std::string new_str;

  for (int i = 0; i < string.size(); ++i) {
    switch (string.at(i)) {
      case 0x00:
        new_str += "\\u0000";
        break;
      case 0x01:
        new_str += "\\u0001";
        break;
      case 0x02:
        new_str += "\\u0002";
        break;
      case 0x03:
        new_str += "\\u0003";
        break;
      case 0x04:
        new_str += "\\u0004";
        break;
      case 0x05:
        new_str += "\\u0005";
        break;
      case 0x06:
        new_str += "\\u0006";
        break;
      case 0x07:
        new_str += "\\u0007";
        break;
      case '\b':
        new_str += "\\b";
        break;
      case '\t':
        new_str += "\\t";
        break;
      case '\n':
        new_str += "\\n";
        break;
      case 0x0b:
        new_str += "\\u000b";
        break;
      case '\f':
        new_str += "\\f";
        break;
      case '\r':
        new_str += "\\r";
        break;
      case 0x0e:
        new_str += "\\u000e";
        break;
      case 0x0f:
        new_str += "\\u000f";
        break;
      case 0x10:
        new_str += "\\u0010";
        break;
      case 0x11:
        new_str += "\\u0011";
        break;
      case 0x12:
        new_str += "\\u0012";
        break;
      case 0x13:
        new_str += "\\u0013";
        break;
      case 0x14:
        new_str += "\\u0014";
        break;
      case 0x15:
        new_str += "\\u0015";
        break;
      case 0x16:
        new_str += "\\u0016";
        break;
      case 0x17:
        new_str += "\\u0017";
        break;
      case 0x18:
        new_str += "\\u0018";
        break;
      case 0x19:
        new_str += "\\u0019";
        break;
      case 0x1a:
        new_str += "\\u001a";
        break;
      case 0x1b:
        new_str += "\\u001b";
        break;
      case 0x1c:
        new_str += "\\u001c";
        break;
      case 0x1d:
        new_str += "\\u001d";
        break;
      case 0x1e:
        new_str += "\\u001e";
        break;
      case 0x1f:
        new_str += "\\u001f";
        break;
      case '"':
        new_str += "\\\"";
        break;
      case '\\':
        new_str += "\\\\";
        break;
      default:
        new_str += string.at(i);
    }
  }

  return new_str;
}

} // namespace json
} // namespace libtransport

