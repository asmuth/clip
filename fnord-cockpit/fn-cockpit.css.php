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
  background: #f2f2f2;
}

.clear {
  clear: both;
}

.viewport {
}

<?php
  include("../3rdparty/libfnord/fnord-webcomponents/components/fn-input.css");
?>

a {
  text-decoration: none;
  color: rgba(0,0,0,.8);
}

ul.fn-cockpit-navigation {
  background: #fff;
  border-bottom: 1px solid #ddd;
  padding: 0.5em 16px;
  list-style-type: none;
  line-height: 1;
  margin: 0;
}

ul.fn-cockpit-navigation li {
  display: inline;
  margin-right: 10px;
}

.fn-cockpit-pane {
  box-sizing: border-box;
  border: 1px solid #bbb;
  background: #fff;
}

.fn-cockpit-pane.collapse-right {
  border-top-right-radius: 0;
  border-bottom-right-radius: 0;
  border-right: none;
}

.fn-cockpit-pane.collapse-left {
  border-top-left-radius: 0;
  border-bottom-left-radius: 0;
  border-left: none;
}

.fn-cockpit-pane.collapse-bottom {
  border-bottom-right-radius: 0;
  border-bottom-left-radius: 0;
  border-bottom: 1px solid #eee;
}

.fn-cockpit-pane.collapse-top {
  border-top-right-radius: 0;
  border-top-left-radius: 0;
  border-top: 1px solid transparent;
}

.fn-cockpit-pane > h4 {
  padding: 0 10px;
  margin: 0;
  font-weight: 500;

  background-color: #F4F4F4;
  background-image: -webkit-gradient(linear, left top, left bottom, from(#f4f4f4), to(#e9e9e9));
  background-image: -webkit-linear-gradient(top, #f4f4f4, #e9e9e9);
  background-image: -moz-linear-gradient(top, #f4f4f4, #e9e9e9);
  background-image: -ms-linear-gradient(top, #f4f4f4, #e9e9e9);
  background-image: -o-linear-gradient(top, #f4f4f4, #e9e9e9);
  background-image: linear-gradient(top, #f4f4f4, #e9e9e9);
  filter: progid:DXImageTransform.Microsoft.gradient(startColorStr='#f4f4f4', EndColorStr='#e9e9e9');
  padding: 0 16px;
  border-bottom: 1px solid #C9C9C9;
  font-size:13px;
  line-height:29px;
  text-shadow: 1px 0px 2px rgba(255, 255, 255, 1);
  -moz-text-shadow: 1px 0px 2px rgba(255,255,255,1);
  -webkit-text-shadow: 1px 0px 2px rgba(255,255,255,1);
  overflow:hidden;
  color:#333;

}

.fn-cockpit-pane > h5 {
  padding: 0 16px;
  margin: 0;
  font-weight: 500;

  border-bottom: 1px solid #eee;
  font-size:13px;
  line-height:29px;
  text-shadow: 1px 0px 2px rgba(255, 255, 255, 1);
  -moz-text-shadow: 1px 0px 2px rgba(255,255,255,1);
  -webkit-text-shadow: 1px 0px 2px rgba(255,255,255,1);
  overflow:hidden;
  color:#333;
}

.fn-cockpit-pane > h6 {
  padding: 0 16px;
  margin: 0;
  font-weight: 500;
  position: relative;
  top: 9px;
  text-align: center;
  font-size:13px;
  line-height:29px;
  text-shadow: 1px 0px 2px rgba(255, 255, 255, 1);
  -moz-text-shadow: 1px 0px 2px rgba(255,255,255,1);
  -webkit-text-shadow: 1px 0px 2px rgba(255,255,255,1);
  overflow:hidden;
  color:#333;
}

.fn-cockpit-pane > h2 {
  padding: 0 10px;
  margin: 0;
  font-weight: 500;

  background-color: #F4F4F4;
  background-image: -webkit-gradient(linear, left top, left bottom, from(#f4f4f4), to(#e9e9e9));
  background-image: -webkit-linear-gradient(top, #f4f4f4, #e9e9e9);
  background-image: -moz-linear-gradient(top, #f4f4f4, #e9e9e9);
  background-image: -ms-linear-gradient(top, #f4f4f4, #e9e9e9);
  background-image: -o-linear-gradient(top, #f4f4f4, #e9e9e9);
  background-image: linear-gradient(top, #f4f4f4, #e9e9e9);
  filter: progid:DXImageTransform.Microsoft.gradient(startColorStr='#f4f4f4', EndColorStr='#e9e9e9');
  padding: 0 14px;
  border-bottom: 1px solid #C9C9C9;
  font-size:16px;
  line-height:34px;
  text-shadow: 1px 0px 2px rgba(255, 255, 255, 1);
  -moz-text-shadow: 1px 0px 2px rgba(255,255,255,1);
  -webkit-text-shadow: 1px 0px 2px rgba(255,255,255,1);
  overflow:hidden;
  color:#333;

}

fn-time-navigation {
  border-bottom: none;
  padding: 6px 0 0 0;
  height: 39px;
}

fn-time-navigation /deep/ .group b {
  display: none;
}

h1 {
  margin:0; padding:0; line-height: 40px; margin-left: 16px; font-size: 140%;
}

h1.small {
  margin:0; padding:0; line-height: 40px; margin-left: 16px; font-size: 110%;
}
