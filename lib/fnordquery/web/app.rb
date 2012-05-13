# encoding: utf-8

class FnordQuery::Web::App < Sinatra::Base

  if RUBY_VERSION =~ /1.9.\d/
    Encoding.default_external = Encoding::UTF_8
  end

  enable :session

  set :haml, :format => :html5
  set :views, ::File.expand_path('../../../../web/haml', __FILE__)
  set :public_folder, ::File.expand_path('../../../../web', __FILE__)

  def initialize(opts)
    @opts = opts

    @reports = FnordQuery::ReportManager.load(@opts)
    
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
  	render :haml, :app
  end

  get '/report/:token.json' do
    if @reports.has_key?(params[:token])
      @reports[params[:token]].to_json
    else
      status 404
      "not found"
    end
  end

end

