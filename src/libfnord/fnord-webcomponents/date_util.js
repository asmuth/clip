/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Laura Schlimmer
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
DateUtil = {};

//Time Constants
DateUtil.secondsPerMinute = 60;
DateUtil.secondsPerHour = 3600;
DateUtil.secondsPerDay = 86400;
DateUtil.millisPerSecond = 1000;
DateUtil.millisPerMinute = DateUtil.millisPerSecond * DateUtil.secondsPerMinute;
DateUtil.millisPerHour = DateUtil.secondsPerHour * DateUtil.millisPerSecond;
DateUtil.millisPerDay = DateUtil.secondsPerDay * DateUtil.millisPerSecond;

// @date DateObject as returned by DateUtil.getDateObject
DateUtil.getDateTimeDescr = function(date) {
  if (DateUtil.isNow(date.timestamp, date.precision)) {
    return "now";
  }


  var descr =
    date.year + "-" +
    Fnord.appendLeadingZero(date.month + 1) + "-" +
    Fnord.appendLeadingZero(date.date);

  if (date.precision == 'date') {
    return descr;
  }

  descr += " " + Fnord.appendLeadingZero(date.hours);
  if (date.precision == 'hour') {
    return descr;
  }

  descr += ":" + Fnord.appendLeadingZero(date.minutes);
  if (date.precision == 'minute') {
    return descr;
  }

  descr += ":" + Fnord.appendLeadingZero(date.seconds);
  return descr;
};


//returns true if @timestamp is equal to the current timestamp (for the given @precision)
DateUtil.isNow = function(timestamp, precision) {
  return (DateUtil.equalDatesForPrecision(
    new Date().getTime(), timestamp, precision));
};

//returns true if @date is a JS Date Object and false otherwise
DateUtil.isInstanceOfDate = function(date) {
  return (date !== null && typeof date === 'object' && date instanceof Date)
};

// @date Javascript Date instance or timestamp
DateUtil.getDateObject = function(date, precision, advanced, utc_offset) {
  if (!DateUtil.isInstanceOfDate(date)) {
    date = new Date(date);

    if (date == 'Invalid Date') {
      return;
    }
  }

  //difference between utc and local time
  var timezoneOffset = date.getTimezoneOffset();

  if (utc_offset) {
    utc_offset = parseInt(utc_offset);
    if (utc_offset != timezoneOffset) {
      var offset = (utc_offset - timezoneOffset) * DateUtil.millisPerMinute;
      date.setTime(date.getTime() - offset);
    }
  }

  var dateObj = {
    'precision' : precision,
    'timestamp' : date.getTime(),
    'year': date.getFullYear(),
    'month': date.getMonth(),
    'date' : date.getDate(),
    'day' : date.getDate(),
    'utc_offset' : utc_offset ? utc_offset : 0
  }

  if (advanced) {
    dateObj.num_days = DateUtil.daysInMonth(dateObj.month + 1, dateObj.year);

    var first_day = new Date(dateObj.year + "-" + (dateObj.month + 1) + "-01");
    //counting from 0 where Monday = 0 and Sunday = 6
    dateObj.first_day = (first_day.getDay() == 0) ? 6 : first_day.getDay() - 1;
    dateObj.month_timestamp = first_day.getTime();
  }

  if (precision == 'date') {
    return dateObj;
  }

  dateObj.hours = date.getHours()
  if (precision == 'hour') {
    return dateObj;
  }

  dateObj.minutes = date.getMinutes();
  if (precision == "minute") {
    return dateObj;
  }

  dateObj.seconds = date.getSeconds();
  if (precision == "second") {
    return dateObj;
  }

  return dateObj;
};

// compares two milliseconds timestamps and checks if they are equal for the
// given precision
DateUtil.equalDatesForPrecision = function(ts1, ts2, precision) {
  if (precision == "hour") {
    return (Math.ceil(ts1 / 360000) === Math.ceil(ts2 / 360000));
  }

  if (precision == "minute") {
    return (Math.ceil(ts1 / 60000) === Math.ceil(ts2 / 60000));
  }

  if (precision == "second") {
    return (Math.ceil(ts1 / 1000) === Math.ceil(ts2 / 1000));
  }

  return false;
};

// determines month and year if @offset != 0 and
// returns the timestamp for the first day of the month
DateUtil.getMonthTimestamp = function(base_month, base_year, offset) {
  var month = base_month + offset;
  var year = base_year;

  if (month == -1) {
    year = year - 1;
    month = 11;
  }

  if (month == 12) {
    year = year + 1;
    month = 0;
  }

  return (new Date(year, month).getTime());
};

//checks if two timestamps are from the same day
DateUtil.isSameDay = function(ts1, ts2) {
  var day1 = new Date(ts1).setHours('0','0','0','0');
  var day2 = new Date(ts2).setHours('0','0','0','0');

  return (day1 === day2);
};

//returns a millisecond timestamp
DateUtil.parseTimestamp = function(timestamp) {
  //TODO add timezone
  var timestamp = parseInt(timestamp);
  //1st January 3000
  var thresholdSeconds = 32503680000;

  if (isNaN(timestamp)) {
    return Date.now();
  }

  if (timestamp < thresholdSeconds) {
    return timestamp * 1000;
  }

  return timestamp;
};

DateUtil.daysInMonth = function(month, year) {
  if (month == 2) {
    return (28 + DateUtil.leapYearOffset());
  }

  return (31 - (month - 1) % 7 % 2);
};

DateUtil.leapYearOffset = function(year) {
  //year is leap year
  if (((year % 4 == 0) && (year % 100 != 0)) || (year % 400 == 0)) {
    return 1;
  }

  return 0;
};

//returns milliseconds since midnight for @timestamp
DateUtil.getTimeTimestamp = function(timestamp) {
  var date = new Date(timestamp);
  return (
    date.getHours() * DateUtil.millisPerHour +
    date.getMinutes() * DateUtil.millisPerMinute +
    date.getSeconds() * DateUtil.millisPerSecond);
};

DateUtil.getTimestampObj = function(timestamp) {
  var time_ts = DateUtil.getTimeTimestamp(timestamp);
  return {
    'time': time_ts,
    'date': timestamp - time_ts
  }
};

// @timestamp = milliseconds since midnight
DateUtil.getTimeObjFromTimestamp = function(timestamp) {
  var hours = Math.floor(timestamp / DateUtil.millisPerHour);
  timestamp = timestamp - (hours * DateUtil.millisPerHour);

  var minutes = Math.floor(timestamp / DateUtil.millisPerMinute);
  timestamp = timestamp - (minutes * DateUtil.millisPerMinute);

  return {
    'hours' : hours,
    'minutes' : minutes,
    'seconds' : Math.floor(timestamp / DateUtil.millisPerSecond)
  }
};

// returns seconds since midnight
DateUtil.getTimestampFromTimeObj = function(timeObj) {
  var timestamp = 0;
  if (timeObj.hours) {
    timestamp += timeObj.hours * DateUtil.millisPerHour;
  }

  if (timeObj.minutes) {
    timestamp += timeObj.minutes * DateUtil.millisPerMinute;
  }

  if (timeObj.seconds) {
    timestamp += timeObj.seconds * DateUtil.millisPerSecond;
  }

  return timestamp;
};
