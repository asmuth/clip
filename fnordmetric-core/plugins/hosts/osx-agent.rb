#!/usr/bin/env ruby
require "json"
require "net/http"
require 'securerandom'
require "timeout"
require 'uri'

###############################################################################

# Prefix every metric name with this string
METRIC_NAME_PREFIX = "/hosts/"

# Report the statistics every 1 second
INTERVAL = 1.0

###############################################################################

if ARGV.length != 1
  puts "usage: #{$0} <rpc-url>"
  puts "  Report OSX system stats to FnordMetric Server via JSONRPC"
  puts ""
  puts "example: #{$0} http://localhost:8080/rpc"
  exit 1
end

def rpc_call(method, params)
  req = {
    "jsonrpc" => "2.0",
    "method" => method,
    "params" => params,
    "id" => SecureRandom.hex
  }

  begin
    uri = URI.parse(ARGV[0])
    http = Net::HTTP.new(uri.host, uri.port)
    resp = http.post(uri.path, req.to_json, {})

    if resp.code.to_i != 200 || JSON.parse(resp.body).has_key?("error")
      STDERR.puts "RPC failed: " + resp.body
    end
  rescue Exception => e
    STDERR.puts "RPC failed: " + e.to_s
  end
end

loop do
  last_run = Time.now.to_f

  rpc_call "KeyValueService.set", {
    :key => "fnordmetric-hosts-plugin-enabled",
    :value => "true"
  }

  # hostname
  hostname = `hostname`.strip

  # uptime
  uptime = `uptime`.strip.gsub(",", ".")

  # gather load averages
  load_avg = 0.0
  if uptime =~ /load averages?: ([0-9]+[,\.][0-9]+)\s+([0-9]+[,\.][0-9]+)\s+([0-9]+[,\.][0-9]+)/
    load_avg = $1

    rpc_call "MetricService.insertSample", {
      :metric => "load_avg_1m",
      :value => $1,
      :labels => {
        :host => hostname
      }
    }

    rpc_call "MetricService.insertSample", {
      :metric => "load_avg_5m",
      :value => $2,
      :labels => {
        :host => hostname
      }
    }

    rpc_call "MetricService.insertSample", {
      :metric => "load_avg_15m",
      :value => $3,
      :labels => {
        :host => hostname
      }
    }
  end


  rpc_call "GroupsService.addMember", {
    :group => "fnordmetric-hosts-plugin-hostnames",
    :member => hostname
  }

  rpc_call "KeyValueService.set", {
    :key => "fnordmetric-hosts-plugin-hostdata~#{hostname}",
    :value => {
      :uptime => uptime,
      :load_avg => load_avg
    }.to_json
  }

  # sleep if we completed executing faster than the requested interval
  sleep_for = (last_run + INTERVAL) - Time.now.to_f
  sleep(sleep_for) if sleep_for > 0
end
