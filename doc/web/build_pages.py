#!/usr/bin/env python3
from build_layout import *
import yaml
import markdown
from pathlib import Path
import re
import glob

def build_page_cmdref(page):
  tpl = Path("web/reference_detail.tpl.html").read_text()
  type_paths = ["types", os.path.join(os.path.dirname(page["file"]), "types")]

  args = []
  for arg in page.get("arguments", []):
    if "include" in arg:
      include_path = os.path.join(os.path.dirname(page["file"]), arg["include"] + ".yaml")
      include = yaml.load(open(include_path))
      for arg in include["arguments"]:
        args.append(arg)
    else:
      args.append(arg)

  for i, p in enumerate(args):
    p.setdefault("examples", [])
    if "desc" in p:
      args[i]["desc"] = markdown.markdown(p["desc"])
    if "desc_detail" in p:
      args[i]["desc_detail"] = markdown.markdown(p["desc_detail"])
    if "type" in p:
      for type_path in type_paths:
        type_file = os.path.join(type_path, p["type"] + ".yaml")
        if os.path.exists(type_file):
          args[i] = {**p, **yaml.load(open(type_file))}
    for k in args[i]:
      if isinstance(args[i][k], str) and "name" in p:
        args[i][k] = args[i][k].replace("{{name}}", p["name"])

  page["arguments"] = args
  page["desc"] = markdown.markdown(page["desc"])
  page["has_examples"] = len(page.get("examples", [])) > 0

  return TPL.render(tpl, page)

def build_page_md(page):
  source = Path(page["file"]).read_text()
  html = markdown.markdown(source, extensions=["tables", "def_list"])
  html = re.sub(
      r'{%([^%]+)%}',
      lambda m: Path(os.path.join(os.environ["output_dir"], m[1])).read_text(),
      html)

  return html

def build_page(page):
  url = page["url"]
  title = page.get("title", url)
  html = None

  print("> Building page: %s" % url)

  if "file" in page:
    html = build_page_md(page)

  if page.get("tpl", None) == "cmdref":
    html = build_page_cmdref(page)

  if html:
    write_page(url, html, title=title)

def main():
  pages = []

  for f in glob.glob("*.yaml") + glob.glob("**/*.yaml"):
    page = yaml.load(open(f))
    page.setdefault("file", f)
    page.setdefault("url", "/" + f.replace(".yaml", ""))
    pages.append(page)

  for f in glob.glob("**/*.md") + glob.glob("*.md"):
    if not os.path.exists(f.replace(".md", ".yaml")):
      pages.append({"file": f, "url": "/" + f.replace(".md", "")})

  for page in pages:
    build_page(page)

main()
