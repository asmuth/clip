/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef ATTRIBUTESOURCE_H
#define ATTRIBUTESOURCE_H

#include "LuceneObject.h"

namespace Lucene {

class AttributeFactory : public LuceneObject {
protected:
    AttributeFactory();

public:
    virtual ~AttributeFactory();

    LUCENE_CLASS(AttributeFactory);

public:
    /// returns an {@link Attribute}.
    virtual AttributePtr createAttributeInstance(const String& className);

    template <class ATTR>
    AttributePtr createInstance(const String& className) {
        AttributePtr attrImpl = createAttributeInstance(className);
        return attrImpl ? attrImpl : newLucene<ATTR>();
    }

    /// This is the default factory that creates {@link Attribute}s using the class name of the supplied
    /// {@link Attribute} interface class by appending Impl to it.
    static AttributeFactoryPtr DEFAULT_ATTRIBUTE_FACTORY();
};

/// An AttributeSource contains a list of different {@link Attribute}s, and methods to add and get them.
/// There can only be a single instance of an attribute in the same AttributeSource instance. This is ensured
/// by passing in the actual type of the Attribute (Class<Attribute>) to the {@link #addAttribute(Class)},
/// which then checks if an instance of that type is already present. If yes, it returns the instance, otherwise
/// it creates a new instance and returns it.
class AttributeSource : public LuceneObject {
public:
    /// An AttributeSource using the default attribute factory {@link DefaultAttributeFactory}.
    AttributeSource();

    /// An AttributeSource that uses the same attributes as the supplied one.
    AttributeSource(const AttributeSourcePtr& input);

    /// An AttributeSource using the supplied {@link AttributeFactory} for creating new {@link Attribute}
    /// instances.
    AttributeSource(const AttributeFactoryPtr& factory);

    virtual ~AttributeSource();

    LUCENE_CLASS(AttributeSource);

protected:
    AttributeFactoryPtr factory;
    MapStringAttribute attributes;
    AttributeSourceStatePtr currentState;

public:
    /// returns the used AttributeFactory.
    AttributeFactoryPtr getAttributeFactory();

    /// This method first checks if an instance of that class is already in this AttributeSource and returns it.
    /// Otherwise a new instance is created, added to this AttributeSource and returned.
    template <class ATTR>
    std::shared_ptr<ATTR> addAttribute() {
        String className(ATTR::_getClassName());
        std::shared_ptr<ATTR> attrImpl(boost::dynamic_pointer_cast<ATTR>(getAttribute(className)));
        if (!attrImpl) {
            attrImpl = boost::dynamic_pointer_cast<ATTR>(factory->createInstance<ATTR>(className));
            if (!attrImpl) {
                boost::throw_exception(IllegalArgumentException(L"Could not instantiate implementing class for " + className));
            }
            addAttribute(className, attrImpl);
        }
        return attrImpl;
    }

    /// Adds a custom Attribute instance.
    void addAttribute(const String& className, const AttributePtr& attrImpl);

    /// Returns true if this AttributeSource has any attributes.
    bool hasAttributes();

    /// Returns true, if this AttributeSource contains the passed-in Attribute.
    template <class ATTR>
    bool hasAttribute() {
        return getAttribute(ATTR::_getClassName()).get() != NULL;
    }

    /// Returns the instance of the passed in Attribute contained in this AttributeSource.
    template <class ATTR>
    std::shared_ptr<ATTR> getAttribute() {
        String className(ATTR::_getClassName());
        std::shared_ptr<ATTR> attr(boost::dynamic_pointer_cast<ATTR>(getAttribute(className)));
        if (!attr) {
            boost::throw_exception(IllegalArgumentException(L"This AttributeSource does not have the attribute '" + className + L"'."));
        }
        return attr;
    }

    /// Resets all Attributes in this AttributeSource by calling {@link AttributeImpl#clear()} on each Attribute
    /// implementation.
    void clearAttributes();

    /// Captures the state of all Attributes. The return value can be passed to {@link #restoreState} to restore
    /// the state of this or another AttributeSource.
    AttributeSourceStatePtr captureState();

    /// Restores this state by copying the values of all attribute implementations that this state contains into
    /// the attributes implementations of the targetStream.  The targetStream must contain a corresponding instance
    /// for each argument contained in this state (eg. it is not possible to restore the state of an AttributeSource
    /// containing a TermAttribute into a AttributeSource using a Token instance as implementation).
    ///
    /// Note that this method does not affect attributes of the targetStream that are not contained in this state.
    /// In other words, if for example the targetStream contains an OffsetAttribute, but this state doesn't, then
    /// the value of the OffsetAttribute remains unchanged. It might be desirable to reset its value to the default,
    /// in which case the caller should first call {@link TokenStream#clearAttributes()} on the targetStream.
    void restoreState(const AttributeSourceStatePtr& state);

    /// Return hash code for this object.
    virtual int32_t hashCode();

    /// Return whether two objects are equal
    virtual bool equals(const LuceneObjectPtr& other);

    /// Returns a string representation of the object
    virtual String toString();

    /// Performs a clone of all {@link AttributeImpl} instances returned in a new AttributeSource instance. This
    /// method can be used to eg. create another TokenStream with exactly the same attributes (using {@link
    /// #AttributeSource(AttributeSource)})
    AttributeSourcePtr cloneAttributes();

    /// Return a vector of attributes based on currentState.
    Collection<AttributePtr> getAttributes();

protected:
    /// The caller must pass in a className value.
    /// This method checks if an instance of that class is already in this AttributeSource and returns it.
    AttributePtr getAttribute(const String& className);

    /// Returns true, if this AttributeSource contains the passed-in Attribute.
    bool hasAttribute(const String& className);

    void computeCurrentState();
};

class DefaultAttributeFactory : public AttributeFactory {
public:
    virtual ~DefaultAttributeFactory();

    LUCENE_CLASS(DefaultAttributeFactory);

public:
    /// returns an {@link Attribute}.
    virtual AttributePtr createAttributeInstance(const String& className);
};

/// This class holds the state of an AttributeSource.
/// @see #captureState
/// @see #restoreState
class AttributeSourceState : public LuceneObject {
public:
    virtual ~AttributeSourceState();

    LUCENE_CLASS(AttributeSourceState);

protected:
    AttributePtr attribute;
    AttributeSourceStatePtr next;

public:
    virtual LuceneObjectPtr clone(const LuceneObjectPtr& other = LuceneObjectPtr());

    friend class AttributeSource;
};

}

#endif
