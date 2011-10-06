class FnordMetric::App < Sinatra::Base
  
  @@sessions = Hash.new
  
  use Rack::Reloader, 0
  
  enable :session

  set :haml, :format => :html5 
  set :views, ::File.expand_path('../../../haml', __FILE__)
  set :public, ::File.expand_path('../../../pub', __FILE__)
  
  helpers do
    include Rack::Utils
    alias_method :h, :escape_html
  end

  if ENV['RACK_ENV'] == "test"
    set :raise_errors, true 
  end

  get '/' do
  	redirect '/fnordmetric'
  end

  get '/fnordmetric' do
  	redirect '/fnordmetric/dashboard/default'
  end

  get '/fnordmetric/dashboard/:name' do
    @dashboard = FnordMetric.dashboards.first
    haml :app
  end

  post '/fnordmetric/events' do
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

