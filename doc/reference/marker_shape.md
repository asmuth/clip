Marker Shapes
=============

The `<marker-shape>` setting is used to specify "markers" (pictograms) in
scatterplots and while drawing legends.

Below you can find a list of built-in standard markers. In addition to the
standard markers, you can use any unicode character as a custom marker and combine
any marker shape with any other marker shape.


### Syntax

The syntax for specifing markers is:

    ;; standard markers
    (<marker-name>)

    ;; custom unicode markers
    (custom <C>)

    ;; combined markers
    (combine (<M>)...)


Examples:

    ;; standard "hexagon" marker
    (hexagon)

    ;; custom unicode marker
    (custom "◐")



List of standard markers
------------------------

<table>
  <tr>
    <td>
      <div class='picto'>●</div>
      <code>circle</code>
    </td>
    <td>
      <div class='picto'>○</div>
      <code>circle-o</code>
    </td>
    <td>
      <div class='picto'>■</div>
      <code>square</code>
    </td>
    <td>
      <div class='picto'>□</div>
      <code>square-o</code>
    </td>
  </tr>
  <tr>
    <td>
      <div class='picto'>◉</div>
      <code>circle-bullet</code>
    </td>
    <td>
      <div class='picto'>◆</div>
      <code>diamond</code>
    </td>
    <td>
      <div class='picto'>◇</div>
      <code>diamond-o</code>
    </td>
    <td>
      <div class='picto'>◈</div>
      <code>diamond-bullet</code>
    </td>
  </tr>
  <tr>
    <td>
      <div class='picto'>⬖</div>
      <code>diamond-left</code>
    </td>
    <td>
      <div class='picto'>⬗</div>
      <code>diamond-right</code>
    </td>
    <td>
      <div class='picto'>⬘</div>
      <code>diamond-top</code>
    </td>
    <td>
      <div class='picto'>⬙</div>
      <code>diamond-bottom</code>
    </td>
  </tr>
  <tr>
    <td>
      <div class='picto'>⮘</div>
      <code>compass-left</code>
    </td>
    <td>
      <div class='picto'>⮚</div>
      <code>compass-right</code>
    </td>
    <td>
      <div class='picto'>⮙</div>
      <code>compass-top</code>
    </td>
    <td>
      <div class='picto'>⮛</div>
      <code>compass-bottom</code>
    </td>
  </tr>
  <tr>
    <td>
      <div class='picto'>⬢</div>
      <code>hexagon</code>
    </td>
    <td>
      <div class='picto'>⬡</div>
      <code>hexagon-o</code>
    </td>
    <td>
      <div class='picto'>⬢</div>
      <code>pentagon</code>
    </td>
    <td>
      <div class='picto'>⬡</div>
      <code>pentagon-o</code>
    </td>
  </tr>
  <tr>
    <td>
      <div class='picto'>◧</div>
      <code>square-left</code>
    </td>
    <td>
      <div class='picto'>◨</div>
      <code>square-right</code>
    </td>
    <td>
      <div class='picto'>⬒</div>
      <code>square-top</code>
    </td>
    <td>
      <div class='picto'>⬓</div>
      <code>square-bottom</code>
    </td>
  </tr>
  <tr>
    <td>
      <div class='picto'>◩</div>
      <code>square-diag-left</code>
    </td>
    <td>
      <div class='picto'>◪</div>
      <code>square-diag-right</code>
    </td>
    <td>
      <div class='picto'>⬚</div>
      <code>square-dotted</code>
    </td>
    <td>
      <div class='picto'>⯐</div>
      <code>square-crosshair</code>
    </td>
  </tr>
  <tr>
    <td>
      <div class='picto'>⯇</div>
      <code>triangle-left</code>
    </td>
    <td>
      <div class='picto'>⯈</div>
      <code>triangle-right</code>
    </td>
    <td>
      <div class='picto'>⯅</div>
      <code>triangle-top</code>
    </td>
    <td>
      <div class='picto'>⯆</div>
      <code>triangle-bottom</code>
    </td>
  </tr>
  <tr>
    <td>
      <div class='picto'>◐</div>
      <code>circle-left</code>
    </td>
    <td>
      <div class='picto'>◑</div>
      <code>circle-right</code>
    </td>
    <td>
      <div class='picto'>◓</div>
      <code>circle-top</code>
    </td>
    <td>
      <div class='picto'>◒</div>
      <code>circle-bottom</code>
    </td>
  </tr>
  <tr>
    <td>
      <div class='picto'>△</div>
      <code>triangle-left-o</code>
    </td>
    <td>
      <div class='picto'>▷</div>
      <code>triangle-right-o</code>
    </td>
    <td>
      <div class='picto'>△</div>
      <code>triangle-top-o</code>
    </td>
    <td>
      <div class='picto'>▽</div>
      <code>triangle-bottom-o</code>
    </td>
  </tr>
  <tr>
    <td>
      <div class='picto'>▣</div>
      <code>square-bullet</code>
    </td>
    <td>
      <div class='picto'>▤</div>
      <code>square-hatch-horiz</code>
    </td>
    <td>
      <div class='picto'>▥</div>
      <code>square-hatch-vert</code>
    </td>
    <td>
      <div class='picto'>▦</div>
      <code>square-hatch-cross</code>
    </td>
  </tr>
  <tr>
    <td>
      <div class='picto'>◭</div>
      <code>triangle-left-half</code>
    </td>
    <td>
      <div class='picto'>◮</div>
      <code>triangle-right-half</code>
    </td>
    <td>
      <div class='picto'>▧</div>
      <code>square-hatch-diag-left</code>
    </td>
    <td>
      <div class='picto'>▨</div>
      <code>square-hatch-diag-right</code>
    </td>
  </tr>
  <tr>
    <td>
      <div class='picto'>⭗</div>
      <code>circle-bullet-alt</code>
    </td>
    <td>
      <div class='picto'>◍</div>
      <code>circle-hatch</code>
    </td>
    <td>
      <div class='picto'>◌</div>
      <code>circle-dashed</code>
    </td>
    <td>
      <div class='picto'>◬</div>
      <code>triangle-dot</code>
    </td>
  </tr>
</table>
