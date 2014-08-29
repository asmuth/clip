/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#ifndef _FNORDMETRIC_AXISDEFINITION_H
#define _FNORDMETRIC_AXISDEFINITION_H
#include <utility>
#include <string>
#include <vector>
#include "domain.h"

namespace fnordmetric {
namespace ui {

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
  AxisDefinition(kPosition axis_position, DomainAdapter* domain);

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

  /**
   * Set the domain for this axis
   */
  void setDomain(DomainAdapter* domain);

protected:
  kPosition position_;
  DomainAdapter* domain_;
  std::string title_;
  std::vector<double> ticks_;
  bool has_ticks_;
  std::vector<std::pair<double, std::string>> labels_;
  bool has_labels_;
};

// FIXPAUL: use template specialization
/*template <typename T>
AxisDefinition::AxisDefinition(
    kPosition axis_position,
    Domain<T>* domain) :
    position_(axis_position) {
  auto numerical = dynamic_cast<NumericalDomain<T>*>(domain);

  if (numerical == nullptr) {
    int m = domain->getCardinality();

    for (int n = 0; n < m; n++) {
      double tick = domain->offsetAt(n);
      addTick((double) n / m);
      addLabel((double) n / m + (1.0f / (m + 1)) * 0.5, domain->labelAt(n));
    }

    addTick(1.0);
  } else {
    for (int n = 0; n < domain->getCardinality(); n++) {
      double tick = domain->offsetAt(n);
      addTick(tick);
      addLabel(tick, domain->labelAt(n));
    }
  }
}
*/

}
}
#endif
