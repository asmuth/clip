require 'rubygems'
require "json"
require 'net/http'

class FnordTracker

	attr_accessor :last_response

	def initialize(api_key)
		@api_key = api_key
	end

	def track!(event_hash)				
		event_data = post_data(event_hash)
		post_to_server!(event_data)
	end

private

	def post_data(event_hash)
	    post = event_hash.map{ |k,v| "e[#{k}]=#{v}" }
		(post << "apikey=#{@api_key}").join("&")		
	end

	def post_to_server!(_data)
		http = Net::HTTP.new('localhost', 3000)
		http.post('/_trackEvent', _data)
	end

end