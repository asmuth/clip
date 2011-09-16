class Event
  include MongoMapper::Document

  key :type, String
  key :client, Integer
  key :data, Hash

  def self.track!(params_hash)
  	params_hash.to_options!
  	event_type = params_hash.delete(:event)
  	self.create(:type => event_type, :data => params_hash)
  end

  def self.calculate_pageview_url_count

  	map_task = "function(){ emit(this.data.url, 1); }"

  	reduce_task = <<-ENDJS
      function(prev, current) {
        var count = 0;

        for (index in current) {
            count += current[index];
        }

        return count;
      }
    ENDJS
  
    Event.collection.map_reduce(map_task, reduce_task, :query => {})
  end

end