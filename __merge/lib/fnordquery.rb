module FnordQuery; end

require "eventmachine"
require "json"
require "haml"
require "sinatra"

require "fnordquery/web/web"
require "fnordquery/web/app"

require "fnordquery/backends/redis_backend"
require "fnordquery/acceptor"
require "fnordquery/acceptors/tcp_acceptor"
require "fnordquery/acceptors/udp_acceptor"
require "fnordquery/query"

require "fnordquery/report"
require "fnordquery/report_builder"
require "fnordquery/report_manager"
require "fnordquery/reports/numeric_timeseries_report"
require "fnordquery/reports/categorical_topk_report"