class FnordMetric::Cache
#  include Mongoid::Document

#  self.collection_name = 'fnordmetric_cache' 

#  field :cache_key, :type => String
#  field :data, :type => Hash

  def self.store!(cache_key, data)
    data = { :value => data } unless data.is_a?(Hash)
    self.create(:cache_key => cache_key, :data => data)
  end

  def self.get(cache_key)
    item = self.where(:cache_key => cache_key).last
    return nil unless item
    item.data.keys == ["value"] ? item.data["value"] : item.data
  end

end