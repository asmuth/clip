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

constexpr Duration UnixTime::operator-(const UnixTime& other) const {
  return *this > other
      ? Duration(utc_micros_ - other.utc_micros_)
      : Duration(other.utc_micros_ - utc_micros_);
}

constexpr UnixTime UnixTime::operator+(const Duration& duration) const {
  return UnixTime(utc_micros_ + duration.microseconds());
}

constexpr UnixTime UnixTime::operator-(const Duration& duration) const {
  return UnixTime(utc_micros_ - duration.microseconds());
}

