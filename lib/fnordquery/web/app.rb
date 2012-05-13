# encoding: utf-8

class FnordQuery::Web::App < Sinatra::Base

  if RUBY_VERSION =~ /1.9.\d/
    Encoding.default_external = Encoding::UTF_8
  end

  enable :session

  #set :haml, :format => :html5
  #set :views, ::File.expand_path('../../../../haml', __FILE__)

  def initialize(opts)
    @opts = opts
    super(nil)
  end

  helpers do
    include Rack::Utils
    alias_method :h, :escape_html
  end

  if ENV['RACK_ENV'] == "test"
    set :raise_errors, true
  end

  get '/' do
  	'hello'
  end

end

