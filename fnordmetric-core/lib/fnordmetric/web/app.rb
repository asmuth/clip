# encoding: utf-8

class FnordMetric::App < Sinatra::Base

  include FnordMetric::AppHelpers

  if RUBY_VERSION =~ /1.9.\d/
    Encoding.default_external = Encoding::UTF_8
  end

  if ENV['RACK_ENV'] == "test"
    set :raise_errors, true
  end

  enable :session
  set :haml, :format => :html5
  set :views, ::File.expand_path('../../../../web/haml', __FILE__)
  set :public_folder, ::File.expand_path('../../../../web', __FILE__)

  helpers do
    include Rack::Utils    
    include FnordMetric::AppHelpers
  end

  %w(fnordmetric-ui.js fnordmetric-ui.css fnordmetric-core.css fnordmetric-core.js).each do |f|
    next if ::File.exists?(::File.expand_path("../../../../web/#{f}", __FILE__))
    raise "error: file 'web/#{f}' does not exist, please run build.sh in web/"
  end

  def initialize(opts = {})
    @opts = FnordMetric.options(opts)

    @namespaces = FnordMetric.namespaces
    @redis = Redis.connect(:url => @opts[:redis_url])

    super(nil)
  end

  get '/' do
    redirect "#{path_prefix}/#{@namespaces.keys.first}"
  end

  get '/:namespace' do
    pass unless current_namespace
    current_namespace.ready!(@redis)
    haml :app
  end

  post '/events' do
    params = JSON.parse(request.body.read) unless params
    halt 400, 'please specify the event_type (_type)' unless params["_type"]
    track_event((8**32).to_s(36), parse_params(params))
  end

  # FIXPAUL move to websockets
  get '/:namespace/dashboard/:dashboard' do
    dashboard = current_namespace.dashboards.fetch(params[:dashboard])

    dashboard.to_json
  end

end

