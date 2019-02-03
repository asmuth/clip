#!/usr/bin/env python3
import xml.etree.ElementTree as ET
import pystache as TPL
from build_layout import *
import markdown
import yaml
import glob

def build_elem_page(elem_file):
  elem = yaml.load(open(elem_file))
  url = "/elements/" + elem["name"]
  print("> Building page: %s" % url)
  title = "%s | API Reference" % (elem["name"])
  tpl = Path("extra/web/reference_detail.tpl.html").read_text()
  html = TPL.render(tpl, elem)
  write_page(url, html, title=title)

def main():
  elems = glob.glob("manual/elem_*.yaml*")

  for elem in elems:
    build_elem_page(elem)

main()
