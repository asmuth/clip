class FnordQuery::ReportManager

  def initialize(opts)
    @opts = opts
    reload!
  end

  def reload!
  	puts "RELOAD called!"
  end

end