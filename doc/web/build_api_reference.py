#!/usr/bin/env python3
import xml.etree.ElementTree as ET
import pystache as TPL
from build_layout import *
import markdown
import yaml
import glob
import os

def load_arg(p):
  if "type" in p:
    type_path = "doc/commands/arg/" + p["type"] + ".yaml"
    if os.path.exists(type_path):
      p = {**p, **yaml.load(open(type_path))}

  if not "desc_short" in p:
    p["desc_short"] = ""

  if not "desc" in p:
    p["desc"] = p["desc_short"]

  if not "name" in p:
    p["name"] = p["arg"]

  if "desc" in p:
    p["desc"] = markdown.markdown(p["desc"])
  else:
    p["desc"] = ""

  if "desc_detail" in p:
    p["desc_detail"] = markdown.markdown(p["desc_detail"])
  else:
    p["desc_detail"] = ""

  for k in p:
    if isinstance(p[k], str):
      p[k] = p[k].replace("{{name}}", p["name"])

  return p

def build_elem_page(elem_file):
  elem = yaml.load(open(elem_file))

  args = []
  for arg in elem.get("arguments", []):
    if "include" in arg:
      include = yaml.load(open("doc/commands/arg/" + arg["include"] + ".yaml"))
      for arg in include["arguments"]:
        args.append(load_arg(arg))
    else:
      args.append(load_arg(arg))

  elem["arguments"] = args

  if "desc" in elem:
    elem["desc"] = markdown.markdown(elem["desc"])

  if "desc_detail" in elem:
    elem["desc_detail"] = markdown.markdown(elem["desc_detail"])

  url = "/commands/" + elem["name"]

  print("> Building page: %s" % url)
  title = elem["name"]
  tpl = Path("doc/web/reference_detail.tpl.html").read_text()
  html = TPL.render(tpl, elem)
  write_page(url, html, title=title)

def main():
  elems = glob.glob("doc/commands/*.yaml")

  for elem in elems:
    build_elem_page(elem)

main()
