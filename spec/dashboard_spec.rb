require ::File.expand_path('../spec_helper.rb', __FILE__)

describe FnordMetric::Dashboard do

  before(:each) do
    FnordMetric::Event.destroy_all
    FnordMetric.reset_metrics
  end

  it "should remember it's title" do
    dashboard = FnordMetric::Dashboard.new(:title => 'My Foobar Dashboard'){ |dash| }
    dashboard.title.should == 'My Foobar Dashboard'
  end

  it "should build a token" do
    dashboard = FnordMetric::Dashboard.new(:title => 'My!F00bar-.Dash_board'){ |dash| }
    dashboard.token.should == 'MyF00barDash_board'
  end

  it "should define a new widget" do
    FnordMetric.define(:my_metric, :sum => :my_field)
    dashboard = FnordMetric::Dashboard.new(:title => 'My Foobar Dashboard'){ |dash| 
      dash.widget :my_metric, :title => "My Widget", :type => :graph
    }
    dashboard.widgets.last.title.should == "My Widget"
  end

  it "should define a new widget" do
    FnordMetric.define(:my_metric, :sum => :my_field)
    dashboard = FnordMetric::Dashboard.new(:title => 'My Foobar Dashboard'){ |dash| 
      dash.widget :my_metric, :title => "My Widget", :type => :graph
    }
    dashboard.widgets.last.should be_a(FnordMetric::GraphWidget)
  end

  it "should define a new widget" do
    FnordMetric.define(:my_metric, :sum => :my_field)
    dashboard = FnordMetric::Dashboard.new(:title => 'My Foobar Dashboard'){ |dash| 
      dash.widget :my_metric, :title => "My Widget", :type => :funnel
    }
    dashboard.widgets.last.should be_a(FnordMetric::FunnelWidget)
  end

  it "should raise an error if no type option is provided" do
    lambda{
      FnordMetric::Dashboard.new(:title => 'My Foobar Dashboard'){ |dash| 
        dash.widget :my_unknown_metric, :title => "My Widget"
      }
    }.should raise_error("missing option - :type")
  end

  it "should raise an error if an unknown metric is added to a widget" do
    lambda{
      FnordMetric::Dashboard.new(:title => 'My Foobar Dashboard'){ |dash| 
        dash.widget :my_unknown_metric, :title => "My Widget"
      }
    }.should raise_error("metric not found: my_unknown_metric")
  end

  it "should define a new widget showing one metric" do
    FnordMetric.define(:my_metric, :sum => :my_field)
    dashboard = FnordMetric::Dashboard.new(:title => 'My Foobar Dashboard'){ |dash| 
      dash.widget :my_metric, :title => "My Widget, :type => :graph"
    }
    widget = dashboard.widgets.last
    widget.metrics.length.should == 1
    widget.metrics.first.should be_a(FnordMetric::SumMetric)
    widget.metrics.first.token.should == :my_metric
  end

  it "should define a new widget showing two metrics" do
    FnordMetric.define(:first_metric, :count => :true)
    FnordMetric.define(:second_metric, :count => :true)
    dashboard = FnordMetric::Dashboard.new(:title => 'My Foobar Dashboard'){ |dash| 
      dash.widget [:first_metric, :second_metric], :title => "My Widget", :type => :graph
    }
    widget = dashboard.widgets.last
    widget.metrics.length.should == 2
    widget.metrics.first.should be_a(FnordMetric::CountMetric)
    widget.metrics.first.token.should == :first_metric
    widget.metrics.last.should be_a(FnordMetric::CountMetric)
    widget.metrics.last.token.should == :second_metric
  end
    
  it "should call the config block" do
   block_called = false
   FnordMetric::Dashboard.new(:title => 'My Dashboard') do |dash|
      block_called = true
      dash.should be_a(FnordMetric::Dashboard)
    end
    block_called.should be_true
  end

end