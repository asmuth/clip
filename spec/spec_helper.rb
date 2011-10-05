require 'rubygems'
require 'rspec'
require 'mongoid'
require 'rack'

require 'rack/test'


Mongoid.load!(::File.expand_path('../../mongo.yml', __FILE__))

$: << ::File.expand_path('../../lib', __FILE__)
require "fnordmetric"
require "fnordmetric/app"

