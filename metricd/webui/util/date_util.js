
dateUtil = this.dateUtil || {};
dateUtil.kSecondsPerMinute = 60;
dateUtil.kSecondsPerHour = 3600;
dateUtil.kSecondsPerDay = 86400;
dateUtil.kMillisPerSecond = 1000;
dateUtil.kMillisPerMinute = dateUtil.kMillisPerSecond * dateUtil.kSecondsPerMinute;
dateUtil.kMillisPerHour = dateUtil.kSecondsPerHour * dateUtil.kMillisPerSecond;
dateUtil.kMillisPerDay = dateUtil.kSecondsPerDay * dateUtil.kMillisPerSecond;
dateUtil.kMillisPerWeek = dateUtil.kMillisPerDay * 7;


