constexpr UnixTime::UnixTime(uint64_t utc_time) :
    utc_micros_(utc_time) {}

constexpr bool UnixTime::operator==(const UnixTime& other) const {
  return utc_micros_ == other.utc_micros_;
}

constexpr bool UnixTime::operator!=(const UnixTime& other) const {
  return utc_micros_ != other.utc_micros_;
}

constexpr bool UnixTime::operator<(const UnixTime& other) const {
  return utc_micros_ < other.utc_micros_;
}

constexpr bool UnixTime::operator>(const UnixTime& other) const {
  return utc_micros_ > other.utc_micros_;
}

constexpr bool UnixTime::operator<=(const UnixTime& other) const {
  return utc_micros_ <= other.utc_micros_;
}

constexpr bool UnixTime::operator>=(const UnixTime& other) const {
  return utc_micros_ >= other.utc_micros_;
}

constexpr UnixTime::operator uint64_t() const {
  return utc_micros_;
}

constexpr UnixTime::operator double() const {
  return utc_micros_;
}

constexpr uint64_t UnixTime::unixMicros() const {
  return utc_micros_;
}

UnixTime UnixTime::epoch() {
  return UnixTime(0);
}

inline constexpr CivilTime::CivilTime() :
    year_(0),
    month_(0),
    day_(0),
    hour_(0),
    minute_(0),
    second_(0),
    millisecond_(0),
    offset_(0) {}

inline constexpr CivilTime::CivilTime(std::nullptr_t) :
    CivilTime() {
}

inline constexpr uint16_t CivilTime::year() const {
  return year_;
}

inline constexpr uint8_t CivilTime::month() const {
  return month_;
}

inline constexpr uint8_t CivilTime::day() const {
  return day_;
}

inline constexpr uint8_t CivilTime::hour() const {
  return hour_;
}

inline constexpr uint8_t CivilTime::minute() const {
  return minute_;
}

inline constexpr uint8_t CivilTime::second() const {
  return second_;
}

inline constexpr uint16_t CivilTime::millisecond() const {
  return millisecond_;
}

inline constexpr int32_t CivilTime::offset() const {
  return offset_;
}

