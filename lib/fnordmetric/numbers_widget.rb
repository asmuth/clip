class FnordMetric::NumbersWidget  < FnordMetric::Widget

  def data
    super.merge(
      :autoupdate => !!@options[:autoupdate],
      :round_to => @options[:round_to]||2,
      :metrics => metrics.map{ |m| m.token },
      :intervals => {
        "Current" => "",
        "Today" => "at=#{1.day.ago.to_i}-#{Time.now.to_i}",
        "Month" => "at=#{30.days.ago.to_i}-#{Time.now.to_i}"
      },
      :widget_url => "/widget_numbers.js?#{(rand*999).to_i}",
      :widget_height => 100
    )
  end

end