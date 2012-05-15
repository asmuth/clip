class FnordQuery::CategoricalTopKReport < FnordQuery::Report

  def execute(runner, _backend)
    super
    backend = _backend[0].new(_backend[1])

    colors = ["#CD645A", "#2F635E", "#FACE4F", "#42436B"]
    
    #backend.on_finish do
      render_result
      runner.send(:shutdown, true)
    #end
  end

private

  def render_result
    render_haml(:categorical_topk_report, 'report.html')
  end

end