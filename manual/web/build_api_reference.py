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

  if not "desc" in p:
    p["desc"] = p["desc_short"]

  if not "name" in p:
    p["name"] = p["prop"]

  if "desc" in p:
    p["desc"] = markdown.markdown(p["desc"])

  if "desc_detail" in p:
    p["desc_detail"] = markdown.markdown(p["desc_detail"])

  return p

def build_elem_page(elem_file):
  elem = yaml.load(open(elem_file))

  props_new = []
  for section in elem["properties"]:
    section_new = {
      "title": section["title"],
      "anchor": section.get("anchor", ""),
      "properties": []
    }

    for prop in section["properties"]:
      section_new["properties"].append(load_prop(prop))
    props_new.append(section_new)

  elem["properties"] = props_new

  if "desc" in elem:
    elem["desc"] = markdown.markdown(elem["desc"])

  url = "/documentation/elements/" + elem["name"]

  if "example" in elem:
    elem["example_src"] = Path(os.path.join("examples", elem["example"] + ".fvz")).read_text()
    elem["example_img"] = "/examples/" + elem["example"] + ".svg"

  print("> Building page: %s" % url)
  title = elem["name"]
  tpl = Path("manual/web/reference_detail.tpl.html").read_text()
  html = TPL.render(tpl, elem)
  write_page(url, html, title=title)

def main():
  elems = glob.glob("elements/**/*_ref.yaml")

  for elem in elems:
    build_elem_page(elem)

main()
