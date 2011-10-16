require 'rubygems'
require 'bundler'
begin
  Bundler.setup(:default, :development)
rescue Bundler::BundlerError => e
  $stderr.puts e.message
  $stderr.puts "Run `bundle install` to install missing gems"
  exit e.status_code
end
require 'rake'

require 'jeweler'
Jeweler::Tasks.new do |gem|
  gem.name = "fnordmetric"
  gem.homepage = "http://github.com/paulasmuth/fnordmetric"
  gem.license = "MIT"
  gem.summary = "FnordMetric is a Ruby Event-Tracking gem on steroids"
  gem.description = "FnordMetric is a Ruby Event-Tracking gem on steroids"
  gem.email = "paul@paulasmuth.com"
  gem.authors = ["Paul Asmuth"]
end

require 'rspec'
require 'rspec/core/rake_task'
desc "Run all examples"
task RSpec::Core::RakeTask.new('spec')

