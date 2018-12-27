#!/usr/bin/env python3
import xml.etree.ElementTree as ET
import pystache as TPL
from build_layout import *

def build_prop(parent):
  tpl = """
    <section class="api_ref">
      {{{elem_name}}}
      <table>
        <tbody>
          {{#props}}
            <tr>
              <td width="40%"><code><strong>{{name}}</strong></code></td>
              <td>Description goes here</td>
            </tr>
          {{/props}}
        </tbody>
      </table>
    </section>
  """

  if parent.attrib["path"] == "global":
    elem_name = "<h2>Global Properties</h2>"
  else:
    elem_name = TPL.render(
        "<h2>Element <code>{{name}}</code></h2>",
        {
          'name': parent.attrib["path"]
        })

  if "cond" in parent.attrib:
    elem_name += TPL.render(
        "<h2 class=sub>where <code>{{cond}}</code></h2>",
        {
          'cond': parent.attrib["cond"]
        })

  props = []
  for prop in parent.findall("./prop"):
    props.append({
      "name": prop.attrib["name"]
    })

  return TPL.render(tpl, {
    "props": props,
    "elem_name": elem_name
  })

def main():
  tree = ET.parse('manual/properties.xml')
  root = tree.getroot()
  html = ""

  for prop in root.findall("./elem"):
    html += build_prop(prop)

  tpl = """
    <h1>API Reference</h1>
    {{{html}}}
  """

  print(build_layout(TPL.render(tpl, {'html': html})))

main()
