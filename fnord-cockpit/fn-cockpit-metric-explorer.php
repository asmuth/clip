<?php

function fn_render_metric_explorer() {
  $base_path = dirname(__FILE__);
  include($base_path . "/metric-explorer/metric-explorer.html");
  include($base_path . "/metric-explorer/metric-explorer-list.html");
  include($base_path . "/metric-explorer/metric-explorer-preview.html");
?>

<style type="text/css">
  <?php
    include($base_path . "/metric-explorer/fn-metric-explorer.css");
  ?>
</style>

<?php
  }
?>


