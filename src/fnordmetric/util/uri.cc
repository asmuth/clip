/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <fnordmetric/util/exception.h>
#include <fnordmetric/util/uri.h>

std::string URI::urlEncode(const std::string& str) {
  static char hextbl[] = "0123456789ABCDEF";
  std::string encoded;

  for (const auto& c : str) {
    switch (c) {
      case 'A':
      case 'B':
      case 'C':
      case 'D':
      case 'E':
      case 'F':
      case 'G':
      case 'H':
      case 'I':
      case 'J':
      case 'K':
      case 'L':
      case 'M':
      case 'N':
      case 'O':
      case 'P':
      case 'Q':
      case 'R':
      case 'S':
      case 'T':
      case 'U':
      case 'V':
      case 'W':
      case 'X':
      case 'Y':
      case 'Z':
      case 'a':
      case 'b':
      case 'c':
      case 'd':
      case 'e':
      case 'f':
      case 'g':
      case 'h':
      case 'i':
      case 'j':
      case 'k':
      case 'l':
      case 'm':
      case 'n':
      case 'o':
      case 'p':
      case 'q':
      case 'r':
      case 's':
      case 't':
      case 'u':
      case 'v':
      case 'w':
      case 'x':
      case 'y':
      case 'z':
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
      case '-':
      case '_':
      case '.':
      case '~':
        encoded += c;
        break;

      default:
        encoded += '%';
        encoded += hextbl[(c >> 4) & 0xf],
        encoded += hextbl[c & 0xf];
        break;

    }
  }

  return encoded;
}

std::string URI::urlDecode(const std::string& str) {
  std::string decoded;
  const char* begin = str.c_str();
  const char* end = begin + str.size();

  while (begin != end) {
    switch (*begin) {

      case '%': {
        if (++begin + 2 > end) {
          RAISE(kIllegalArgumentError, "invalid URL encoding");
        }

        String hstr(begin, 2);
        if (!StringUtil::isHexString(hstr)) {
          decoded += "%";
          continue;
        }

        decoded += static_cast<char>(std::stoul(hstr, nullptr, 16));
        begin += 2;
        continue;
      }

      case '+':
        decoded += " ";
        ++begin;
        continue;

      default:
        decoded += *begin++;
        continue;

    }
  }

  return decoded;
}

URI::URI() : port_(0) {}

URI::URI(const std::string& uri_str) : port_(0) {
  parse(uri_str);
}

void URI::parse(const std::string& uri_str) {
  URI::parseURI(
      uri_str,
      &scheme_,
      &userinfo_,
      &host_,
      &port_,
      &path_,
      &query_,
      &fragment_);
}

const std::string& URI::scheme() const {
  return scheme_;
}

const std::string& URI::userinfo() const {
  return userinfo_;
}

const std::string& URI::host() const {
  return host_;
}

unsigned URI::port() const {
  return port_;
}

std::string URI::hostAndPort() const {
  if (port_ > 0) {
    return StringUtil::format("$0:$1", host_, port_);
  } else {
    return host_;
  }
}

const std::string& URI::path() const {
  return path_;
}

void URI::setPath(const std::string& path) {
  path_ = path;
}

std::string URI::pathAndQuery() const {
  if (query_.length() > 0) {
    return StringUtil::format("$0?$1", path_, query_);
  } else {
    return path_;
  }
}

const std::string& URI::query() const {
  return query_;
}

URI::ParamList URI::queryParams()
    const {
  URI::ParamList params;

  if (query_.size() > 0) {
    URI::parseQueryString(query_, &params);
  }

  return params;
}

bool URI::getParam(
    const URI::ParamList& params,
    const std::string& key,
    std::string* value) {
  for (const auto& param : params) {
    if (param.first == key) {
      *value = param.second;
      return true;
    }
  }

  return false;
}

const std::string& URI::fragment() const {
  return fragment_;
}

std::string URI::toString() const {
  std::string uri_str;

  uri_str.append(scheme());
  uri_str.append(":");

  if (host_.size() > 0) {
    uri_str.append("//");

    if (userinfo_.size() > 0) {
      uri_str.append(userinfo_);
      uri_str.append("@");
    }

    uri_str.append(host_);

    if (port_ > 0) { // FIXPAUL hasPort
      uri_str.append(":");
      uri_str.append(std::to_string(port_));
    }
  }

  uri_str.append(path_);

  if (query_.size() > 0) {
    uri_str.append("?");
    uri_str.append(query_);
  }

  if (fragment_.size() > 0) {
    uri_str.append("#");
    uri_str.append(fragment_);
  }

  return uri_str;
}

void URI::parseURI(
    const std::string& uri_str,
    std::string* scheme,
    std::string* userinfo,
    std::string* host,
    unsigned* port,
    std::string* path,
    std::string* query,
    std::string* fragment) {
  const char* begin = uri_str.c_str();
  const char* end = begin + uri_str.size();

  /* scheme */
  bool has_scheme = false;
  for (const char* cur = begin; cur < end; ++cur) {
    if (*cur == '/') {
      break;
    }

    if (*cur == ':') {
      *scheme = std::string(begin, cur - begin);
      begin = cur + 1;
      has_scheme = true;
      break;
    }
  }

  /* authority */
  if (begin < end - 2 && begin[0] == '/' && begin[1] == '/') {
    begin += 2;
    const char* cur = begin;
    for (; cur < end && *cur != '/' && *cur != '?' && *cur != '#'; ++cur);
    if (cur > begin) {
      const char* abegin = begin;
      const char* aend = cur;

      /* userinfo */
      for (const char* acur = abegin; acur < aend; ++acur) {
        if (*acur == '/' || *acur == '?' || *acur == '#') {
          break;
        }

        if (*acur == '@') {
          *userinfo = std::string(abegin, acur - abegin);
          abegin = acur + 1;
          break;
        }
      }

      /* host */
      const char* acur = abegin;
      for (; acur < aend &&
            *acur != '/' &&
            *acur != '?' &&
            *acur != '#' &&
            *acur != ':'; ++acur);
      *host = std::string(abegin, acur - abegin);

      /* port */
      if (acur < aend - 1 && *acur == ':') {
        abegin = ++acur;
        for (; *acur >= '0' && *acur <= '9'; ++acur);
        if (acur > abegin) {
          std::string port_str(abegin, acur - abegin);
          try {
            *port = std::stoi(port_str);
          } catch (const std::exception& e) {
            RAISE(
                kRuntimeError,
                "invalid URI: invalid port: %s",
                port_str.c_str());
          }
        }
      }
    }
    begin = cur;
  }

  /* path */
  if (begin < end) {
    const char* cur = begin;
    for (; cur < end && *cur != '?' && *cur != '#'; ++cur);
    if (cur > begin) {
      *path = std::string(begin, cur - begin);
    }
    begin = cur;
  }

  /* query */
  if (begin < end && *begin == '?') {
    const char* cur = ++begin;
    for (; cur < end && *cur != '#'; ++cur);
    if (cur > begin) {
      *query = std::string(begin, cur - begin);
    }
    begin = cur;
  }

  /* fragment */
  if (begin < end - 1 && *begin == '#') {
    *fragment = std::string(begin + 1, end - begin - 1);
  }
}

void URI::parseQueryString(
    const std::string& query,
    std::vector<std::pair<std::string, std::string>>* params) {
  const char* begin = query.c_str();
  const char* end = begin + query.size();

  for (const char* cur = begin; cur < end; ++cur) {
    for (; cur < end && *cur != '=' && *cur != '&'; cur++);
    if (cur > begin && cur < end && *cur == '=' ) {
      std::string key_str(begin, cur - begin);
      const char* val = ++cur;
      for (; cur < end && *cur != '=' && *cur != '&'; cur++);
      if (cur > val) {
        std::string val_str(val, cur - val);
        params->emplace_back(URI::urlDecode(key_str), URI::urlDecode(val_str));
        begin = cur + 1;
      } else {
        params->emplace_back(URI::urlDecode(key_str), "");
        begin = cur + 1;
      }
    } else {
      break;
    }
  }
}

std::string URI::buildQueryString(const URI::ParamList& params) {
  std::string out;

  for (int i = 0; i < params.size(); ++i) {
    if (i > 0) {
      out += "&";
    }

    out += StringUtil::format(
        "$0=$1",
        URI::urlEncode(params[i].first),
        URI::urlEncode(params[i].second));
  }

  return out;
}

