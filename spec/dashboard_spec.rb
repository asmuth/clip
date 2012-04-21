# encoding: utf-8
require ::File.expand_path('../spec_helper.rb', __FILE__)

describe FnordMetric::Dashboard do

  before(:each) do
  end

  it "should remember it's title" do
    dashboard = FnordMetric::Dashboard.new(:title => 'My Foobar Dashboard')
    dashboard.title.should == 'My Foobar Dashboard'
  end

  it "should build a token" do
    dashboard = FnordMetric::Dashboard.new(:title => 'My!F00bar-.Dash_board'){ |dash| }
    dashboard.token.should == 'MyF00barDash_board'
  end

  it "should build the correct token if the dashboard name contains whitespaces" do
    dashboard = FnordMetric::Dashboard.new(:title => 'My!F00bar Dash_board'){ |dash| }
    dashboard.token.should == 'MyF00barDash_board'
  end

  it "should build the correct token if the dashboard name contains non-english characters" do
    dashboard = FnordMetric::Dashboard.new(:title => 'Новая статистика!'){ |dash| }
    dashboard.token.should_not be_empty
  end

  it "should add a widget" do
    pending("fix this")
    #dashboard = FnordMetric::Dashboard.new(:title => 'My!F00bar-.Dash_board'){ |dash| }
    #FnordMetric.metric(:my_metric, :sum => :my_field)
    #idget = FnordMetric.widget(:my_widget, :metrics => :my_metric, :title => "My Widget", :type => :timeline)
    #dashboard.add_widget(widget)
    #dashboard.widgets.first.should == widget
  end

end