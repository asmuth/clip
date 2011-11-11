require 'rubygems'
require 'rspec'
require 'redis'
require 'rack'
require 'rack/test'
require 'delorean'

ENV['RACK_ENV'] = "test"

$: << ::File.expand_path('../../lib', __FILE__)
require "fnordmetric"
