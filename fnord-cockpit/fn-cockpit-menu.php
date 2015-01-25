<ul class="navigation">
  <? foreach ($nav_items as $link => $text): ?>
    <li><a href=<?= $link; ?>><?= $text; ?></a></li>
  <? endforeach; ?>
</ul>
<div class="menu">
  <? foreach ($menu_items as $link => $text): ?>
    <a class="item" href=<?= $link; ?>><?= $text; ?></a>
  <? endforeach; ?>
</div>
