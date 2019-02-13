#!/usr/bin/env python3
import pystache as TPL
import itertools
from glob import glob

from build_layout import *
from pathlib import Path

def build_example(example):
  url = example["file"]
  print("> Building page: /examples/%s" % url)

  tpl = Path("manual/example.tpl.html").read_text()
  path = "/examples/" + url
  env = {
    "example_url": url,
    "example_src": Path(os.path.join("examples", url + ".ptx")).read_text(),
    "title": "Example: %s" % url,
  }

  html = TPL.render(tpl, env)
  write_page(path, html, title=env["title"])
  copy_file(path + ".svg", os.path.join("examples", url + ".svg"))

def build_example_list(examples):
  tpl = """
  <h1 style="margin-bottom: 0;">Examples</h1>

  {{#list}}
    <div style="margin-bottom: 6em; margin-top: -1em;">
      <h2>{{section}}</h2>
      {{#files}}
        <a href="/examples/{{file}}"><h3>Example: {{file}}</h3></a>
        <div>
          {{desc}}
        </div>
        <section class="info_box" style="margin-bottom: 3em;">
          <div class="example">
            <a href="/examples/{{file}}"><img src="/examples/{{file}}.svg"></a>
          </div>
        </section>
      {{/files}}
    </div>
  {{/list}}
  """
  env = {
    "list": examples,
  }

  write_page("/examples", TPL.render(tpl, env), title="Example Gallery")

def main():
  examples = yaml.load(Path("examples/examples.yaml").read_text())
  examples_flat = list(itertools.chain(*map(lambda x: x["files"], examples)))

  for example in examples_flat:
    build_example(example)

  build_example_list(examples)

main()
