#!/usr/bin/env python3
from build_layout import *
import yaml
import markdown
from pathlib import Path
import re

def build_page(page):
  if not "file" in page:
    return

  print("> Building page: %s" % page["url"])
  url = page["url"]
  path = "manual/" + page["file"] + ".md"
  title = page.get("doctitle", page["title"])

  if os.path.isfile(path):
    source = Path(path).read_text()
  else:
    source = "file not found"

  html = markdown.markdown(source)

  html = re.sub(
      r'{%([^%]+)%}',
      lambda m: Path(os.path.join(os.environ["output_dir"], m[1])).read_text(),
      html)

  write_page(url, html, title=title)

def build_toc(toc):
  for page in toc:
    if "url" in page:
      build_page(page)
    if "pages" in page:
      build_toc(page["pages"])
    if "sections" in page:
      build_toc(page["sections"])

def main():
  toc = yaml.load(Path("manual/toc.yaml").read_text())
  build_toc(toc["documentation"])

main()
