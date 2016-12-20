
dateUtil = this.dateUtil || {};
dateUtil.kSecondsPerMinute = 60;
dateUtil.kSecondsPerHour = 3600;
dateUtil.kSecondsPerDay = 86400;
dateUtil.kMillisPerSecond = 1000;
dateUtil.kMillisPerMinute = dateUtil.kMillisPerSecond * dateUtil.kSecondsPerMinute;
dateUtil.kMillisPerHour = dateUtil.kSecondsPerHour * dateUtil.kMillisPerSecond;
dateUtil.kMillisPerDay = dateUtil.kSecondsPerDay * dateUtil.kMillisPerSecond;
dateUtil.kMillisPerWeek = dateUtil.kMillisPerDay * 7;

dateUtil.daysInMonth = function(month, year) {
  if (month == 2) {
    return (28 + dateUtil.leapYearOffset());
  }

  return (31 - (month - 1) % 7 % 2);
}

dateUtil.leapYearOffset = function(year) {
  //year is leap year
  if (((year % 4 == 0) && (year % 100 != 0)) || (year % 400 == 0)) {
    return 1;
  }

  return 0;
}

dateUtil.isSameMonth = function(d, d1) {
  return d.getFullYear() == d1.getFullYear() && d.getMonth() == d1.getMonth();
}

dateUtil.isCurrentMonth = function(d) {
  return dateUtil.isSameMonth(d, new Date());
}

dateUtil.toUTC = function(timestamp) {
  var date = new Date(timestamp);
  return timestamp - (date.getTimezoneOffset() * dateUtil.kMillisPerMinute);
}

dateUtil.toLocal = function(timestamp) {
  var date = new Date(timestamp);
  return timestamp + (date.getTimezoneOffset() * dateUtil.kMillisPerMinute);
}

/**
  * Format to a date time string yyyy-mm-dd hh:mm
  * @param timestamp to format
  * @param timezone determines the timezone (must be utc or local, defaults to local)
**/
dateUtil.formatDateTime = function(timestamp, timezone) {
  function appendLeadingZero(num) {
    if (num < 10) {
      return "0" + num;
    }

    return "" + num;
  }

  var d;
  if (timezone == 'utc') {
    d = new Date(dateUtil.toLocal(timestamp));
  } else {
    d = new Date(timestamp);
  }
  return [
      d.getFullYear(), "-",
      appendLeadingZero(d.getMonth() + 1), "-",
      appendLeadingZero(d.getDate()), " ",
      appendLeadingZero(d.getHours()), ":",
      appendLeadingZero(d.getMinutes())].join("");
}

