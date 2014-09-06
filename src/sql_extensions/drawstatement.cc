/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2011-2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
#include <fnordmetric/sql_extensions/domainconfig.h>
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

  applyDomainDefinitions(chart);
  applyTitle(chart);
  applyAxisDefinitions(chart);
  applyGrid(chart);
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

    for (int i = 1; i < child->getChildren().size(); ++i) {
      auto prop = child->getChildren()[i];

      if (prop->getType() == ASTNode::T_PROPERTY &&
          prop->getToken() != nullptr &&
          *prop->getToken() == Token::T_TITLE &&
          prop->getChildren().size() == 1) {
        auto axis_title = executeSimpleConstExpression(prop->getChildren()[0]);
        axis->setTitle(axis_title.toString());
        continue;
      }

      if (prop->getType() == ASTNode::T_AXIS_LABELS) {
        applyAxisLabels(prop, axis);
      }
    }
  }
}

void DrawStatement::applyAxisLabels(
    ASTNode* ast,
    ui::AxisDefinition* axis) const {
  for (const auto& prop : ast->getChildren()) {
    if (prop->getType() != ASTNode::T_PROPERTY ||
        prop->getToken() == nullptr) {
      continue;
    }

    switch (prop->getToken()->getType()) {
      case Token::T_INSIDE:
        axis->setLabelPosition(ui::AxisDefinition::LABELS_INSIDE);
        break;
      case Token::T_OUTSIDE:
        axis->setLabelPosition(ui::AxisDefinition::LABELS_OUTSIDE);
        break;
      case Token::T_OFF:
        axis->setLabelPosition(ui::AxisDefinition::LABELS_OFF);
        break;
      case Token::T_ROTATE: {
        if (prop->getChildren().size() != 1) {
          RAISE(util::RuntimeException, "corrupt AST: ROTATE has no children");
        }

        auto rot = executeSimpleConstExpression(prop->getChildren()[0]);
        axis->setLabelRotation(rot.getValue<double>());
        break;
      }
      default:
        RAISE(util::RuntimeException, "corrupt AST: LABELS has invalid token");
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

    if (child->getToken() == nullptr) {
      RAISE(util::RuntimeException, "corrupt AST: DOMAIN has no token");
    }

    ui::AnyDomain::kDimension dim;
    switch (child->getToken()->getType()) {
      case Token::T_XDOMAIN:
        dim = ui::AnyDomain::DIM_X;
        break;
      case Token::T_YDOMAIN:
        dim = ui::AnyDomain::DIM_Y;
        break;
      case Token::T_ZDOMAIN:
        dim = ui::AnyDomain::DIM_Z;
        break;
      default:
        RAISE(util::RuntimeException, "corrupt AST: DOMAIN has invalid token");
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

    DomainConfig domain_config(chart, dim);
    domain_config.setInvert(invert);
    domain_config.setLogarithmic(logarithmic);
    if (min_expr != nullptr && max_expr != nullptr) {
      domain_config.setMin(executeSimpleConstExpression(min_expr));
      domain_config.setMax(executeSimpleConstExpression(max_expr));
    }
  }
}

void DrawStatement::applyTitle(ui::Drawable* chart) const {
  for (const auto& child : ast_->getChildren()) {
    if (child->getType() != ASTNode::T_PROPERTY ||
        child->getToken() == nullptr || !(
        child->getToken()->getType() == Token::T_TITLE ||
        child->getToken()->getType() == Token::T_SUBTITLE)) {
      continue;
    }

    if (child->getChildren().size() != 1) {
      RAISE(util::RuntimeException, "corrupt AST: [SUB]TITLE has != 1 child");
    }

    auto title_eval = executeSimpleConstExpression(child->getChildren()[0]);
    auto title_str = title_eval.toString();

    switch (child->getToken()->getType()) {
      case Token::T_TITLE:
        chart->setTitle(title_str);
        break;

      case Token::T_SUBTITLE:
        chart->setSubtitle(title_str);
        break;

      default:
        break;
    }
  }
}

void DrawStatement::applyGrid(ui::Drawable* chart) const {
  ASTNode* grid = nullptr;

  for (const auto& child : ast_->getChildren()) {
    if (child->getType() == ASTNode::T_GRID) {
      grid = child;
      break;
    }
  }

  if (!grid) {
    return;
  }

  bool horizontal = false;
  bool vertical = false;

  for (const auto& prop : grid->getChildren()) {
    if (prop->getType() == ASTNode::T_PROPERTY && prop->getToken() != nullptr) {
      switch (prop->getToken()->getType()) {
        case Token::T_HORIZONTAL:
          horizontal = true;
          break;

        case Token::T_VERTICAL:
          vertical = true;
          break;

        default:
          RAISE(
              util::RuntimeException,
              "corrupt AST: GRID has invalid property");
      }
    }
  }

  if (horizontal || vertical) {
    chart->addGrid(horizontal, vertical);
  }
}

}
}
