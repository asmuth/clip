#!/usr/bin/env python3
import pystache as TPL

tpl = """
<!DOCTYPE html>
<html>
  <head>
    <title>plotfx | <%= @title || "Collect and visualize timeseries data with SQL" %></title>
    <link href='/documentation.css' type='text/css' rel='stylesheet' />
    <meta http-equiv="Content-Type" content="text/html;charset=utf-8">
  </head>
  <body>
    <div id="header" class="border">
      <div class="doc_wrap">
        <a class="menu" href="http://github.com/plotfx/plotfx" target="_blank">Github</a>
        <a class="menu" href="/documentation/download">Download</a>
        <a class="menu active" href="/reference">API Reference</a>
        <a class="menu active" href="/">Documentation</a>
        <a class="menu" href="/examples">Examples</a>
        <h1><a href="/">PlotFX</a></h1>
      </div>
    </div>

    <div class="doc_wrap">
      <div class="doc_container">
        <div id="navigation">
        </div>

        <div id="documentation">
          <a target="_blank" href="https://github.com/paulasmuth/plotfx/blob/master/doc/FIXME..md" style="float: right; margin-top:18px; font-size: 80%;">
            Edit this page on GitHub
          </a>

          {{{content}}}
        </div>
      </div>
    </div>

    <div id="footer">
      <div class="wrap">
        Copyright &copy; 2011-2018 Paul Asmuth, Laura Schlimmer et al. &mdash;
        PlotFX is free software licensed under a 3-Clause BSD License
      </div>
    </div>
  </body>
</html>
"""

def build_layout(content):
  return TPL.render(tpl, {'content': content})

