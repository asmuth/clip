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
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <link href="https://fonts.googleapis.com/css?family=Roboto:400,400i,500,700" rel="stylesheet">
    <meta name="google-site-verification" content="zhyGsSQu1HANExonMZDMr0LppXCCv47inMKRXiZS8aQ" />
  </head>
  <body>
    <nav>
      <a style="display: block; text-decoration: none; color: #666; float: right;" href="http://github.com/asmuth/fviz" target="_blank">
        View on <img src="https://github.githubassets.com/images/modules/logos_page/GitHub-Logo.png" style="height: 12px; position: relative; top: 1px; margin-left: 2px;">
      </a>

      <a href="/">
        <img src="/favicon.ico" style="height: 1em; position: relative; top: 0.13em; margin-right: 0.3em;"/>
        fviz
      </a>

      <span style="margin: 0 .3em">·</span>
      <a class="link" href="/" style="">Documentation</a>
      <span style="margin: 0 .3em">·</span>
      <a class="link" href="/examples">Examples</a>
      <span style="margin: 0 .3em">·</span>
      <a class="link" href="/download">Download</a>

      <ul class="crumbs">
        <li><a href="/">Documentation</a></li>
        <li><a>{{title}}</a></li>
      </ul>
    </nav>

    <article class="{{article_class}}">
      <div id="documentation">
        {{{content}}}
      </div>

      <div style="text-align: right; margin-top: 10em; font-size: 80%;">
        <a class="link" target="_blank" href="https://github.com/asmuth/fviz/blob/master/manual">
          Edit this page on GitHub
        </a>
      </div>
    </article>

    <footer>
      Copyright &copy; 2011-2018 Paul Asmuth, Laura Schlimmer et al. &mdash;
      fviz is free software licensed under the Apache 2.0 license.
    </footer>
  </body>
</html>
"""

def extend_toc(entry):
  if "pages" in entry:
    entry["pages"] = map(lambda x: extend_toc(x), entry["pages"])
  else:
    entry["pages"] = []

  return entry

def build_layout(url, content, title="", article_class=""):
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
    "article_class": article_class,
    "menu_active_documentation": "active" if (url.startswith("/documentation") or url == "/") else "",
    "menu_active_download": "active" if url.startswith("/download") else "",
    "menu_active_examples": "active" if url.startswith("/examples") else "",
    "menu_active_reference": "active" if url.startswith("/reference") else "",
  })

def write_page(url, content, **opts):
  write_file(url + "/index.html", build_layout(url, content, **opts))

def write_file(path, content):
  output_path = os.environ["output_dir"] + path
  output_dir = Path(os.path.dirname(output_path))
  output_dir.mkdir(parents=True, exist_ok=True)
  with open(output_path, "w+") as f:
    f.write(content)

def copy_file(path, src_path):
  output_path = os.environ["output_dir"] + path
  copyfile(src_path, output_path)
