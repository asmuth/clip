<div class="time_navigation">
  <div class="group">
    <b>End Time</b>
    <fn-datepicker
      data-time-select
      data-selectable='past'
      data-timestamp=<?= $timeNav->endTimeMilli(); ?>
    ></fn-datepicker>
  </div>
  <div class="group">
    <b>Time Range</b>
    <fn-daterangepicker
      data-selectable='past'
      data-daterange=<?= $timeNav->timeRangeMilli(); ?> 
      data-endtime=<?= $timeNav->endTimeMilli(); ?>
    ></fn-daterangepicker>
  </div>
  <div class="group">
    <b>Show The Last...</b>
    <fn-dropdown data-style="input small">
      <fn-dropdown-header>5 Minutes</fn-dropdown-header>
      <fn-dropdown-item data-value=300>5 minutes</fn-dropdown-item>
      <fn-dropdown-item data-value=900>15 minutes</fn-dropdown-item>
      <fn-dropdown-item data-value=1800>30 minutes</fn-dropdown-item>
      <fn-dropdown-item data-value=3600>1 hour</fn-dropdown-item>
      <fn-dropdown-item data-value=14400>4 hours</fn-dropdown-item>
      <fn-dropdown-item data-value=43200>12 hours</fn-dropdown-item>
      <fn-dropdown-item data-value=86400>1 day</fn-dropdown-item>
    </fn-dropdown>
  </div>

</div>

<script type='text/javascript'>
  var base = document.querySelector(".time_navigation");
  var dropdown = document.body.querySelector(".time_navigation fn-dropdown");
  var datepicker = document.body.querySelector(".time_navigation fn-datepicker");
  var daterangepicker =
    document.body.querySelector(".time_navigation fn-daterangepicker");

  dropdown.addEventListener("fn-dropdown-item-click", function(e) {
    var range = parseInt(e.srcElement.getAttribute('data-value'), 10);
    daterangepicker.setAttribute('data-daterange', (range * 1000));
    dispatchCustomEvent("start-time-select", range);
  },false);

  datepicker.addEventListener("fn-datepicker-select", function() {
    var ts = parseInt(this.getAttribute('data-timestamp'), 10);
    daterangepicker.setAttribute('data-endtime', ts);
    dispatchCustomEvent("end-time-select", Math.round(ts / 1000));
  }, false);

  daterangepicker.addEventListener("fn-daterangepicker-select-previous",
    function() {
      var ts = parseInt(this.getAttribute('data-endtime'), 10);
      datepicker.setAttribute('data-timestamp', ts);
      dispatchCustomEvent("end-time-select", Math.round(ts / 1000));
    },
  false);

  daterangepicker.addEventListener("fn-daterangepicker-select-next",
    function() {
      var ts = parseInt(this.getAttribute('data-endtime'), 10);
      datepicker.setAttribute('data-timestamp', ts);
      dispatchCustomEvent("end-time-select", Math.round(ts / 1000));
    },
  false);

  function dispatchCustomEvent(name, eventDetail) {
    var newEvent = new CustomEvent(
      name, {detail : {time: eventDetail},
      bubbles: true,
      cancelable: true});

    base.dispatchEvent(newEvent);
  };


</script>
