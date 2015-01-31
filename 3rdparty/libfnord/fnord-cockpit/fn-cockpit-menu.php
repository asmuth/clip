<div class="menu">
  <? foreach ($menu_items as $link => $text): ?>
    <a class="item" href=<?= $link; ?>><?= $text; ?></a>
  <? endforeach; ?>
</div>
