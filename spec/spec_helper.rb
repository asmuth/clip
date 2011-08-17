require 'rubygems'
require 'rspec'

require 'active_record'
ActiveRecord::Base.establish_connection(:adapter => "sqlite3", :database => ::File.expand_path('../../db/test.sqlite', __FILE__))

$: << ::File.expand_path('../../lib', __FILE__)

require "fnordmetric"

class MyMetric < FnordMetric::Base
	

end