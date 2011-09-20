class Fnordmetric::Event
  include Mongoid::Document

  field :type, :type => String
  field :client, :type =>  Integer
  field :data, :type =>  Hash

  def self.track!(params_hash)
  	params_hash.to_options!
  	self.create(:type => params_hash.delete(:type), :data => params_hash)
  end

end