<ul class="navigation">
  <? foreach ($nav_items as $link => $text): ?>
    <li><a href=<?= $link; ?>><?= $text; ?></a></li>
  <? endforeach; ?>
</ul>
