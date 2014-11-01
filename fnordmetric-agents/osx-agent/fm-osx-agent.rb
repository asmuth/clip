#!/usr/bin/env ruby
require "timeout"
require "uri"
require "net/http"


metric_prefix = "/osx"

loop do
  samples = []

  # hostname
  hostname = `hostname`.strip

  # uptime
  uptime = `uptime`.strip

  # load averages
  if uptime =~ /load averages?: ([0-9]+[,\.][0-9]+)\s+([0-9]+[,\.][0-9]+)\s+([0-9]+[,\.][0-9]+)/
    samples << {
      :metric => "#{metric_prefix}/load_avg_1m",
      :value => $1.to_f,
      :labels => {
        :host => hostname
      }
    }

    samples << {
      :metric => "#{metric_prefix}/load_avg_5m",
      :value => $2.to_f,
      :labels => {
        :host => hostname
      }
    }

    samples << {
      :metric => "#{metric_prefix}/load_avg_15m",
      :value => $3.to_f,
      :labels => {
        :host => hostname
      }
    }
  end

  samples.each do |sample|
    begin
      Timeout::timeout(5.0) do
        params = Hash[sample.delete(:labels).map{ |k,v| ["label[#{k}]", v] }]
        params.merge!(sample)

        uri = URI("http://localhost:8080/metrics")
        uri.query = URI.encode_www_form(params)
        Net::HTTP.post_form(uri, {})
      end
    rescue Exception => e
      puts e
    end
  end

  sleep 1
end
