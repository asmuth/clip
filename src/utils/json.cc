/**
 * This file is part of the "clip" project
 *   Copyright (c) 2020 Paul Asmuth
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <assert.h>
#include <iostream>
#include "json.h"

namespace clip {

ReturnCode json_parse_literal(
    std::istream* input,
    const std::string& literal) {
  std::string buf(literal.size(), '\0');

  input->read(buf.data(), buf.size());
  if (!input) {
    return {ERROR, "unexpected end of file"};
  }

  if (buf == literal) {
    return OK;
  } else {
    return {ERROR, "expected '" + literal + "', got '" + buf + "'"};
  }
}

ReturnCode json_parse_string(
    std::istream* input,
    std::string* token_data) {
  input->get();

  for (;;) {
    switch (input->peek()) {

      case '\\':
        switch (input->get()) {

          case '"':
            *token_data += "\"";
            break;

          case '\\':
            *token_data += "\\";
            break;

          case '/':
            *token_data += "/";
            break;

          case 'b':
            *token_data += "\b";
            break;

          case 'f':
            *token_data += "\f";
            break;

          case 'n':
            *token_data += "\n";
            break;

          case 'r':
            *token_data += "\r";
            break;

          case 't':
            *token_data += "\t";
            break;

          case 'u': {
            std::array<char, 4> code;

            input->read(code.data(), code.size());
            if (!input) {
              return {ERROR, "unexpected end of file"};
            }

            // FIXME
            break;
          }

          default:
            return {ERROR, "invalid escape sequence"};

        }

      default:
        *token_data += input->get();
        break;

      case '"':
        input->get();
        return OK;

      case 0:
        return {ERROR, "unterminated string"};

    }
  }
}

ReturnCode json_parse_number(
    std::istream* input,
    std::string* token_data) {
  for (;;) {
    switch (input->peek()) {
      case '-':
      case '+':
      case '.':
      case '0':
      case '1':
      case '2':
      case '3':
      case '4':
      case '5':
      case '6':
      case '7':
      case '8':
      case '9':
      case 'e':
      case 'E':
        *token_data += input->get();
        break;

      case ' ':
        input->get();
        break;

      case EOF:
      case 0:
        return {ERROR, "unexpected end of file"};

      default:
        return OK;
    }
  }
}

ReturnCode json_parse(
    std::istream* input,
    TokenType* token_type,
    std::string* token_data) {
  token_data->clear();

  for (;;) {
    switch (auto c = input->peek(); c) {

      case '{':
        *token_type = JSON_OBJECT_BEGIN;
        input->get();
        return OK;

      case '}':
        *token_type = JSON_OBJECT_END;
        input->get();
        return OK;

      case '[':
        *token_type = JSON_ARRAY_BEGIN;
        input->get();
        return OK;

      case ']':
        *token_type = JSON_ARRAY_END;
        input->get();
        return OK;

      case ':':
      case ',':
      case ' ':
      case '\n':
      case '\r':
      case '\t':
        input->get();
        break;

      case '-':
      case '0':
      case '1':
      case '2':
      case '3':
      case '4':
      case '5':
      case '6':
      case '7':
      case '8':
      case '9':
        *token_type = JSON_NUMBER;
        return json_parse_number(input, token_data);

      case '"':
        *token_type = JSON_STRING;
        return json_parse_string(input, token_data);

      case 't':
        *token_type = JSON_TRUE;
        return json_parse_literal(input, "true");

      case 'f':
        *token_type = JSON_FALSE;
        return json_parse_literal(input, "false");

      case 'n':
        *token_type = JSON_NULL;
        return json_parse_literal(input, "null");

      case EOF:
      case 0:
        return {ERROR, "unexpected end of file"};

      default:
        return {ERROR, "unexpected char: " + std::string(c, 1)};

    }
  }
}

ReturnCode json_read_array_begin(
    std::istream* input) {
  TokenType token;
  std::string token_data;
  if (auto rc = json_parse(input, &token, &token_data); !rc) {
    return rc;
  }

  if (token == JSON_ARRAY_BEGIN) {
    return OK;
  } else {
    return {ERROR, "expected a JSON object"};
  }
}

ReturnCode json_read_object_begin(
    std::istream* input) {
  TokenType token;
  std::string token_data;
  if (auto rc = json_parse(input, &token, &token_data); !rc) {
    return rc;
  }

  if (token == JSON_OBJECT_BEGIN) {
    return OK;
  } else {
    return {ERROR, "expected a JSON object"};
  }
}

ReturnCode json_read_string(
    std::istream* input,
    std::string* value) {
  TokenType token;
  if (auto rc = json_parse(input, &token, value); !rc) {
    return rc;
  }

  if (token == JSON_STRING) {
    return OK;
  } else {
    return {ERROR, "expected a JSON string"};
  }

  return OK;
}

ReturnCode json_skip_array(std::istream* input);
ReturnCode json_skip_object(std::istream* input);

ReturnCode json_skip_array(std::istream* input) {
  for (;;) {
    TokenType token;
    std::string token_data;
    if (auto rc = json_parse(input, &token, &token_data); !rc) {
      return rc;
    }

    switch (token) {
      case JSON_ARRAY_BEGIN:
        if (auto rc = json_skip_array(input); !rc) {
          return rc;
        }
        break;

      case JSON_OBJECT_BEGIN:
        if (auto rc = json_skip_object(input); !rc) {
          return rc;
        }
        break;

      case JSON_NUMBER:
      case JSON_STRING:
      case JSON_NULL:
      case JSON_TRUE:
      case JSON_FALSE:
        break;

      case JSON_ARRAY_END:
        return OK;

      case JSON_OBJECT_END:
        return {ERROR, "invalid JSON"};
    }
  }
}

ReturnCode json_skip_object(std::istream* input) {
  for (;;) {
    TokenType token;
    std::string token_data;
    if (auto rc = json_parse(input, &token, &token_data); !rc) {
      return rc;
    }

    switch (token) {
      case JSON_OBJECT_END:
        return OK;
      case JSON_STRING:
        break;
      default:
        return {ERROR, "invalid JSON: object keys must be strings"};
    }

    if (auto rc = json_parse(input, &token, &token_data); !rc) {
      return rc;
    }

    switch (token) {
      case JSON_ARRAY_BEGIN:
        if (auto rc = json_skip_array(input); !rc) {
          return rc;
        }
        break;

      case JSON_OBJECT_BEGIN:
        if (auto rc = json_skip_object(input); !rc) {
          return rc;
        }
        break;

      case JSON_NUMBER:
      case JSON_STRING:
      case JSON_NULL:
      case JSON_TRUE:
      case JSON_FALSE:
        break;

      case JSON_ARRAY_END:
      case JSON_OBJECT_END:
        return {ERROR, "invalid JSON"};
    }
  }
}

ReturnCode json_skip(std::istream* input) {
  TokenType token;
  std::string token_data;
  if (auto rc = json_parse(input, &token, &token_data); !rc) {
    return rc;
  }

  switch (token) {
    case JSON_ARRAY_BEGIN:
      if (auto rc = json_skip_array(input); !rc) {
        return rc;
      }
      break;

    case JSON_OBJECT_BEGIN:
      if (auto rc = json_skip_object(input); !rc) {
        return rc;
      }
      break;

    case JSON_NUMBER:
    case JSON_STRING:
    case JSON_NULL:
    case JSON_TRUE:
    case JSON_FALSE:
      break;

    case JSON_OBJECT_END:
    case JSON_ARRAY_END:
      return {ERROR, "invalid JSON"};

  }

  return OK;
}

} // namespace clip
