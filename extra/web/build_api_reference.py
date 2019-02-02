#!/usr/bin/env python3
import xml.etree.ElementTree as ET
import pystache as TPL
from build_layout import *
import markdown
import yaml

def build_prop_overview(parent):
  tpl = """
    <section class="api_ref">
      {{{elem_name}}}
      <div class="plist">
        {{#props}}
          <a href="/reference/{{scope}}/{{name}}" class="prop">
            <span><code><strong>{{name}}</strong></code></span>
            <span>{{desc}}</span>
          </a>
        {{/props}}
      </div>
    </section>
  """

  if parent["path"] == "global":
    elem_name = "<h2>Global Properties</h2>"
  else:
    elem_name = TPL.render(
        "<h2>Element <code>{{name}}</code></h2>",
        {
          'name': parent["path"]
        })

  if "cond" in parent:
    elem_name += TPL.render(
        "<h2 class=sub>where <code>{{cond}}</code></h2>",
        {
          'cond': parent["cond"]
        })

  props = []
  for prop in parent["properties"]:
    props.append({
      "scope": parent["id"],
      "name": prop["name"],
      "desc": prop["desc_short"] if "desc_short" in prop else "",
    })

  return TPL.render(tpl, {
    "props": props,
    "elem_name": elem_name
  })

def build_prop_detail(parent, prop):
  url = parent["id"] + "/" + prop["name"]
  print("> Building page: /reference/%s" % url)

  title = "%s | %s | API Reference" % (prop["name"], parent["path"])
  tpl = Path("extra/web/reference_detail.tpl.html").read_text()
  path = "/reference/" + url
  env = {
    "name": prop["name"],
    "description": markdown.markdown(prop.get("desc", prop["desc_short"])),
    "demo": prop.get("demo", ""),
    "syntax_formal": prop.get("syntax_formal", ""),
    "syntax_example": prop.get("syntax_example", ""),
    "values": prop.get("values", []),
    "default": markdown.markdown(prop.get("default", "")),
    "scope": markdown.markdown(prop.get("scope", "")),
    "scope_example": prop.get("scope_example", ""),
  }

  html = TPL.render(tpl, env)
  write_page(path, html, title=title)

def main():
  tree = yaml.load(open('manual/properties.yml'))
  html = ""

  for elem in tree["elements"]:
    html += build_prop_overview(elem)

    for prop in elem["properties"]:
      build_prop_detail(elem, prop)

  tpl = """
    <h1>API Reference</h1>
    {{{html}}}
  """

  write_file("/_reference.gen.html", html)


main()
