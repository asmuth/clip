require File.expand_path("../fnord_tracker", __FILE__)

domains = %w(foobar blubb blah fnord foo asd f234 auj5 235342 jgs dsadf q235)
urls = domains.map do |d|
  rand(6).to_i.times.map do 
    %W{
	  http://#{d}.com/page_one
	  http://#{d}.com/page_two
	  http://#{d}.com/blubb
	  http://#{d}.com/register
    } 
  end
end.flatten

n = 1000

100.times do 
  start = Time.now

  n.times do |i|
    print "."
    print "\n" if (i+1)%100==0
    fnord_tracker = FnordTracker.new("123123")
    fnord_tracker.track!(
      :event => "pageview", 
      :url => urls.shuffle.first
    )
  end

  puts (Time.now - start).to_s
end