#!/usr/bin/env ruby
require "timeout"
require "socket"

###############################################################################

# Prefix every metric name with this string
METRIC_NAME_PREFIX = "/linux/"

# Report the statistics every 1 second
INTERVAL = 1.0

###############################################################################

if ARGV.length != 2
  puts "usage: #{$0} <host> <port>"
  puts "  Report Linux system stats to FnordMetric Server via statsd"
  puts ""
  puts "example: #{$0} 127.0.0.1 8125"
  exit 1
end

udp = UDPSocket.new
target_host = ARGV[0]
target_port = ARGV[1].to_i

loop do
  last_run = Time.now.to_f
  @samples = []

  # hostname
  @hostname = `hostname`.strip


  # collect CPU load averages

  def report_cpu_load

    if File.exists?("/proc/loadavg")
      loadavg_data = IO::read("/proc/loadavg")

      loadavg = loadavg_data.scan(/([0-9]+[,\.][0-9]+)+/).flatten

      if loadavg.size == 3
        @samples << {
          :metric => "load_avg_1m",
          :value => loadavg[0],
          :labels => {
            :host => @hostname
          }
        }

        @samples << {
          :metric => "load_avg_5m",
          :value => loadavg[1],
          :labels => {
            :host => @hostname
          }
        }

        @samples << {
          :metric => "load_avg_15m",
          :value => loadavg[2],
          :labels => {
            :host => @hostname
          }
        }
      end
    end
  end

  # gather basic memory statistics

  def report_memory_stats
    if File.exists?("/proc/meminfo")
      loadavg_data = IO::read("/proc/meminfo")
      memtotal  = loadavg_data.scan(/MemTotal:\s+ (\d+)\skB/).flatten.first.to_i
      memfree   = loadavg_data.scan(/MemFree:\s+ (\d+)\skB/).flatten.first.to_i
      swaptotal = loadavg_data.scan(/SwapTotal:\s+ (\d+)\skB/).flatten.first.to_i
      swapfree  = loadavg_data.scan(/SwapFree:\s+ (\d+)\skB/).flatten.first.to_i

        @samples << {
          :metric => "memory_total",
          :value => memtotal,
          :labels => {
            :host => @hostname
          }
        }

        @samples << {
          :metric => "memory_free",
          :value => memfree,
          :labels => {
            :host => @hostname
          }
        }

        @samples << {
          :metric => "swap_total",
          :value => swaptotal,
          :labels => {
            :host => @hostname
          }
        }

        @samples << {
          :metric => "swap_free",
          :value => swapfree,
          :labels => {
            :host => @hostname
          }
        }
    end
  end

  # determine disk usage and available space

  def report_disk_stats

    df_input = `df -P`.lines[1..-1]

    if !df_input.empty?
     df_input.each do |single_line|
      elements = single_line.split " "

          @samples << {
          :metric => "disk_used",
          :value => elements[2],
          :labels => {
            :host => @hostname,
            :mount_name => elements[0]
          }
        }

          @samples << {
          :metric => "disk_available",
          :value => elements[3],
          :labels => {
            :host => @hostname,
            :mount_name => elements[0]
          }
        }
     end
    end
  end

  #count open TCP and UDP sockets

  def report_open_sockets

    if File.exists?("/proc/net/tcp")
     tcp_sockets =  %x{wc -l "/proc/net/tcp"}.to_i - 1

      @samples << {
        :metric => "open_tcp_sockets",
        :value => tcp_sockets,
        :labels => {
          :host => @hostname
        }
      }
    end

    if File.exists?("/proc/net/udp")
     tcp_sockets =  %x{wc -l "/proc/net/udp"}.to_i - 1

      @samples << {
        :metric => "open_udp_sockets",
        :value => tcp_sockets,
        :labels => {
          :host => @hostname
        }
      }


    end
  end

  # fill samples array with data

  report_memory_stats
  report_disk_stats
  report_cpu_load
  report_open_sockets

  # send the samples in a single udp packet to FnordMetric server (the combined
  # packet size must be strictly less than or equal to 65535 bytes
  packet = ""
  @samples.each do |sample|
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
