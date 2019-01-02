#!/usr/bin/env python3
import pystache as TPL
import yaml
import os
from pathlib import Path
from shutil import copyfile

tpl = """
<!DOCTYPE html>
<html>
  <head>
    <title>{{title}} | plotfx</title>
    <link href='/documentation.css' type='text/css' rel='stylesheet' />
    <meta http-equiv="Content-Type" content="text/html;charset=utf-8">
  </head>
  <body>
    <div id="header" class="border">
      <div class="doc_wrap">
        <a class="menu" href="http://github.com/plotfx/plotfx" target="_blank">Github</a>
        <a class="menu {{menu_active_download}}" href="/download">Download</a>
        <a class="menu {{menu_active_reference}}" href="/reference">API Reference</a>
        <a class="menu {{menu_active_examples}}" href="/examples">Examples</a>
        <a class="menu {{menu_active_documentation}}" href="/">Documentation</a>
        <a href="/"><img src="/logo.png" alt="PlotFX" class="logo" /></a>
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

def build_layout(url, content, title=""):
  toc = yaml.load(Path("manual/toc.yaml").read_text())["documentation"]
  return TPL.render(tpl, {
    "content": content,
    "url": url,
    "toc": toc,
    "title": title,
    "menu_active_documentation": "active" if (url.startswith("/documentation") or url == "/") else "",
    "menu_active_download": "active" if url.startswith("/download") else "",
    "menu_active_examples": "active" if url.startswith("/examples") else "",
    "menu_active_reference": "active" if url.startswith("/reference") else "",
  })

def write_page(url, content, title=""):
  write_file(url + "/index.html", build_layout(url, content, title=title))

def write_file(path, content):
  output_path = os.environ["output_dir"] + path
  output_dir = Path(os.path.dirname(output_path))
  output_dir.mkdir(parents=True, exist_ok=True)
  with open(output_path, "w+") as f:
    f.write(content)

def copy_file(path, src_path):
  output_path = os.environ["output_dir"] + path
  copyfile(src_path, output_path)
