#!/usr/bin/env python3
from build_layout import *
import yaml
import markdown
from pathlib import Path
from mdx_gfm import GithubFlavoredMarkdownExtension
import re

def build_page(page):
  print("> Building page: %s" % page["url"])
  url = page["url"]
  path = "manual/" + page["file"] + ".md"

  if os.path.isfile(path):
    source = Path(path).read_text()
  else:
    source = "file not found"

  html = markdown.markdown(
      source,
      extensions=[GithubFlavoredMarkdownExtension()])

  html = re.sub(
      r'{%([^%]+)%}',
      lambda m: Path(os.path.join(os.environ["output_dir"], m[1])).read_text(),
      html)

  write_page(url, html)

def build_toc(toc):
  for page in toc:
    if "url" in page:
      build_page(page)
    if "pages" in page:
      build_toc(page["pages"])

def main():
  toc = yaml.load(Path("manual/toc.yaml").read_text())
  build_toc(toc["documentation"])

main()
