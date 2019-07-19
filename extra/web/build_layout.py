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
    <title>{{title_seo}}</title>
    <link href='/documentation.css' type='text/css' rel='stylesheet' />
    <meta http-equiv="Content-Type" content="text/html;charset=utf-8">
    <link href="https://fonts.googleapis.com/css?family=Roboto:400,400i,500,700" rel="stylesheet">
    <meta name="google-site-verification" content="YeeIR81CPKSaRurQUh6j82N5tH5lcNXTsdqBICkNpyU" />
    <meta name="google-site-verification" content="GxeOO-JtsPc9Doi_WQTvQFI-b7TQgCYxjsVTNtsA7Rc" />
  </head>
  <body>
    <div id="navigation">
      <a href="/" class="logo">fviz 0.4.0</a>

      {{#toc}}
        <a class="nav_title">{{title}}</a>
        <ul>
          {{#pages}}
            <li>
              <a href="{{url}}">{{title}}</a>
              <ul>
                {{#pages}}
                  <li>
                    <a href="{{url}}">{{title}}</a>
                  </li>
                {{/pages}}
              </ul>
            </li>
          {{/pages}}
        </ul>
      {{/toc}}
    </div>

    <article>
      <div class="header">
        <a style="display: block; text-decoration: none; color: #666; float: right; margin-top: 2em;" href="http://github.com/fviz/fviz" target="_blank">
          View on <img src="https://github.githubassets.com/images/modules/logos_page/GitHub-Logo.png" style="height: 12px; position: relative; top: 1px; margin-left: 2px;">
        </a>

        <ul class="crumbs">
          <li><a>Documentation</a></li>
          <li><a>{{title}}</a></li>
        </ul>
      </div>

      <div id="documentation">
        {{{content}}}
      </div>

      <div style="text-align: right; margin-top: 10em; font-size: 80%;">
        <a class="link" target="_blank" href="https://github.com/fviz/fviz/blob/master/manual">
          Edit this page on GitHub
        </a>
      </div>

      <div id="footer">
        Copyright &copy; 2011-2018 Paul Asmuth, Laura Schlimmer et al. &mdash;
        fviz is free software licensed under the Apache 2.0 license.
      </div>
    </article>
  </body>
</html>
"""

def extend_toc(entry):
  if "pages" in entry:
    entry["pages"] = map(lambda x: extend_toc(x), entry["pages"])
  else:
    entry["pages"] = []

  return entry

def build_layout(url, content, title=""):
  toc = yaml.load(Path("manual/toc.yaml").read_text())["documentation"]
  toc = map(lambda x: extend_toc(x), toc)

  title_seo = title
  if title_seo != "fviz":
    title_seo += " - fviz"

  return TPL.render(tpl, {
    "content": content,
    "url": url,
    "toc": toc,
    "title": title,
    "title_seo": title_seo,
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
