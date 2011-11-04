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

  it "should add a widget" do
    dashboard = FnordMetric::Dashboard.new(:title => 'My!F00bar-.Dash_board'){ |dash| }
    FnordMetric.define(:my_metric, :sum => :my_field)
    widget = FnordMetric.widget(:my_widget, :metrics => :my_metric, :title => "My Widget", :type => :timeline)
    dashboard.add_widget(widget)
    dashboard.widgets.first.should == widget
  end

  it "should add a widget by name" do
    dashboard = FnordMetric::Dashboard.new(:title => 'My!F00bar-.Dash_board'){ |dash| }
    FnordMetric.define(:my_metric, :sum => :my_field)
    widget = FnordMetric.widget(:my_widget, :metrics => :my_metric, :title => "My Widget", :type => :timeline)
    dashboard.add_widget(:my_widget)
    dashboard.widgets.first.should == widget
  end

  it "should add the report on init (and to all widgets)" do
    FnordMetric.define(:my_metric, :sum => :my_field)
    report = FnordMetric.report(:range => (4.days.ago..Time.now))
    dashboard = FnordMetric::Dashboard.new(:title => 'My Foobar Dashboard', :report => report){ |dash|       
      dash.add_widget FnordMetric.widget(:my_widget, :metrics => :my_metric, :title => "My Widget", :type => :timeline)
    }
    dashboard.report.should == report
    dashboard.widgets.last.report.should == report
  end

  it "should add the report after init (and to all widgets)" do
    FnordMetric.define(:my_metric, :sum => :my_field)
    report = FnordMetric.report(:range => (4.days.ago..Time.now))
    dashboard = FnordMetric::Dashboard.new(:title => 'My Foobar Dashboard'){ |dash| 
      dash.add_widget FnordMetric.widget(:my_widget, :metrics => :my_metric, :title => "My Widget", :type => :timeline)
    }
    dashboard.report.should == nil
    dashboard.add_report(report)
    dashboard.report.should == report
    dashboard.widgets.last.report.should == report
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