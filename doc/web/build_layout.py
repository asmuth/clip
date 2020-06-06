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
    <meta name="google-site-verification" content="w2kCLJSotDjUxYBmGi3G2vsv4SO8j5K0YD0KVHoD7rw" />
    <link href="https://fonts.googleapis.com/css2?family=Inconsolata:wght@400;600&display=swap" rel="stylesheet">
  </head>
  <body class="{{article_class}}">
    <nav style="text-align: right;">
      <a class="logo" href="/" style="">clip v{{version}}</a>
      <a class="link" href="/" style="">Documentation</a>
      <span style="margin: 0 .3em">·</span>
      <a class="link" href="/examples">Examples</a>
      <span style="margin: 0 .3em">·</span>
      <a class="link" href="/installation">Download</a>
      <span style="margin: 0 .3em">·</span>
      <a class="link" href="http://github.com/asmuth/clip">Github</a>
    </nav>
    {{#breadcrumbs_present}}
    <nav style="border-bottom: none; box-shadow: none; font-size: 90%; margin-bottom: .4em; margin-top: -.2em;">
      <a href="" class="link">Plotting Data (2D)</a>
      <code>&rarr;</code>
      <a href="" class="link">API Reference</a>
      <code>&rarr;</code>
      <a href="">Command: areas</a>
    </nav>
    {{/breadcrumbs_present}}
    <article class="{{article_class}}">
      {{{content}}}
      <div style="text-align: right; margin-top: 10em; font-size: 80%;">
        <a class="link" target="_blank" href="https://github.com/asmuth/clip/blob/master/doc">
          Edit this page on GitHub
        </a>
      </div>
    </article>
    <footer>
      Copyright &copy; 2011-2018 Paul Asmuth, Laura Schlimmer et al. &mdash;
      clip is free software licensed under the Apache 2.0 license.
    </footer>
  </body>
</html>
"""

def build_layout(url, content, title="", article_class=""):
  title_seo = title
  if title_seo == "clip":
    title_seo += " - The command line illustration processor"
  else:
    title_seo += " - clip"

  return TPL.render(tpl, {
    "content": content,
    "version": os.getenv("CLIP_VERSION"),
    "url": url,
    "title": title,
    "title_seo": title_seo,
    "article_class": article_class,
    "breadcrumbs_present": False,
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
