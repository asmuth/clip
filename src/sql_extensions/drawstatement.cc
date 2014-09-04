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
#include <fnordmetric/sql_extensions/linechartbuilder.h>

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
      chart = mkChart<LineChartBuilder>(canvas);
      break;
    default:
      RAISE(
          util::RuntimeException,
          "invalid chart type: %s",
          Token::getTypeName(ast_->getToken()->getType()));
  }

  applyAxisDefinitions(chart);
  applyDomainDefinitions(chart);
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
    if (child->getType() != ASTNode::T_AXIS ||
        child->getChildren().size() < 1 ||
        child->getChildren()[0]->getType() != ASTNode::T_AXIS_POSITION) {
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

void DrawStatement::applyDomainDefinitions(ui::Drawable* chart) const {
  for (const auto& child : ast_->getChildren()) {
    bool invert = false;
    bool logarithmic = false;
    ASTNode* min_expr = nullptr;
    ASTNode* max_expr = nullptr;

    if (child->getType() != ASTNode::T_DOMAIN) {
      continue;
    }

    for (const auto& domain_prop : child->getChildren()) {
      switch (domain_prop->getType()) {
        case ASTNode::T_DOMAIN_SCALE: {
          auto min_max_expr = domain_prop->getChildren();
          if (min_max_expr.size() != 2 ) {
            RAISE(util::RuntimeException, "corrupt AST: invalid DOMAIN SCALE");
          }
          min_expr = min_max_expr[0];
          max_expr = min_max_expr[1];
          break;
        }

        case ASTNode::T_PROPERTY: {
          if (domain_prop->getToken() != nullptr) {
            switch (domain_prop->getToken()->getType()) {
              case Token::T_INVERT:
                invert = true;
                continue;
              case Token::T_LOGARITHMIC:
                logarithmic = true;
                continue;
              default:
                break;
            }
          }

          RAISE(util::RuntimeException, "corrupt AST: invalid DOMAIN property");
          break;
        }

        default:
          RAISE(util::RuntimeException, "corrupt AST: unexpected DOMAIN child");

      }
    }

    if (min_expr != nullptr && max_expr != nullptr) {
      auto min = executeSimpleConstExpression(min_expr).getFloat();
      auto max = executeSimpleConstExpression(max_expr).getFloat();

      printf("domain: min=%f, max=%f, inv=%s, log=%s\n",
        min,
        max,
        invert ? "true" : "false",
        logarithmic ? "true" : "false");
    }
  }
}

}
}
