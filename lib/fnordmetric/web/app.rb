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

  def initialize(opts)
    @opts = opts

    @namespaces = FnordMetric.namespaces
    @redis = Redis.connect(:url => opts[:redis_url])

    super(nil)
  end

  get '/' do
    redirect "#{path_prefix}/#{@namespaces.keys.first}"
  end

  get '/:namespace' do
    pass unless current_namespace
    haml :app
  end

  post '/events' do
    halt 400, 'please specify the event_type (_type)' unless params["_type"]
    track_event((8**32).to_s(36), parse_params(params))
  end

  # FIXPAUL move to websockets
  get '/:namespace/dashboard/:dashboard' do
    dashboard = current_namespace.dashboards.fetch(params[:dashboard])

    dashboard.to_json
  end


  # get '/favicon.ico' do
  #   ""
  # end

  # get '/:namespace/gauge/:name' do
  #   gauge = if params[:name].include?("++")
  #     parts = params[:name].split("++")
  #     current_namespace.gauges[parts.first.to_sym].fetch_gauge(parts.last, params[:tick].to_i)
  #   else
  #     current_namespace.gauges[params[:name].to_sym]
  #   end

  #   if !gauge
  #     status 404
  #     return ""
  #   end

  #   data = if gauge.three_dimensional?
  #     _t = (params[:at] || Time.now).to_i
  #     { :count => gauge.field_values_total(_t), :values => gauge.field_values_at(_t) }
  #   elsif params[:at] && params[:at] =~ /^[0-9]+$/
  #     { (_t = gauge.tick_at(params[:at].to_i)) => gauge.value_at(_t) }
  #   elsif params[:at] && params[:at] =~ /^([0-9]+)-([0-9]+)$/
  #     _range = params[:at].split("-").map(&:to_i)
  #     _values = gauge.values_in(_range.first.._range.last)
  #     params[:sum] ? { :sum => _values.values.compact.map(&:to_i).sum } : _values
  #   else
  #     { (_t = gauge.tick_at(Time.now.to_i-gauge.tick)) => gauge.value_at(_t) }
  #   end

  #   data.to_json
  # end

  # get '/:namespace/sessions' do
  #   sessions = current_namespace.sessions(:all, :limit => 100).map do |session|
  #     session.fetch_data!
  #     session.to_json
  #   end

  #   { :sessions => sessions }.to_json
  # end

  # get '/:namespace/events' do
  #   events = if params[:type]
  #     current_namespace.events(:by_type, :type => params[:type])
  #   elsif params[:session_key]
  #     current_namespace.events(:by_session_key, :session_key => params[:session_key])
  #   else 
  #     find_opts = { :limit => 100 }
  #     find_opts.merge!(:since => params[:since].to_i+1) if params[:since]
  #     current_namespace.events(:all, find_opts)
  #   end

  #   { :events => events.map(&:to_json) }.to_json
  # end

  # get '/:namespace/event_types' do
  #   types_key = current_namespace.key_prefix("type-")
  #   keys = @redis.keys("#{types_key}*").map{ |k| k.gsub(types_key,'') }

  #   { :types => keys }.to_json
  # end



end

