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
  gem.summary = "yet another report generator"
  gem.description = "yet another report generator"
  gem.email = "paul@paulasmuth.com"
  gem.authors = ["Paul Asmuth"]
end

require 'rspec'
require 'rspec/core/rake_task'
desc "Run all examples"
task RSpec::Core::RakeTask.new('spec')

desc "Create test-database (db/test.sqlite)"
task :create_test_database do
  require ::File.expand_path('../spec/spec_helper.rb', __FILE__)
  begin
    ActiveRecord::Migration.class_eval do
      drop_table :my_metrics
    end
  rescue; end
  ActiveRecord::Migration.class_eval do
    create_table :my_metrics do |t|        
      t.text :data
    end
  end
end