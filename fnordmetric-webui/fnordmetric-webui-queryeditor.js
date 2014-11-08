/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Laura Schlimmer
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
if (FnordMetric === undefined) {
  FnordMetric = {};
}

if (FnordMetric.views === undefined) {
  FnordMetric.views = {};
}

FnordMetric.views.QueryPlayground = function() {
  function parseMilliTS(ts) {
    if (ts < 1000) {
      if (ts == 0) {
        return " less than 1 millisecond";
      } else if (ts == 1) {
        return " 1 millisecond";
      } else {
        return ts + " milliseconds";
      }
    } else if (ts < 60000) {
      ts = ts / 1000;
    return (ts + (ts == 1? " second" : " seconds"));
    } else {
      ts = ts / 60000;
      return (ts + (ts == 1? " minute" : " minutes"));
    }
  }

  function getRowsInfo(tables) {
    var num = 0;
    tables.map(function(table) {
      num += table.rows.length;
    });
    return (num == 1? num + " row" : num + " rows")
  }

  var horizontal = true;
  var editor_width = 42;
  var editor_height = 300;
  var height;
  var navbar;
  var editor_pane;
  var result_pane;
  var empty_text;
  var editor_resizer_tooltip;
  var split_button;
  var query_button;
  var embed_button;
  var query_editor;
  var visual_editor;
  var visualIsOn = true;
  var sql_editor;
  var visualHoverTtp;
  var cm;

  var initEditor = function() {
    query_editor = document.createElement("div");
    query_editor.className = "card editor";

    editor_pane = document.createElement("div");
    editor_pane.className = "editor_pane";

    empty_text = document.createElement("p");
    empty_text.innerHTML = "Text Text Insert your query on the left ... \n " +
      "Example: DRAW POINTCHART AXIS LEFT AXIS BOTTOM; SELECT 'fu' as series,\n "+
      "time AS x, value as y FROM http_status_codes;";

    result_pane = document.createElement("div");
    result_pane.className = "result_pane";
    result_pane.style.background = "#fff";
    result_pane.style.borderLeft = "1px solid #ddd";
    result_pane.appendChild(empty_text);

    editor_resizer_tooltip = document.createElement("div");
    editor_resizer_tooltip.className = "editor_resizer_tooltip";
    editor_resizer_tooltip.setAttribute('draggable', 'true');

    function createTabBar() {
      var tabbar = document.createElement("div");
      tabbar.className = "editor_navbar";

      function createTabHoverText(inner_text) {
        var textbox = document.createElement("div");
        textbox.className = "hover_tooltip";
        textbox.innerHTML = inner_text;
        tabbar.appendChild(textbox);
        return textbox;
      }

      function createTab(inner_text, callback) {
        var tab = FnordMetric.createButton(
          "#", undefined, "<h3>"+inner_text+"</h3>");
        tabbar.appendChild(tab);

        return tab;
      }
      visual_editor = createTab("Visual");
      sql_editor = createTab("SQL");
      visualHoverTtp = createTabHoverText(
        "The visual editor has been "+
        "disabled because you modified the ChartSQL query");
      editor_pane.appendChild(tabbar);
    }

    function createNavbar() {
      navbar = document.createElement("div");
      navbar.className = "navbar";

      var createFancyButton = function(text, href, floatdir) {
        var button = document.createElement("div");
        button.className = "fancy_button";
        button.style.margin = "10px";
        button.style.float = floatdir;
        var button_link = FnordMetric.createButton(href, undefined, text);
        button.appendChild(button_link);
        navbar.appendChild(button);
        return button;
      }

      split_button = createFancyButton("Change View", document.URL, "");
      query_button = createFancyButton("Run Query", '#', "left");
      embed_button = createFancyButton("Embed This Query", '#', "right");
      var textfield = document.createElement("div");
      textfield.innerHTML = "You can submit the query pressing CTRL Enter";
      textfield.style.margin = "10px";
      textfield.style.float = "left";
      navbar.appendChild(textfield);
    }
    createTabBar();
    editor_pane.appendChild(query_editor);
    createNavbar();
  }

  var initCM = function() {

    var tab_width = 4;
    var font = "14px monospace";
    var patterns = [
      {
        regex: /^(SELECT|FROM|WHERE|GROUP|ORDER|BY|HAVING|LIMIT|OFFSET|ASC|DESC|COMMA|DOT|IDENTIFIER|STRING|NUMERIC|SEMICOLON|LPAREN|RPAREN|AND|OR|EQUAL|PLUS|MINUS|ASTERISK|SLASH|NOT|TRUE|FALSE|BANG|CIRCUMFLEX|TILDE|PERCENT|DIV|MOD|AMPERSAND|PIPE|LSHIFT|RSHIFT|LT|GT|BEGIN|CREATE|WITH|IMPORT|TABLE|ON|OFF|DRAW|LINECHART|AREACHART|BARCHART|POINTCHART|HEATMAP|HISTOGRAM|AXIS|TOP|RIGHT|BOTTOM|LEFT|ORIENTATION|HO  RIZONTAL|VERTICAL|STACKED|XDOMAIN|YDOMAIN|ZDOMAIN|XGRID|YGRID|LOGARITHMIC|INVERT|TITLE|SUBTITLE|GRID|LABELS|TICKS|INSIDE|OUTSIDE|ROTATE|LEGEND)$/i,
        color: "#d33682",
      },
      {
        regex: /AS/i,
        color: "#6c71c4"
      }
    ];

    cm = CodeMirror(query_editor, {
      lineNumbers: true,
    });

    //FIXME ?
    cm.on('change', function(cmirror) {
      var cm_val = cm.getValue();
      if (cm_val.length == 0 || (/^\s*$/).test(cm_val) ) {
        if (!visualIsOn) {
          visual_editor.firstChild.style.color = "#444";
          visualIsOn = true;
        }

      } else {
        if (visualIsOn) {
          visualIsOn = false;
          visual_editor.firstChild.style.color = "#ddd";
        }
      }
    });
  }

  initEditor();

  var updateLayout = function(tooltip, viewport) {
    if (horizontal) {
      if (viewport !== undefined) {
        viewport.className = "viewport horizontal_split";
      }
      result_pane.style.height = "auto";

      var initial_height =  (window.innerHeight - 68) / 1.2;
      var editor_height = (document.querySelector(
        ".editor_pane")).offsetHeight;
      editor_height = Math.max(initial_height, editor_height);
      var result_height = (document.querySelector(
        ".result_pane")).offsetHeight;
      height = Math.max(editor_height, result_height);

      query_editor.className = "query_editor";
      editor_pane.style.width = editor_width + "%";
      editor_pane.style.float = "left";
      result_pane.style.height = height + "px";
      result_pane.style.width = (99 - editor_width) + "%";
      result_pane.style.left = editor_width + "%";
      result_pane.style.top = "";
      result_pane.style.overflowY = "auto";
      editor_resizer_tooltip.style.left = (editor_pane.offsetWidth) + "px";
      editor_resizer_tooltip.style.top = editor_pane.offsetTop + "px";
      editor_resizer_tooltip.style.height = height + "px";
      if (cm !== undefined) {cm.setSize("auto", height);}

    } else {
      if (viewport !== undefined) {
        viewport.className = "viewport vertical_split";
      }
      if (!tooltip) {
        editor_height = (cm.lineCount() * 30 + 60);
      }
      query_editor.className = "query_editor";
      editor_pane.style.float = "";
      editor_pane.style.width = "100%";
      editor_pane.style.height = editor_height + "px";
      query_editor.style.height = editor_height + "px";
      result_pane.style.width = (window.innerWidth - 55) + "px";
      result_pane.style.left = "20px";
      result_pane.style.top = (editor_pane.offsetTop + editor_height) + "px";
      result_pane.style.height = "auto";
      editor_resizer_tooltip.style.top = (result_pane.offsetTop - 3) + "px";
      editor_resizer_tooltip.style.left = "20px";
      editor_resizer_tooltip.style.right = "20px";
      editor_resizer_tooltip.style.height = "6px";
      if (cm !== undefined) {cm.setSize("auto", editor_height + "px");}
    }
  }


  var visualEditor = function() {
    var query = {
      metric: "", 
      aggregation: {type: "", step: "", time: ""},
      group_by: [],
      chart: {type: "", color: ""}};

    function destroyCM() {
      cm = undefined;
      var cm_pane = query_editor.querySelector(".CodeMirror");
      if (cm_pane === null) {return;}
      while (cm_pane.firstChild) {
        cm_pane.removeChild(cm_pane.firstChild);
      }
      query_editor.removeChild(cm_pane);
    }

    function initPane() {
      if (horizontal) {
        query_editor.style.height = (height - 29) + "px";
      } else{
          //set query_editor height
      }
    }

    var draw_dropdown;
    var aggr_dropdown;
    var color_dropdown;
    var step_input;
    var time_input;
    var metric;

    var render = function(url_query) {
      destroyCM();
      initPane();
      function setQueryValues() {

      }

      console.log("query in visual editor " + url_query);
      function renderFieldTitle(title_text, parentNode) {
         var title = document.createElement("div");
        title.className = "field_title";
        title.innerHTML = title_text;
        if (parentNode === undefined) {return title;}
        parentNode.appendChild(title);
      }

      function createField() {
        var field = document.createElement("div");
        field.className = "visual_editor field";
        return field;
      }

      function createDropdown(values) {
        var dropdown = document.createElement("select");
        values.map(function(value) {
          var option = document.createElement("option");
          option.value = value;
          option.innerHTML = value;
           dropdown.appendChild(option);
        });
        return dropdown;
      }

      function renderSelectButton(inner_text, parentNode) {
        var button = FnordMetric.createButton(
          "#", "simple_button", inner_text);
        parentNode.appendChild(button);

        button.addEventListener('click', function(e) {
          e.preventDefault();
          if (this.className == "simple_button") {
            this.className = "simple_button active";
          } else {
            this.className = "simple_button";
          }
          query.group_by.push(inner_text);
        }, false);
      }


      var metric_field = createField();
      metric = document.createElement("input");
      metric.value = FnordMetric.MetricData[0] !== undefined ?
      FnordMetric.MetricData[0].key : "";
      var metric_list = document.createElement("ul");
      metric_list.className = "dropdown";
      var keys = [];
      FnordMetric.MetricData.map(function(metric) {
        keys.push(metric.key);
      });
      metric_field.appendChild(metric);

      FnordMetric.DropdownAutocomplete(
        metric_field, metric_list, metric, keys, undefined);

      var aggr_field = createField();
      var aggr_title = renderFieldTitle("AGGREGATION", aggr_field);

      aggr_dropdown = createDropdown(
        ["", "SUM", "AVG", "COUNT", "MAX", "MIN"]);
      aggr_field.appendChild(aggr_dropdown);

      var step_title = renderFieldTitle("STEP");
      step_title.style.marginLeft = "20px";
      aggr_field.appendChild(step_title);

      step_input = document.createElement("input");
      step_input.className = "small";
      aggr_field.appendChild(step_input);
      var time_title = renderFieldTitle("TIME");
      time_title.style.marginLeft = "20px";
      aggr_field.appendChild(time_title);

      time_input = document.createElement("input");
      time_input.className = "small";
      aggr_field.appendChild(time_input);


      var group_field = createField();
      var group_title = renderFieldTitle("GROUP BY", group_field);

      var columns = ["Key", "Labels", "Last Insert", "Total Stored Bytes"];
      columns.map(function(column) {
          renderSelectButton(column, group_field);
      });


      var draw_field = createField();
      renderFieldTitle("DRAW AS", draw_field);

      draw_dropdown = createDropdown(["LINECHART", 
        "POINTCHART","BARCHART", "AREACHART"]);
      draw_field.appendChild(draw_dropdown);

      var color_ttl = renderFieldTitle("COLOR", undefined);
      color_ttl.style.marginLeft = "20px";
      draw_field.appendChild(color_ttl);

      color_dropdown = createDropdown(
        ["Blue", "Red", "Green", "Purple"]);
      draw_field.appendChild(color_dropdown);


      query_editor.appendChild(metric_field);
      query_editor.appendChild(aggr_field);
      query_editor.appendChild(group_field);
      query_editor.appendChild(draw_field);
    }

    var buildQuery = function() {
      var querystring;
      function getQueryInput() {
        query.metric = metric.value;
        query.chart.type = draw_dropdown.value;
        query.chart.color = curr_color.id;
        query.aggregation.type = aggr_dropdown.value;
        query.aggregation.step = step_input.value;
        query.aggregation.time = time_input.value;
        query.chart.color = color_dropdown.value;
      }

      function buildQueryString() {
        querystring = "DRAW " + query.chart.type +";";
        querystring += " SELECT ";
        if (query.aggregation.type.length > 0) {
          querystring += query.aggregation.type;
        }
        if (query.aggregation.step.length > 0 && 
            query.aggregation.time.length >0) {
              querystring += " GROUP OVER TIMEWINDOW("+
              query.aggregation.time + ", " +
              query.aggregation.step + ")";
        }

        querystring += " FROM " + query.metric;
        querystring += ";"
        console.log(query.aggregation);
        console.log(querystring);
      }
      getQueryInput();
      buildQueryString();
      return querystring;
    }
      //updateLayout(false, undefined);

    return {
      "render" : render,
      "buildQuery" : buildQuery
    }
  }

  var plainEditor = function() {
    var render = function(query) {
      function initQueryEditor() {
        while (query_editor.firstChild) {
          query_editor.removeChild(query_editor.firstChild)
        }
      }

      initQueryEditor();
      initCM();

      if (query !== undefined) {
        cm.setValue(query);
      }
      updateLayout();
    }

    var buildQuery = function() {
      return (cm.getValue());
    }
    return {
      "render" : render,
      "buildQuery" : buildQuery
    }
  }


  var render = function(elem, query) {
    var current_editor;
    elem.appendChild(navbar);
    elem.appendChild(editor_pane);
    elem.appendChild(editor_resizer_tooltip);
    elem.appendChild(result_pane);

    visual_editor.addEventListener('click', function(e) {
      e.preventDefault();
      if (visualIsOn) {
        this.className = "active";
        sql_editor.className = "";
        current_editor = visualEditor();
        renderEditor();
      } else {
        alert("display nice box: You will loose your query. Do you want to proceed anyway? ... ");
      }
    }, false);

    visual_editor.addEventListener('mouseover', function() {
      if (!visualIsOn) {
        visualHoverTtp.style.display = "block";
      }
    }, false);

    visual_editor.addEventListener('mouseout', function() {
      visualHoverTtp.style.display = "none";
    }, false);

    sql_editor.addEventListener('click', function(e) {
      e.preventDefault();
      this.className = "active";
      visual_editor.className = "";
      current_editor = plainEditor();
      renderEditor();
    }, false);

    query_button.addEventListener('click', function(e) {
      e.preventDefault();
      var query = current_editor.buildQuery();
      runQuery(query);
    }, false);

    embed_button.addEventListener('click', function(e) {
      e.preventDefault();
      openEmbedPopup();
    }, false);

    split_button.addEventListener('click', function(e) {
      e.preventDefault();
      horizontal = !horizontal;

      if (!horizontal) {
        editor_height = window.innerheight * 0.3;
      }

      updateLayout(false, elem);
    }, false);

    editor_resizer_tooltip.addEventListener('drag',function (e) {
      editor_resizer_tooltip.style.background = "";
      if (horizontal && e.clientX > 0) {
        editor_width = (e.clientX / window.innerWidth) * 100;i
        editor_width = Math.min(Math.max(25, editor_width), 60);
        updateLayout(true);
      }

      if (!horizontal && e.clientY > 0) {
        editor_height = (e.clientY + window.pageYOffset) - editor_pane.offsetTop;
        editor_height = Math.max(100, editor_height);
        updateLayout(true);
      }
    }, false);

    editor_resizer_tooltip.addEventListener('dragstart', function(e) {
      this.style.background = "transparent";
    }, false);

    window.addEventListener('dragover', function(e) {
      e.preventDefault();
    }, false);

    window.addEventListener('resize', function() {
      updateLayout(false, elem);
    }, true);

    window.addEventListener('keydown', function(e) {
      if (e.ctrlKey && e.keyCode == 13) {
        e.preventDefault();
        var query = cm.getValue();
        if (query) {runQuery(query)};
      }
    }, false);

    var renderEditor = function() {
      if (current_editor === undefined) {
        current_editor = plainEditor();
        sql_editor.className = "active";
      }

      if (query == "undefined" || query === undefined) {
        current_editor.render();
      } else {
        current_editor.render(query);
        runQuery(query);
      }
    };
    renderEditor();
    updateLayout(false, elem);
  };

  var destroy = function(viewport) {
    if (viewport) {
      while (viewport.firstChild) {
        viewport.removeChild(viewport.firstChild);
      }
    } else {
      while (result_pane.firstChild) {
        result_pane.removeChild(result_pane.firstChild);
      }
      if (editor_pane.lastChild.className == "info_field") {
        editor_pane.removeChild(editor_pane.lastChild);
      }
    }
  };

  var renderTable = function(table) {
    var rows = table.rows;
    var columns = table.columns;
    result_table = document.createElement("table");
    result_table.className = "result_table";
    result_table.setAttribute("id", "result_table");
    var table_header = document.createElement("tr");
    columns.map(function(column) {
      var table_header_cell = document.createElement("th");
      table_header_cell.innerHTML = column;
      table_header.appendChild(table_header_cell);
    });

    result_table.appendChild(table_header);

    var renderResultTableTooltip = function(rows, rows_per_side) {
      var start_index = 0;
      var end_index = rows_per_side;
      var table_navbar = document.createElement("div");
      table_navbar.className = "pagination_navbar";
      table_navbar.id = "table_navbar";

      var tooltipProp = {
        "for" : {
          "arrow" : "&#8594;",
          "marginRight" : "0px"
        },
        "back" : {
          "arrow" : "&#8592;",
          "marginRight" : "2px;"
        }
      }

      var tooltipEvent = function() {
        updateNavbarTooltips();
        destroyResultTableRows();
        renderResultTableRows(start_index, end_index);
      }

      var createTooltip = function(type) {
        var tooltip = FnordMetric.createButton(
          "#", "pagination_tooltip", tooltipPrp[type]["arrow"]);
        tooltip.style.marginRight = tooltipProp[type]["marginRight"];

        tooltip.addEventListener('click', function(e) {
          e.preventDefault();
          if (type == "for") {
            start_index = parseInt(this.id);
            end_index = Math.min(rows.length, 
              start_index + rows_per_side);

          } else {
            start_index = Math.max(0, 
              parseInt(this.id) - rows_per_side);
            end_index = start_index + rows_per_side;
          }
          tooltipEvent();
        }, false)

        return tooltip;
      }


      var navbar_label = document.createElement("div");
      navbar_label.className = "navbar_label";

      var updateNavbarLabel = function() {
        navbar_label.innerHTML = "<b>" + (start_index +1) + 
          "</b><span> - </span><b>" + end_index + 
          "</b><span> of </span><b>" + rows.length + "</b>";
      }

      var updateNavbarTooltips = function() {
        tooltip_for.setAttribute("id", end_index);
        tooltip_back.setAttribute("id" , start_index);

        tooltip_for.style.color = 
          (end_index == rows.length) ? "#ddd" : "#444";

        tooltip_back.style.color = 
          (start_index == 0) ? "#ddd" : "#444";
      }

      var tooltip_for = createTooltip("for");
      var tooltip_back = createTooltip("back");

      updateNavbarLabel();
      updateNavbarTooltips();

      table_navbar.appendChild(tooltip_for);
      table_navbar.appendChild(tooltip_back);
      table_navbar.appendChild(navbar_label);
      result_pane.appendChild(table_navbar);
   }


    var renderResultTableRows = function(start, end) {
      for (var i = start; i < end; i++) {
        var row = document.createElement("tr");
        rows[i].map(function(celldata) {
          var cell = document.createElement("td");
          cell.innerHTML = celldata;
          row.appendChild(cell);
        });
        result_table.appendChild(row);
      }
      return false;
    }

    var destroyResultTableRows = function() {
      while(result_table.childNodes.length > 1) {
        result_table.removeChild(result_table.lastChild);
      }
    }

    var rows_per_side = 5;
    if (table.rows.length > rows_per_side) {
      renderResultTableTooltip(table.rows, rows_per_side);
      renderResultTableRows(0, rows_per_side);
    } else {
      renderResultTableRows(0, table.rows.length);
    }

    result_pane.appendChild(result_table);
  }


  var renderChart = function(chart) {
    var chart_container = document.createElement("div");
    chart_container.className = "chart_container";
    chart_container.setAttribute("id", "chart_container");
    if (chart != undefined) {
      chart_container.innerHTML = chart.svg;
    }
    result_pane.appendChild(chart_container);

    return false;
  }


  var destroyResult = function(type) {
    var typeNodes = {
      "chart" : ["chart_container"],
      "table" : ["result_table", "table_navbar"]
    }

    var parentElems = typeNodes[type];
    for (var i = 0; i < parentElems.length; i++) {
      var parentNode = document.getElementById(parentElems[i]);
      while (parentNode.firstChild) {
        parentNode.removeChild(parentNode.firstChild);
      }
      result_pane.removeChild(parentNode);
    }
  }


  var renderError = function(msg) {
    result_pane.style.background = "#fff";
    result_pane.style.borderLeft = "1px solid #ddd";
    var error_msg = document.createElement("div");
    error_msg.style.padding = "20px";
    error_msg.innerHTML = msg;
    result_pane.appendChild(error_msg);
  }

  var renderResultPane = function(resp, duration) {
    if (resp.status == "error") {
      renderError(resp.error);
      return;
    }

    var charts = resp.charts;
    var tables = resp.tables;
    var curr_chart;
    var curr_table;
    var curr_chartID = 0;
    var curr_tableID = 0;
    var curr_url = document.URL;



    var outputObj = {
      chart : {
        label : "Chart",
        currResult : curr_chart,
        resultData : charts
      },
      table : {
        label : "Table",
        currResult : curr_table,
        resultData : tables
      }
    }

    var renderExecutionInfo = function() {
      var info_field = document.createElement("div");
      info_field.className = "info_field";
      info_field.innerHTML =
        "Query execution took " + parseMilliTS(duration) 
        + " and returned " + getRowsInfo(tables);
      editor_pane.appendChild(info_field);
    }

    var renderResultNavbar = function(type, quantity) {
      var result_navbar = document.createElement("div");
      result_navbar.className = "result_navbar";
      for (var i = 0; i < quantity; i++) {
        var navitem = FnordMetric.createButton(
          curr_url, 
          "result_link",
          "<h3>" + outputObj[type]["label"] + " " + (i+1) + "</h3>");
        navitem.setAttribute("id", i);
        result_navbar.appendChild(navitem);

        navitem.addEventListener('click', function(e) {
          e.preventDefault();
          if (this.id != outputObj[type][currResult]) {
            updateResult(type, this);
          }
        }, false);
      }
      result_pane.appendChild(result_navbar);
    }


    var updateResultNavbar = function(type, new_id, old_id) {
      //get new_item
      //set background color for new item
      if (old_id >= 0) {
        //get old_item and set background color to white
      }
    }

    var updateResult = function(type, elem) {
      destroyResult(type);
      updateNavbar(elem, outputObj[type][currResult]);
      if (type == "chart") {
        curr_chartID = elem.id;
      } else {
        curr_tableID = elem.id;
      }
      renderChart(outputObj[type][resultData][elem.id]);
      updateLayout(false);

    }

    renderExecutionInfo();
    if (charts !== undefined) {
      renderResultNavbar("chart", charts.length, curr_chart);
      renderChart(charts[curr_chartID]);
    }
    if (tables !== undefined) {
      renderResultNavbar("table", tables.length, curr_table);
      renderTable(tables[curr_tableID]);
    }

    updateResultNavbar("chart", curr_chartID, -1);
    updateResultNavbar("table", curr_tableID, -1); 

  }

  var runQuery = function(query) {
    if (query === undefined) {
      var query = cm.getValue();
    }
    FnordMetric.Loading().render();
    //cm.setValue(query);
    var encoded_query = encodeURIComponent(query);
    var url = "/admin#query_playground!" + encoded_query;
    window.history.pushState({url: url}, "", "#" + url);
    FnordMetric.httpPost("/query", query, function(r, duration) {
      FnordMetric.Loading().destroy();
      window.location.href = url;
      if (r.status == 200 && r.statusText == "OK") {
        var res = JSON.parse(r.response);
        destroy();
        renderResultPane(res, duration);
        updateLayout(false);
      } else {
        renderError(r);
      }
    });
  }

  var openEmbedPopup = function() {
    var query = cm.getValue();
    if (query.length == 0) {
      //FIXME
      alert("Please enter a query");
      return;
    }
    var renderPopup = function() {
      var foreground = document.createElement("div");
      foreground.className = "load_foreground";
      var popup = document.createElement("div");
      popup.className = "popup";
      var close_button = FnordMetric.createButton(
        "#", undefined, "X");
      //FIXME & make me fancy
      popup.innerHTML = "Ruby/html/js snippet ";
      popup.appendChild(close_button);

      document.body.appendChild(foreground);
      document.body.appendChild(popup);

      close_button.addEventListener('click', function(e) {
        e.preventDefault();
        document.body.removeChild(popup);
        document.body.removeChild(foreground);
      }, false);
    }

    renderPopup();
  } 

  return {
    "render": render,
    "destroy": destroy,
    "name": "query_playground"
  };
}


