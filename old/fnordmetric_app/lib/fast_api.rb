module FnordmetricApp
  class FastApi

    def initialize(app)
      @app = app
    end

    def call(env)
      self.dup._call(env)
    end

    def _call(env)
      if env["PATH_INFO"] =~ /^\/_trackEvent/
        process_request(env)
      else
        @app.call(env)
      end
    end

  private

    def process_request(env)
      @event = Event.track!(env["rack.request.form_hash"]["e"])
      [200, { "Content-Type" => "text/plain" }, @event.inspect]
    end

  end
end