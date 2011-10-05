require 'rubygems'
require 'rspec'
require 'mongoid'
require 'rack'
require 'rack/test'

ENV['RACK_ENV'] = "test"

Mongoid.configure{ |c| c.master = Mongo::Connection.new.db("fnordmetric_test") }

$: << ::File.expand_path('../../lib', __FILE__)
require "fnordmetric"