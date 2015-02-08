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

DateUtil.appendLeadingZero = function(num) {
  if (typeof num == 'string') {
    return (num.length > 1)? num : "0" + num;
  }
  return (num > 9)? num : "0" + num;
}

 /**
  * @date DateObject as returned by DateUtil.getDateObject
  */
DateUtil.getDateTimeDescr = function(date) {
  if (DateUtil.isNow(date.timestamp, date.precision)) {
    return "now";
  }

  var descr =
    date.year + "-" +
    DateUtil.appendLeadingZero(date.month) + "-" +
    DateUtil.appendLeadingZero(date.date);

  if (date.precision == 'date') {
    return descr;
  }

  descr += " " + DateUtil.appendLeadingZero(date.hours);
  if (date.precision == 'hour') {
    return descr;
  }

  descr += ":" + DateUtil.appendLeadingZero(date.minutes);
  if (date.precision == 'minute') {
    return descr;
  }

  descr += ":" + DateUtil.appendLeadingZero(date.seconds);
  return descr;
}

DateUtil.isNow = function(timestamp, precision) {
  return (DateUtil.equalDatesForPrecision(
    new Date().getTime(), timestamp, precision));
}

DateUtil.isInstanceOfDate = function(date) {
  return (date !== null && typeof date === 'object' && date instanceof Date)
}

/**
  * @date Javascript Date instance or timestamp
  */
DateUtil.getDateObject = function(date, precision, advanced) {
  if (!DateUtil.isInstanceOfDate(date)) {
    date = new Date(date);

    if (date == 'Invalid Date') {
      return;
    }
  }

  var dateObj = {
    'precision' : precision,
    'timestamp' : date.getTime(),
    'year': date.getFullYear(),
    'month': date.getMonth(),
    'date' : date.getDate(),
    'day' : date.getDate(),
  }

  if (advanced) {
    dateObj.num_days = DateUtil.daysInMonth(dateObj.month + 1, dateObj.year);
    dateObj.first_day = new Date(
      dateObj.year + "-" + (dateObj.month + 1) + "-01").getDay();
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

/**
  * @ts = milliseconds timestamp
  */
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

DateUtil.parseTimestamp = function(timestamp) {
  //TODO add timezone
  var timestamp = parseInt(timestamp);
  //1st January 2100
  var thresholdSeconds = 4102444800;

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
  if (((year % 4 == 0) && (year % 100 != 0)) || (year % 400 == 0)) {
    return 1;
  }

  return 0;
};


