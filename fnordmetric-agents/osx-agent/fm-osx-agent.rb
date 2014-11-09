#!/usr/bin/env ruby
require "timeout"
require "socket"

###############################################################################

# Prefix every metric name with this string
METRIC_NAME_PREFIX = "/osx/"

# Report the statistics every 1 second
INTERVAL = 1.0

###############################################################################

if ARGV.length != 2
  puts "usage: #{$0} <host> <port>"
  puts "  Report OSX system stats to FnordMetric Server via statsd"
  puts ""
  puts "example: #{$0} 127.0.0.1 8125"
  exit 1
end

udp = UDPSocket.new
target_host = ARGV[0]
target_port = ARGV[1].to_i

loop do
  last_run = Time.now.to_f
  samples = []

  # hostname
  hostname = `hostname`.strip

  # uptime
  uptime = `uptime`.strip

  # gather load averages
  if uptime =~ /load averages?: ([0-9]+[,\.][0-9]+)\s+([0-9]+[,\.][0-9]+)\s+([0-9]+[,\.][0-9]+)/
    samples << {
      :metric => "load_avg_1m",
      :value => $1,
      :labels => {
        :host => hostname
      }
    }

    samples << {
      :metric => "load_avg_5m",
      :value => $2,
      :labels => {
        :host => hostname
      }
    }

    samples << {
      :metric => "load_avg_15m",
      :value => $3,
      :labels => {
        :host => hostname
      }
    }
  end

  # send the samples in a single udp packet to FnordMetric server (the combined
  # packet size must be strictly less than or equal to 65535 bytes
  packet = ""
  samples.each do |sample|
    packet << METRIC_NAME_PREFIX + sample[:metric]
    sample[:labels].each do |k,v|
      packet << "[#{k}=#{v}]"
    end
    packet << ":"
    packet << sample[:value].to_s.gsub(",", ".")
    packet << "\n"
  end

  begin
    udp.send packet, 0, target_host, target_port
  rescue Exception => e
    puts e
  end

  # sleep if we completed executing faster than the requested interval
  sleep_for = (last_run + INTERVAL) - Time.now.to_f
  sleep(sleep_for) if sleep_for > 0
end
