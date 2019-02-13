#!/usr/bin/env python3
import xml.etree.ElementTree as ET
import pystache as TPL
from build_layout import *
import markdown
import yaml
import glob
import os

def load_prop(p):
  if "inherit" in p:
    path = "manual/prop_%s.yaml" % p.get("inherit", "")
    if os.path.exists(path):
      p = {**yaml.load(open(path)), **p}

  if not "desc_short" in p:
    p["desc_short"] = ""

  if not "name" in p:
    p["name"] = p["prop"]

  return p

def build_elem_page(elem_file):
  elem = yaml.load(open(elem_file))
  elem["properties"] = list(map(lambda p: load_prop(p), elem["properties"]))
  url = "/documentation/elements/" + elem["name"]
  print("> Building page: %s" % url)
  title = elem["name"]
  tpl = Path("extra/web/reference_detail.tpl.html").read_text()
  html = TPL.render(tpl, elem)
  write_page(url, html, title=title)

def main():
  elems = glob.glob("manual/elem_*.yaml")

  for elem in elems:
    build_elem_page(elem)

main()
