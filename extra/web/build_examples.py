#!/usr/bin/env python3
import pystache as TPL
from glob import glob

from build_layout import *
from pathlib import Path

def build_example(url):
  print("> Building page: /examples/%s" % url)

  tpl = Path("manual/example.tpl.html").read_text()
  path = "/examples/" + url
  env = {
    "example_url": url,
    "example_src": Path(os.path.join("examples", url + ".ptx")).read_text(),
  }

  html = TPL.render(tpl, env)
  write_page(path, html)
  copy_file(path + ".svg", os.path.join("examples", url + ".svg"))

def build_example_list(examples):
  tpl = """
    <ul>
      {{#list}}
        <li><a href="/examples/{{name}}">{{name}}</a></li>
      {{/list}}
    </ul>
  """
  env = {
    "list": map(lambda x: { "name": x }, examples)
  }

  write_file("/_example_list.gen.html", TPL.render(tpl, env))

def main():
  examples = []
  for example in glob("examples/**/*.ptx", recursive=True):
    examples.append(os.path.splitext(os.path.relpath(example, "examples"))[0])

  for example in examples:
    build_example(example)

  build_example_list(examples)

main()
