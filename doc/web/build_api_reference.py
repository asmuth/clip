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
    inherit_path = p["inherit"][0] + "_ref.yaml"
    if os.path.exists(inherit_path):
      for inherit_section in yaml.load(open(inherit_path))["properties"]:
        if not "inherit" in p:
          break
        for inherit_property in inherit_section["properties"]:
          if inherit_property["name"] == p["inherit"][1]:
            p = inherit_property
            break

  if not "desc_short" in p:
    p["desc_short"] = ""

  if not "desc" in p:
    p["desc"] = p["desc_short"]

  if not "name" in p:
    p["name"] = p["prop"]

  if "desc" in p:
    p["desc"] = markdown.markdown(p["desc"])
  else:
    p["desc"] = ""

  if "desc_detail" in p:
    p["desc_detail"] = markdown.markdown(p["desc_detail"])
  else:
    p["desc_detail"] = ""

  return p

def build_elem_page(elem_file):
  elem = yaml.load(open(elem_file))

  props_all = []
  props_new = []
  for section in elem["properties"]:
    section_new = {
      "title": section["title"],
      "anchor": section.get("anchor", ""),
      "properties": []
    }

    section_props = section.get("properties", [])
    if "inherit" in section:
      section_inherit_path = section["inherit"][0] + "_ref.yaml"
      if os.path.exists(section_inherit_path):
        for section_inherit in yaml.load(open(section_inherit_path))["properties"]:
          if section_inherit["anchor"] == section["inherit"][1]:
            section_props += section_inherit["properties"]

    for prop in section_props:
      section_new["properties"].append(load_prop(prop))
      props_all.append(load_prop(prop))

    props_new.append(section_new)

  elem["properties"] = props_new
  elem["properties_all"] = props_all

  if "desc" in elem:
    elem["desc"] = markdown.markdown(elem["desc"])

  if "desc_detail" in elem:
    elem["desc_detail"] = markdown.markdown(elem["desc_detail"])

  url = "/commands/" + elem["name"]

  if "example" in elem:
    elem["example_src"] = Path(os.path.join("doc/examples", elem["example"] + ".clp")).read_text()
    elem["example_img"] = "/examples/" + elem["example"] + ".svg"

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
