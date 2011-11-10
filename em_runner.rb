require "rubygems"
require "eventmachine"
require 'em-hiredis'

module Runner

  def self.redis
    @redis ||= EM::Hiredis.connect("redis://localhost:6379")
  end

  def self.next
    redis.blpop('fnordmetric-queue', 0).callback do |list, data|
      puts data
      EM.next_tick(&method(:next))
    end
  end

end

EM.run do
  Runner.next
end