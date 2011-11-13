class FnordMetric::App < Sinatra::Base
  
  @@sessions = Hash.new
  
  #use Rack::Reloader, 0
  
  enable :session

  
  set :haml, :format => :html5 
  set :views, ::File.expand_path('../../../haml', __FILE__)
  set :public, ::File.expand_path('../../../pub', __FILE__)

  def initialize(namespaces, opts)
    @namespaces = {}
    namespaces.each do |key, block|
      @namespaces[key] = FnordMetric::Namespace.new(key, opts.clone)
      @namespaces[key].instance_eval(&block)
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
        puts "#{k} <-> #{params[:namespace]}"
        k && k.to_s == params[:namespace]
      }.intern]
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

  #get '/metric/:name' do
  #  content_type 'application/json'
  #  FnordMetric::MetricAPI.new(params).render    
  #end

  #get '/widget/:name' do
  #  @dashboard = FnordMetric.dashboards.first
  #  @widget = @dashboard.widgets.first
  #  haml :widget
  #end

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

