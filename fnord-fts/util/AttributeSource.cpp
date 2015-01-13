/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#include "LuceneInc.h"
#include "AttributeSource.h"
#include "Attribute.h"

namespace Lucene {

AttributeFactory::AttributeFactory() {
}

AttributeFactory::~AttributeFactory() {
}

AttributePtr AttributeFactory::createAttributeInstance(const String& className) {
    return AttributePtr(); // override
}

AttributeFactoryPtr AttributeFactory::DEFAULT_ATTRIBUTE_FACTORY() {
    static AttributeFactoryPtr _DEFAULT_ATTRIBUTE_FACTORY;
    if (!_DEFAULT_ATTRIBUTE_FACTORY) {
        _DEFAULT_ATTRIBUTE_FACTORY = newLucene<DefaultAttributeFactory>();
        CycleCheck::addStatic(_DEFAULT_ATTRIBUTE_FACTORY);
    }
    return _DEFAULT_ATTRIBUTE_FACTORY;
}

AttributeSource::AttributeSource() {
    this->attributes = MapStringAttribute::newInstance();
    this->factory = AttributeFactory::DEFAULT_ATTRIBUTE_FACTORY();
}

AttributeSource::AttributeSource(const AttributeSourcePtr& input) {
    if (!input) {
        boost::throw_exception(IllegalArgumentException(L"input AttributeSource must not be null"));
    }
    this->attributes = input->attributes;
    this->factory = input->factory;
}

AttributeSource::AttributeSource(const AttributeFactoryPtr& factory) {
    this->attributes = MapStringAttribute::newInstance();
    this->factory = factory;
}

AttributeSource::~AttributeSource() {
}

AttributeFactoryPtr AttributeSource::getAttributeFactory() {
    return this->factory;
}

void AttributeSource::addAttribute(const String& className, const AttributePtr& attrImpl) {
    // invalidate state to force recomputation in captureState()
    currentState.reset();
    attributes.put(className, attrImpl);
}

bool AttributeSource::hasAttributes() {
    return !attributes.empty();
}

AttributePtr AttributeSource::getAttribute(const String& className) {
    return attributes.get(className);
}

bool AttributeSource::hasAttribute(const String& className) {
    return attributes.contains(className);
}

void AttributeSource::computeCurrentState() {
    currentState = newLucene<AttributeSourceState>();
    AttributeSourceStatePtr c(currentState);
    MapStringAttribute::iterator attrImpl = attributes.begin();
    c->attribute = attrImpl->second;
    ++attrImpl;
    while (attrImpl != attributes.end()) {
        c->next = newLucene<AttributeSourceState>();
        c = c->next;
        c->attribute = attrImpl->second;
        ++attrImpl;
    }
}

void AttributeSource::clearAttributes() {
    if (hasAttributes()) {
        if (!currentState) {
            computeCurrentState();
        }
        for (MapStringAttribute::iterator attrImpl = attributes.begin(); attrImpl != attributes.end(); ++attrImpl) {
            attrImpl->second->clear();
        }
    }
}

AttributeSourceStatePtr AttributeSource::captureState() {
    if (!hasAttributes()) {
        return AttributeSourceStatePtr();
    }

    if (!currentState) {
        computeCurrentState();
    }

    return boost::dynamic_pointer_cast<AttributeSourceState>(currentState->clone());
}

void AttributeSource::restoreState(const AttributeSourceStatePtr& state) {
    AttributeSourceStatePtr _state(state);
    if (!_state) {
        return;
    }

    do {
        MapStringAttribute::iterator attrImpl = attributes.find(_state->attribute->getClassName());
        if (attrImpl == attributes.end()) {
            boost::throw_exception(IllegalArgumentException(L"State contains an AttributeImpl that is not in this AttributeSource"));
        }
        _state->attribute->copyTo(attrImpl->second);
        _state = _state->next;
    } while (_state);
}

int32_t AttributeSource::hashCode() {
    int32_t code = 0;
    for (MapStringAttribute::iterator attrImpl = attributes.begin(); attrImpl != attributes.end(); ++attrImpl) {
        code = code * 31 + attrImpl->second->hashCode();
    }
    return code;
}

bool AttributeSource::equals(const LuceneObjectPtr& other) {
    if (LuceneObject::equals(other)) {
        return true;
    }

    AttributeSourcePtr otherAttributeSource = boost::dynamic_pointer_cast<AttributeSource>(other);
    if (otherAttributeSource) {
        if (hasAttributes()) {
            if (!otherAttributeSource->hasAttributes()) {
                return false;
            }

            if (attributes.size() != otherAttributeSource->attributes.size()) {
                return false;
            }

            // it is only equal if all attribute impls are the same in the same order
            if (!currentState) {
                computeCurrentState();
            }

            AttributeSourceStatePtr thisState(currentState);
            if (!otherAttributeSource->currentState) {
                otherAttributeSource->computeCurrentState();
            }

            AttributeSourceStatePtr otherState(otherAttributeSource->currentState);
            while (thisState && otherState) {
                if (otherState->attribute->getClassName() != thisState->attribute->getClassName() || !otherState->attribute->equals(thisState->attribute)) {
                    return false;
                }
                thisState = thisState->next;
                otherState = otherState->next;
            }
            return true;
        } else {
            return !otherAttributeSource->hasAttributes();
        }
    } else {
        return false;
    }
}

String AttributeSource::toString() {
    StringStream buf;
    buf << L"(";
    if (hasAttributes()) {
        if (!currentState) {
            computeCurrentState();
        }
        for (AttributeSourceStatePtr state(currentState); state; state = state->next) {
            if (state != currentState) {
                buf << L",";
            }
            buf << state->attribute->toString();
        }
    }
    buf << ")";
    return buf.str();
}

AttributeSourcePtr AttributeSource::cloneAttributes() {
    AttributeSourcePtr clone(newLucene<AttributeSource>(this->factory));

    if (hasAttributes()) {
        if (!currentState) {
            computeCurrentState();
        }
        for (AttributeSourceStatePtr state(currentState); state; state = state->next) {
            clone->attributes.put(state->attribute->getClassName(), boost::dynamic_pointer_cast<Attribute>(state->attribute->clone()));
        }
    }

    return clone;
}

Collection<AttributePtr> AttributeSource::getAttributes() {
    Collection<AttributePtr> attrImpls(Collection<AttributePtr>::newInstance());
    if (hasAttributes()) {
        if (!currentState) {
            computeCurrentState();
        }
        for (AttributeSourceStatePtr state(currentState); state; state = state->next) {
            attrImpls.add(state->attribute);
        }
    }
    return attrImpls;
}

DefaultAttributeFactory::~DefaultAttributeFactory() {
}

AttributePtr DefaultAttributeFactory::createAttributeInstance(const String& className) {
    return AttributePtr();
}

AttributeSourceState::~AttributeSourceState() {
}

LuceneObjectPtr AttributeSourceState::clone(const LuceneObjectPtr& other) {
    AttributeSourceStatePtr clone(newLucene<AttributeSourceState>());
    clone->attribute = boost::dynamic_pointer_cast<Attribute>(attribute->clone());

    if (next) {
        clone->next = boost::dynamic_pointer_cast<AttributeSourceState>(next->clone());
    }

    return clone;
}

}
