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
      <code>disk</code>
    </td>
    <td>
      <div class='picto'>○</div>
      <code>circle</code>
    </td>
    <td>
      <div class='picto'>⬢</div>
      <code>hexagon</code>
    </td>
    <td>
      <div class='picto'>⬡</div>
      <code>hexagon-o</code>
    </td>
  </tr>
</table>
