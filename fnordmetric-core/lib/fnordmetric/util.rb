class FnordMetric::Util

  def self.parse_time(str)
    str = str.downcase

    if (str == "now")
      Time.now.to_i
    elsif str =~ /^([0-9]+(?:\.[0-9]+)?)$/
      $1.to_i
    elsif str =~ /^-([0-9]+(?:\.[0-9]+)?)$/
      Time.now.to_i - $1.to_i
    elsif str =~ /^-([0-9]+(?:\.[0-9]+)?)s(ec(ond)?(s?))?$/
      Time.now.to_i - $1.to_f
    elsif str =~ /^-([0-9]+(?:\.[0-9]+)?)m(in(ute)?(s?))?$/
      Time.now.to_i - ($1.to_f * 60)
    elsif str =~ /^-([0-9]+(?:\.[0-9]+)?)h(our(s?))?$/
      Time.now.to_i - ($1.to_f * 3600)
    elsif str =~ /^-([0-9]+(?:\.[0-9]+)?)d(ay(s?))?$/
      Time.now.to_i - ($1.to_i * 86400)
    else 
      raise "invalid time specifiation: #{str}"
    end
  end

end
