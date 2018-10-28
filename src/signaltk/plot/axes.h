/**
 * This file is part of the "libstx" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *
 * libstx is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#pragma once
#include <utility>
#include <string>
#include <vector>
#include "domain.h"
#include "domainprovider.h"

namespace signaltk {
namespace chart {

class AxisDefinition {
public:

  /**
   * The axis tick position
   */
  enum kLabelPosition {
    LABELS_INSIDE = 0,
    LABELS_OUTSIDE = 1,
    LABELS_OFF = 2
  };

  /**
   * Create a new axis definition
   *
   * @param axis_position the position of the axis ({TOP,RIGHT,BOTTOM,LEFT})
   */
  AxisDefinition();

  /**
   * Add a "tick" to this axis
   *
   * @param tick_position the position of the tick (0.0-1.0)
   */
  void addTick(double tick_position);

  /**
   * Returns the ticks of this axis
   */
  const std::vector<double> getTicks() const;

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
  const std::vector<std::pair<double, std::string>> getLabels() const;

  /**
   * Returns true if this axis has labels, false otherwise
   */
  bool hasLabels() const;

  /**
   * Set the label position for this axis
   */
  void setLabelPosition(kLabelPosition pos);

  /**
   * Return the label position for this axis
   */
  kLabelPosition getLabelPosition() const;

  /**
   * Set the label rotation for this axis
   */
  void setLabelRotation(double deg);

  /**
   * Return the label rotaitoj for this axis
   */
  double getLabelRotation() const;

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

  /**
   * Set the domain for this axis
   */
  void setDomain(DomainProvider* domain);

  bool enabled_;
  std::string title_;
  std::vector<double> ticks_;
  bool has_ticks_;
  std::vector<std::pair<double, std::string>> labels_;
  bool has_labels_;
};

struct AxisDefinitions {
  AxisDefinition top;
  AxisDefinition right;
  AxisDefinition bottom;
  AxisDefinition left;
};

} // namespace chart
} // namespace signaltk

