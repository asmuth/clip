# -*- encoding: utf-8 -*-
$:.push File.expand_path("../lib", __FILE__)
require "fnordmetric/version"

Gem::Specification.new do |s|
<<<<<<< HEAD
  s.name = %q{fnordmetric}
  s.version = "0.6.4"

  s.required_rubygems_version = Gem::Requirement.new(">= 0") if s.respond_to? :required_rubygems_version=
  s.authors = ["Paul Asmuth"]
  s.date = %q{2012-01-19}
  s.description = %q{FnordMetric is a Ruby Event-Tracking gem on steroids}
  s.email = %q{paul@paulasmuth.com}
  s.extra_rdoc_files = [
    "README.md"
  ]
  s.files = [
    ".rspec",
    ".travis.yml",
    "Gemfile",
    "Gemfile.lock",
    "README.md",
    "Rakefile",
    "VERSION",
    "doc/full_example.rb",
    "doc/preview1.png",
    "doc/preview2.png",
    "doc/version",
    "fnordmetric.gemspec",
    "haml/app.haml",
    "haml/widget.haml",
    "lib/fnordmetric.rb",
    "lib/fnordmetric/api.rb",
    "lib/fnordmetric/app.rb",
    "lib/fnordmetric/average_metric.rb",
    "lib/fnordmetric/bars_widget.rb",
    "lib/fnordmetric/cache.rb",
    "lib/fnordmetric/combine_metric.rb",
    "lib/fnordmetric/context.rb",
    "lib/fnordmetric/count_metric.rb",
    "lib/fnordmetric/dashboard.rb",
    "lib/fnordmetric/event.rb",
    "lib/fnordmetric/funnel_widget.rb",
    "lib/fnordmetric/gauge.rb",
    "lib/fnordmetric/gauge_calculations.rb",
    "lib/fnordmetric/gauge_modifiers.rb",
    "lib/fnordmetric/inbound_stream.rb",
    "lib/fnordmetric/logger.rb",
    "lib/fnordmetric/metric.rb",
    "lib/fnordmetric/metric_api.rb",
    "lib/fnordmetric/namespace.rb",
    "lib/fnordmetric/numbers_widget.rb",
    "lib/fnordmetric/pie_widget.rb",
    "lib/fnordmetric/report.rb",
    "lib/fnordmetric/session.rb",
    "lib/fnordmetric/standalone.rb",
    "lib/fnordmetric/sum_metric.rb",
    "lib/fnordmetric/timeline_widget.rb",
    "lib/fnordmetric/toplist_widget.rb",
    "lib/fnordmetric/widget.rb",
    "lib/fnordmetric/worker.rb",
    "pub/fnordmetric.css",
    "pub/fnordmetric.js",
    "pub/loader.gif",
    "pub/vendor/highcharts.js",
    "pub/vendor/jquery-1.6.1.min.js",
    "spec/api_spec.rb",
    "spec/app_spec.rb",
    "spec/context_spec.rb",
    "spec/dashboard_spec.rb",
    "spec/event_spec.rb",
    "spec/gauge_modifiers_spec.rb",
    "spec/gauge_spec.rb",
    "spec/inbound_stream_spec.rb",
    "spec/namespace_spec.rb",
    "spec/session_spec.rb",
    "spec/spec_helper.rb",
    "spec/widget_spec.rb",
    "spec/worker_spec.rb"
  ]
  s.homepage = %q{http://github.com/paulasmuth/fnordmetric}
  s.licenses = ["MIT"]
  s.require_paths = ["lib"]
  s.rubygems_version = %q{1.5.2}
  s.summary = %q{FnordMetric is a Ruby Event-Tracking gem on steroids}
  s.test_files = [
    "spec/api_spec.rb",
    "spec/app_spec.rb",
    "spec/context_spec.rb",
    "spec/dashboard_spec.rb",
    "spec/event_spec.rb",
    "spec/gauge_modifiers_spec.rb",
    "spec/gauge_spec.rb",
    "spec/inbound_stream_spec.rb",
    "spec/namespace_spec.rb",
    "spec/session_spec.rb",
    "spec/spec_helper.rb",
    "spec/widget_spec.rb",
    "spec/worker_spec.rb"
  ]
=======
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
  s.add_dependency "em-hiredis"
  s.add_dependency "json"
  s.add_dependency "i18n"
  s.add_dependency "haml"
  s.add_dependency "rack"
  s.add_dependency "rack-test"
  s.add_dependency "yajl-ruby"
  s.add_dependency "thin"
  s.add_dependency "activesupport"
>>>>>>> 1713f4ce7384adae42df7c620e66d5bacf589bd3

  s.add_development_dependency "delorean"
  s.add_development_dependency "rspec", "~> 2.6.0"
  s.add_development_dependency "shoulda"

<<<<<<< HEAD
    if Gem::Version.new(Gem::VERSION) >= Gem::Version.new('1.2.0') then
      s.add_runtime_dependency(%q<bson_ext>, [">= 1.4.0"])
      s.add_runtime_dependency(%q<sinatra>, [">= 1.2.6"])
      s.add_runtime_dependency(%q<redis>, [">= 2.2.2"])
      s.add_runtime_dependency(%q<eventmachine>, [">= 0"])
      s.add_runtime_dependency(%q<em-hiredis>, [">= 0"])
      s.add_runtime_dependency(%q<json>, [">= 0"])
      s.add_runtime_dependency(%q<i18n>, [">= 0"])
      s.add_runtime_dependency(%q<haml>, [">= 0"])
      s.add_runtime_dependency(%q<rack>, [">= 0"])
      s.add_runtime_dependency(%q<rack-test>, [">= 0"])
      s.add_runtime_dependency(%q<yajl-ruby>, [">= 0"])
      s.add_runtime_dependency(%q<thin>, [">= 0"])
      s.add_runtime_dependency(%q<activesupport>, [">= 0"])
      s.add_development_dependency(%q<delorean>, [">= 0"])
      s.add_development_dependency(%q<rspec>, ["~> 2.6.0"])
      s.add_development_dependency(%q<shoulda>, [">= 0"])
      s.add_development_dependency(%q<jeweler>, ["~> 1.5.2"])
    else
      s.add_dependency(%q<bson_ext>, [">= 1.4.0"])
      s.add_dependency(%q<sinatra>, [">= 1.2.6"])
      s.add_dependency(%q<redis>, [">= 2.2.2"])
      s.add_dependency(%q<eventmachine>, [">= 0"])
      s.add_dependency(%q<em-hiredis>, [">= 0"])
      s.add_dependency(%q<json>, [">= 0"])
      s.add_dependency(%q<i18n>, [">= 0"])
      s.add_dependency(%q<haml>, [">= 0"])
      s.add_dependency(%q<rack>, [">= 0"])
      s.add_dependency(%q<rack-test>, [">= 0"])
      s.add_dependency(%q<yajl-ruby>, [">= 0"])
      s.add_dependency(%q<thin>, [">= 0"])
      s.add_dependency(%q<activesupport>, [">= 0"])
      s.add_dependency(%q<delorean>, [">= 0"])
      s.add_dependency(%q<rspec>, ["~> 2.6.0"])
      s.add_dependency(%q<shoulda>, [">= 0"])
      s.add_dependency(%q<jeweler>, ["~> 1.5.2"])
    end
  else
    s.add_dependency(%q<bson_ext>, [">= 1.4.0"])
    s.add_dependency(%q<sinatra>, [">= 1.2.6"])
    s.add_dependency(%q<redis>, [">= 2.2.2"])
    s.add_dependency(%q<eventmachine>, [">= 0"])
    s.add_dependency(%q<em-hiredis>, [">= 0"])
    s.add_dependency(%q<json>, [">= 0"])
    s.add_dependency(%q<i18n>, [">= 0"])
    s.add_dependency(%q<haml>, [">= 0"])
    s.add_dependency(%q<rack>, [">= 0"])
    s.add_dependency(%q<rack-test>, [">= 0"])
    s.add_dependency(%q<yajl-ruby>, [">= 0"])
    s.add_dependency(%q<thin>, [">= 0"])
    s.add_dependency(%q<activesupport>, [">= 0"])
    s.add_dependency(%q<delorean>, [">= 0"])
    s.add_dependency(%q<rspec>, ["~> 2.6.0"])
    s.add_dependency(%q<shoulda>, [">= 0"])
    s.add_dependency(%q<jeweler>, ["~> 1.5.2"])
  end
=======
  s.files         = `git ls-files`.split("\n") - [".gitignore", ".rspec", ".travis.yml"]
  s.test_files    = `git ls-files -- spec/*`.split("\n")
  s.executables   = `git ls-files -- bin/*`.split("\n").map{ |f| File.basename(f) }
  s.require_paths = ["lib"]
>>>>>>> 1713f4ce7384adae42df7c620e66d5bacf589bd3
end
