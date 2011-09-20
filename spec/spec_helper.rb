require 'rubygems'
require 'rspec'
require 'mongoid'

$: << ::File.expand_path('../../lib', __FILE__)
require "fnordmetric"

include FnordMetric

