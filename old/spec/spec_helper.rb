require 'rubygems'
require 'rspec'

require 'active_record'
ActiveRecord::Base.establish_connection(:adapter => "sqlite3", :database => ::File.expand_path('../../db/test.sqlite', __FILE__))

$: << ::File.expand_path('../../lib', __FILE__)
require "fnordmetric"


unless ENV["NO_MIGRATION"]

  begin
    ActiveRecord::Migration.class_eval do
      drop_table :my_metric
    end
  rescue; end

  ActiveRecord::Migration.class_eval do
    create_table :my_metric do |t|        
      t.text :data
      t.timestamps
    end
  end

end


class MyMetric < ActiveRecord::Base

  include FnordMetric

  set_table_name "my_metric"

end