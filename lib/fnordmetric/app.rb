# encoding: utf-8

class FnordMetric::App < Sinatra::Base
  
  @@sessions = Hash.new
  
  Encoding.default_external = Encoding::UTF_8

  #use Rack::Reloader, 0
  
  enable :session

  set :haml, :format => :html5 
  set :views, ::File.expand_path('../../../haml', __FILE__)
  set :public, ::File.expand_path('../../../pub', __FILE__)

  def initialize(namespaces, opts)
    @namespaces = {}
    @redis = Redis.new
    namespaces.each do |key, block|
      @namespaces[key] = FnordMetric::Namespace.new(key, opts.clone)
      @namespaces[key].instance_eval(&block)
      @namespaces[key].ready!(@redis.clone)
    end
    super(nil)
  end
  
  helpers do
    include Rack::Utils
    alias_method :h, :escape_html

    def path_prefix
      request.env["SCRIPT_NAME"]
    end

    def current_namespace          
      @namespaces[@namespaces.keys.detect{ |k|
        k.to_s == params[:namespace]
      }.try(:intern)]
    end

  end

  if ENV['RACK_ENV'] == "test"
    set :raise_errors, true 
  end

  get '/' do
  	redirect "#{path_prefix}/#{@namespaces.keys.first}"
  end

  get '/:namespace' do
    haml :app
  end

  get '/favicon.ico' do
    ""
  end

  #get '/metric/:name' do
  #  content_type 'application/json'
  #  FnordMetric::MetricAPI.new(params).render    
  #end

  #get '/widget/:name' do
  #  @dashboard = FnordMetric.dashboards.first
  #  @widget = @dashboard.widgets.first
  #  haml :widget
  #end

  get '/:namespace/sessions' do

    sessions = current_namespace.sessions(:all, :limit => 100).map do |session|
      session.fetch_data!
      session.to_json
    end 

    { :sessions => sessions }.to_json
  end

  get '/:namespace/events' do

    events = if params[:type] 
      current_namespace.events(:by_type, :type => params[:type])
    else 
      find_opts = { :limit => 100 }
      find_opts.merge!(:since => params[:since].to_i+1) if params[:since]
      current_namespace.events(:all, find_opts)
    end

    { :events => events.map(&:to_json) }.to_json
  end

  get '/:namespace/event_types' do
    types_key = current_namespace.key_prefix("type-")
    keys = @redis.keys("#{types_key}*").map{ |k| k.gsub(types_key,'') }

    { :types => keys }.to_json
  end

  post '/events' do
    halt 400, 'please specify the event_type' unless params["type"]       
    event_type = params.delete("type")
    FnordMetric.track(event_type, parse_params(params))
  end

private

  def parse_params(hash)
    hash.tap do |h|
      h.keys.each{ |k| h[k] = parse_param(h[k]) }      
    end
  end

  def parse_param(object)
    return object unless object.is_a?(String)
    return object.to_f if object.match(/^[0-9]+[,\.][0-9]+$/)
    return object.to_i if object.match(/^[0-9]+$/)
    object
  end

end

