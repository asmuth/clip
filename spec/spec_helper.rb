require 'rubygems'
require 'rspec'
require 'mongoid'
require 'rack'
require 'rack/test'

Mongoid.configure{ |c| c.master = Mongo::Connection.new.db("fnordmetric_test") }

$: << ::File.expand_path('../../lib', __FILE__)
require "fnordmetric"
require "fnordmetric/app"

