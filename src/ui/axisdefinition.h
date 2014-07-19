/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * Licensed under the MIT license (see LICENSE).
 */
#ifndef _FNORDMETRIC_AXISDEFINITION_H
#define _FNORDMETRIC_AXISDEFINITION_H
#include <utility>
#include <string>
#include <vector>

namespace fnordmetric {
namespace ui {
class Domain;

class AxisDefinition {
public:

  /**
   * The axis positions/placements
   */
  enum kPosition {
    TOP = 0,
    RIGHT = 1,
    BOTTOM = 2,
    LEFT = 3
  };

  /**
   * Create a new axis definition
   *
   * @param axis_position the position of the axis ({TOP,RIGHT,BOTTOM,LEFT})
   */
  AxisDefinition(kPosition axis_position);

  /**
   * Create a new axis definition from a domain
   *
   * @param axis_position the position of the axis ({TOP,RIGHT,BOTTOM,LEFT})
   * @param domain the domain. does not transfer ownership
   */
  AxisDefinition(kPosition axis_position, Domain* domain);

  /**
   * Add a "tick" to this axis
   *
   * @param tick_position the position of the tick (0.0-1.0)
   */
  void addTick(double tick_position);

  /**
   * Returns the ticks of this axis
   */
  const std::vector<double>& getTicks() const;

  /**
   * Add a label to this axis
   *
   * @param label_position the position of the label (0.0-1.0)
   * @param label_text the label text
   */
  void addLabel(double label_position, const std::string& label_text);

  /**
   * Removes the labels from this axis
   */
  void removeLabels();

  /**
   * Returns the labels of this axis
   */
  const std::vector<std::pair<double, std::string>>& getLabels() const;

  /**
   * Returns true if this axis has labels, false otherwise
   */
  bool hasLabels() const;

  /**
   * Return the position/placement of this axis
   */
  kPosition getPosition() const;

  /**
   * Set the title for this axis
   */
  void setTitle(const std::string& title);

  /**
   * Get the title for this axis
   */
  const std::string& getTitle();

  /**
   * Returns true if the title of this axis is a string with len > 0 and false
   * otherwise
   */
  bool hasTitle() const;

protected:
  kPosition position_;
  std::string title_;
  std::vector<double> ticks_;
  std::vector<std::pair<double, std::string>> labels_;
};

}
}
#endif
