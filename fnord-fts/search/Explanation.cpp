/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "fnord-fts/fts.h"
#include "fnord-fts/search/Explanation.h"
#include "fnord-fts/util/StringUtils.h"

namespace Lucene {

Explanation::Explanation(double value, const String& description) {
    this->value = value;
    this->description = description;
}

Explanation::~Explanation() {
}

bool Explanation::isMatch() {
    return (0.0 < getValue());
}

double Explanation::getValue() {
    return value;
}

void Explanation::setValue(double value) {
    this->value = value;
}

String Explanation::getDescription() {
    return description;
}

void Explanation::setDescription(const String& description) {
    this->description = description;
}

String Explanation::getSummary() {
    return StringUtils::toString(getValue()) + L" = " + getDescription();
}

Collection<ExplanationPtr> Explanation::getDetails() {
    if (!details) {
        return Collection<ExplanationPtr>();
    }
    return Collection<ExplanationPtr>::newInstance(this->details.begin(), this->details.end());
}

void Explanation::addDetail(const ExplanationPtr& detail) {
    if (!details) {
        details = Collection<ExplanationPtr>::newInstance();
    }
    details.add(detail);
}

String Explanation::toString() {
    return toString(0);
}

String Explanation::toString(int32_t depth) {
    String buffer;
    for (int32_t i = 0; i < depth; ++i) {
        buffer += L"  ";
    }
    buffer += getSummary() + L"\n";
    if (details) {
        for (int32_t i = 0; i < details.size(); ++i) {
            buffer += details[i]->toString(depth + 1);
        }
    }
    return buffer;
}

String Explanation::toHtml() {
    String buffer(L"<ul>\n<li>" + getSummary() + L"<br />\n");
    if (details) {
        for (int32_t i = 0; i < details.size(); ++i) {
            buffer += details[i]->toHtml();
        }
    }
    buffer += L"</li>\n</ul>\n";
    return buffer;
}

IDFExplanation::~IDFExplanation() {
}

}
