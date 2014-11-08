echo ">> building fnordmetric-webui.js"

(
  echo "/* THIS IS AN AUTOMATICALLY GENEREATED FILE --- DO NOT EDIT */"
  cat fnordmetric-webui-layout.js \
      fnordmetric-webui-util.js \
      fnordmetric-webui-tableview.js \
      fnordmetric-webui-metriclist.js \
      fnordmetric-webui-queryeditor.js \
      codemirror.js \
) > fnordmetric-webui.js
