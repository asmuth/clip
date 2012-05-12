module FnordQuery; end

require "eventmachine"

require "fnordquery/backends/redis_backend"
require "fnordquery/acceptor"
require "fnordquery/acceptors/tcp_acceptor"
require "fnordquery/acceptors/udp_acceptor"