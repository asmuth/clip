html {
  height: 100%;
  font-size: 14px;
  font-family: 'Helvetica Neue',Arial,Helvetica,sans-serif;
}
body {
  padding: 0;
  margin: 0;
  font-size: 14px;
  font-family: 'Helvetica Neue',Arial,Helvetica,sans-serif;
  line-height: 1.33;
  color: rgba(0,0,0,.8);
  height: 100%;
}

.clear {
  clear: both;
}

.viewport {
  margin: 0 0 0 200px;
  text-align:center;
}

<?php
  include("../3rdparty/libfnord/fnord-webcomponents/components/fn-input.css");
?>

a {
  text-decoration: none;
  color: rgba(0,0,0,.8);
}

ul.navigation {
  border-bottom: 1px solid rgba(0,0,0,.1);
  padding: 0.5em;
  list-style-type: none;
  line-height: 1;
  margin: 0;

}

ul.navigation li {
  display: inline;
  padding: 0 5px;
}

.menu {
  position: fixed;
  float:left;
  height: 100%;
  width: 200px;;
  background: #fff;
  font-weight: 400;
  border: 1px solid rgba(39,41,43,.15);
  border-width: 0 1px 1px 0;
  border-bottom-right-radius: .2857rem;
}

.menu a.item {
  padding: .78571em .95em;
  background: 0 0;
  display: block;
  height: auto!important;
  border-top: none;
  border-left: 0 solid transparent;
  border-right: none;
  vertical-align: middle;
  line-height: 1;
  box-sizing: border-box;
  -webkit-tap-highlight-color: transparent;
  -webkit-user-select: none;
  -moz-user-select: none;
  -ms-user-select: none;
  user-select: none;
  -webkit-transition: opacity .2s ease,background .2s ease,box-shadow .2s ease;
  border-bottom: 1px solid rgba(0,0,0,.05)
}



.menu a.item:hover{
  background-color: rgba(0,0,0,.03);
}


