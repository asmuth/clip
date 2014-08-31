/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <fnordmetric/sql_extensions/drawstatement.h>
#include <fnordmetric/sql_extensions/barchartbuilder.h>

namespace fnordmetric {
namespace query {

DrawStatement::DrawStatement(ASTNode* ast) : ast_(ast->deepCopy()) {}

void DrawStatement::execute(ui::Canvas* canvas) const {
  ui::Drawable* chart = nullptr;

  switch (ast_->getToken()->getType()) {
    case Token::T_BARCHART:
      chart = mkChart<BarChartBuilder>(canvas);
      break;
    case Token::T_LINECHART:
      chart = mkChart<BarChartBuilder>(canvas);
      break;
    default:
      RAISE(
          util::RuntimeException,
          "invalid chart type: %s",
          Token::getTypeName(ast_->getToken()->getType()));
  }

  applyAxisDefinitions(chart);
}

ASTNode const* DrawStatement::getProperty(Token::kTokenType key) const {
  for (const auto& child : ast_->getChildren()) {
    if (child->getType() != ASTNode::T_PROPERTY) {
      continue;
    }

    if (child->getToken()->getType() != key) {
      continue;
    }

    const auto& values = child->getChildren();
    if (values.size() != 1) {
      RAISE(util::RuntimeException, "corrupt AST: T_PROPERTY has != 1 child");
    }

    return values[0];
  }

  return nullptr;
}

void DrawStatement::applyAxisDefinitions(ui::Drawable* chart) const {
  for (const auto& child : ast_->getChildren()) {
    if (child->getType() != ASTNode::T_PROPERTY ||
        child->getToken() == nullptr ||
        child->getToken()->getType() != Token::T_AXIS) {
      continue;
    }

    ui::AxisDefinition* axis = nullptr;

    if (child->getChildren().size() < 1) {
      RAISE(util::RuntimeException, "corrupt AST: AXIS has < 1 child");
    }

    switch (child->getChildren()[0]->getToken()->getType()) {
      case Token::T_TOP:
        axis = chart->addAxis(ui::AxisDefinition::TOP);
        break;

      case Token::T_RIGHT:
        axis = chart->addAxis(ui::AxisDefinition::RIGHT);
        break;

      case Token::T_BOTTOM:
        axis = chart->addAxis(ui::AxisDefinition::BOTTOM);
        break;

      case Token::T_LEFT:
        axis = chart->addAxis(ui::AxisDefinition::LEFT);
        break;

      default:
        RAISE(util::RuntimeException, "corrupt AST: invalid axis position");
    }
  }
}

}
}
