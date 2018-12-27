#!/usr/bin/env python3
import pystache as TPL
import yaml
import os
from pathlib import Path

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
        <a class="menu" href="/examples">Examples</a>
        <a class="menu active" href="/">Documentation</a>
        <h1><a href="/">PlotFX</a></h1>
      </div>
    </div>

    <div class="doc_wrap">
      <div class="doc_container">
        <div id="navigation">
          {{#toc}}
            <a class="nav_title">{{title}}</a>
            <ul>
              {{#pages}}
                <li>
                  <a href="{{url}}">{{title}}</a>
                </li>
              {{/pages}}
            </ul>
          {{/toc}}
        </div>

        <div id="documentation">
          <a target="_blank" href="https://github.com/plotfx/plotfx/blob/master/manual" style="float: right; margin-top:18px; font-size: 80%;">
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

def build_layout(url, content):
  toc = yaml.load(Path("manual/toc.yaml").read_text())["documentation"]
  return TPL.render(tpl, {"content": content, "url": url, "toc": toc})

def write_page(url, content):
  write_file(url + "/index.html", build_layout(url, content))

def write_file(path, content):
  output_path = os.environ["output_dir"] + path
  output_dir = Path(os.path.dirname(output_path))
  output_dir.mkdir(parents=True, exist_ok=True)
  with open(output_path, "w+") as f:
    f.write(content)
