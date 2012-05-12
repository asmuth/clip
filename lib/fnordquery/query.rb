class FnordQuery::Query
  
  X_VALIDATE = /^(([a-z]+\([^\)]*\)) *)+$/
  X_EXTRACT  = /(([a-z]+)\(([^\)]*)\))/

  def initialize(str)
  	raise "Invalid Query: #{str}" unless str.match(X_VALIDATE)
  end

end  

#   filter(KEY = VALUE)
#   filter(KEY < MAX)
#   filter(KEY > MIN)
#   filter(KEY ~ MIN-MAX)
#   filter(KEY & ONE,TWO,THREE...)
#   filter(KEY)
#   since(TIMESTAMP)
#   since(now)
#   until(TIMESTAMP)
#   until(now)
#   stream()
#
#   filter(channel = 'dawanda-firehose') since(0) until(now)
#   filter(channel = 'dawanda-firehose') since(now) stream()
#   filter(channel & 'dawanda-firehose','dawanda-searchfeed') since(now) stream()



