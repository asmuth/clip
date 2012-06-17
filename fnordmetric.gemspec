# -*- encoding: utf-8 -*-
$:.push File.expand_path("../lib", __FILE__)
require "fnordmetric/version"

Gem::Specification.new do |s|
  s.name        = "fnordmetric"
  s.version     = FnordMetric::VERSION
  s.date        = Date.today.to_s
  s.platform    = Gem::Platform::RUBY
  s.authors     = ["Paul Asmuth"]
  s.email       = ["paul@paulasmuth.com"]
  s.homepage    = "http://github.com/paulasmuth/fnordmetric"
  s.summary     = %q{FnordMetric is a Ruby Event-Tracking gem on steroids}
  s.description = %q{FnordMetric is a Ruby Event-Tracking gem on steroids}
  s.licenses    = ["MIT"]
  
  s.add_dependency "bson_ext", ">= 1.4.0"
  s.add_dependency "sinatra", ">= 1.2.6"
  s.add_dependency "redis", ">= 2.2.2"
  s.add_dependency "eventmachine"
  s.add_dependency "websocket-rack", "0.4.0"
  s.add_dependency "em-hiredis", "0.1.1"
  s.add_dependency "json"
  s.add_dependency "i18n"
  s.add_dependency "haml"
  s.add_dependency "rack"
  s.add_dependency "rack-test"
  s.add_dependency "yajl-ruby"
  s.add_dependency "thin", "~> 1.3.0"
  s.add_dependency "activesupport"

  s.add_development_dependency "delorean"
  s.add_development_dependency "rspec", "~> 2.8.0"
  s.add_development_dependency "shoulda"

  s.files         = `git ls-files`.split("\n") - [".gitignore", ".rspec", ".travis.yml"]
  s.test_files    = `git ls-files -- spec/*`.split("\n")
  s.executables   = `git ls-files -- bin/*`.split("\n").map{ |f| File.basename(f) }
  s.require_paths = ["lib"]
end
