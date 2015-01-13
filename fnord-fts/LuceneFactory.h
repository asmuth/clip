/////////////////////////////////////////////////////////////////////////////
// Copyright (c) 2009-2014 Alan Wright. All rights reserved.
// Distributable under the terms of either the Apache License (Version 2.0)
// or the GNU Lesser General Public License.
/////////////////////////////////////////////////////////////////////////////

#ifndef LUCENEFACTORY_H
#define LUCENEFACTORY_H

#include <boost/make_shared.hpp>
#include <boost/version.hpp>

namespace Lucene {

template <class T>
std::shared_ptr<T> newInstance() {
#if BOOST_VERSION <= 103800
    return std::shared_ptr<T>(new T);
#else
    return boost::make_shared<T>();
#endif
}

template <class T, class A1>
std::shared_ptr<T> newInstance(A1 const& a1) {
#if BOOST_VERSION <= 103800
    return std::shared_ptr<T>(new T(a1));
#else
    return boost::make_shared<T>(a1);
#endif
}

template <class T, class A1, class A2>
std::shared_ptr<T> newInstance(A1 const& a1, A2 const& a2) {
#if BOOST_VERSION <= 103800
    return std::shared_ptr<T>(new T(a1, a2));
#else
    return boost::make_shared<T>(a1, a2);
#endif
}

template <class T, class A1, class A2, class A3>
std::shared_ptr<T> newInstance(A1 const& a1, A2 const& a2, A3 const& a3) {
#if BOOST_VERSION <= 103800
    return std::shared_ptr<T>(new T(a1, a2, a3));
#else
    return boost::make_shared<T>(a1, a2, a3);
#endif
}

template <class T, class A1, class A2, class A3, class A4>
std::shared_ptr<T> newInstance(A1 const& a1, A2 const& a2, A3 const& a3, A4 const& a4) {
#if BOOST_VERSION <= 103800
    return std::shared_ptr<T>(new T(a1, a2, a3, a4));
#else
    return boost::make_shared<T>(a1, a2, a3, a4);
#endif
}

template <class T, class A1, class A2, class A3, class A4, class A5>
std::shared_ptr<T> newInstance(A1 const& a1, A2 const& a2, A3 const& a3, A4 const& a4, A5 const& a5) {
#if BOOST_VERSION <= 103800
    return std::shared_ptr<T>(new T(a1, a2, a3, a4, a5));
#else
    return boost::make_shared<T>(a1, a2, a3, a4, a5);
#endif
}

template <class T, class A1, class A2, class A3, class A4, class A5, class A6>
std::shared_ptr<T> newInstance(A1 const& a1, A2 const& a2, A3 const& a3, A4 const& a4, A5 const& a5, A6 const& a6) {
#if BOOST_VERSION <= 103800
    return std::shared_ptr<T>(new T(a1, a2, a3, a4, a5, a6));
#else
    return boost::make_shared<T>(a1, a2, a3, a4, a5, a6);
#endif
}

template <class T, class A1, class A2, class A3, class A4, class A5, class A6, class A7>
std::shared_ptr<T> newInstance(A1 const& a1, A2 const& a2, A3 const& a3, A4 const& a4, A5 const& a5, A6 const& a6, A7 const& a7) {
#if BOOST_VERSION <= 103800
    return std::shared_ptr<T>(new T(a1, a2, a3, a4, a5, a6, a7));
#else
    return boost::make_shared<T>(a1, a2, a3, a4, a5, a6, a7);
#endif
}

template <class T, class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8>
std::shared_ptr<T> newInstance(A1 const& a1, A2 const& a2, A3 const& a3, A4 const& a4, A5 const& a5, A6 const& a6, A7 const& a7, A8 const& a8) {
#if BOOST_VERSION <= 103800
    return std::shared_ptr<T>(new T(a1, a2, a3, a4, a5, a6, a7, a8));
#else
    return boost::make_shared<T>(a1, a2, a3, a4, a5, a6, a7, a8);
#endif
}

template <class T, class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9>
std::shared_ptr<T> newInstance(A1 const& a1, A2 const& a2, A3 const& a3, A4 const& a4, A5 const& a5, A6 const& a6, A7 const& a7, A8 const& a8, A9 const& a9) {
#if BOOST_VERSION <= 103800
    return std::shared_ptr<T>(new T(a1, a2, a3, a4, a5, a6, a7, a8, a9));
#else
    return boost::make_shared<T>(a1, a2, a3, a4, a5, a6, a7, a8, a9);
#endif
}

template <class T>
std::shared_ptr<T> newLucene() {
    std::shared_ptr<T> instance(newInstance<T>());
    instance->initialize();
    return instance;
}

template <class T, class A1>
std::shared_ptr<T> newLucene(A1 const& a1) {
    std::shared_ptr<T> instance(newInstance<T>(a1));
    instance->initialize();
    return instance;
}

template <class T, class A1, class A2>
std::shared_ptr<T> newLucene(A1 const& a1, A2 const& a2) {
    std::shared_ptr<T> instance(newInstance<T>(a1, a2));
    instance->initialize();
    return instance;
}

template <class T, class A1, class A2, class A3>
std::shared_ptr<T> newLucene(A1 const& a1, A2 const& a2, A3 const& a3) {
    std::shared_ptr<T> instance(newInstance<T>(a1, a2, a3));
    instance->initialize();
    return instance;
}

template <class T, class A1, class A2, class A3, class A4>
std::shared_ptr<T> newLucene(A1 const& a1, A2 const& a2, A3 const& a3, A4 const& a4) {
    std::shared_ptr<T> instance(newInstance<T>(a1, a2, a3, a4));
    instance->initialize();
    return instance;
}

template <class T, class A1, class A2, class A3, class A4, class A5>
std::shared_ptr<T> newLucene(A1 const& a1, A2 const& a2, A3 const& a3, A4 const& a4, A5 const& a5) {
    std::shared_ptr<T> instance(newInstance<T>(a1, a2, a3, a4, a5));
    instance->initialize();
    return instance;
}

template <class T, class A1, class A2, class A3, class A4, class A5, class A6>
std::shared_ptr<T> newLucene(A1 const& a1, A2 const& a2, A3 const& a3, A4 const& a4, A5 const& a5, A6 const& a6) {
    std::shared_ptr<T> instance(newInstance<T>(a1, a2, a3, a4, a5, a6));
    instance->initialize();
    return instance;
}

template <class T, class A1, class A2, class A3, class A4, class A5, class A6, class A7>
std::shared_ptr<T> newLucene(A1 const& a1, A2 const& a2, A3 const& a3, A4 const& a4, A5 const& a5, A6 const& a6, A7 const& a7) {
    std::shared_ptr<T> instance(newInstance<T>(a1, a2, a3, a4, a5, a6, a7));
    instance->initialize();
    return instance;
}

template <class T, class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8>
std::shared_ptr<T> newLucene(A1 const& a1, A2 const& a2, A3 const& a3, A4 const& a4, A5 const& a5, A6 const& a6, A7 const& a7, A8 const& a8) {
    std::shared_ptr<T> instance(newInstance<T>(a1, a2, a3, a4, a5, a6, a7, a8));
    instance->initialize();
    return instance;
}

template <class T, class A1, class A2, class A3, class A4, class A5, class A6, class A7, class A8, class A9>
std::shared_ptr<T> newLucene(A1 const& a1, A2 const& a2, A3 const& a3, A4 const& a4, A5 const& a5, A6 const& a6, A7 const& a7, A8 const& a8, A9 const& a9) {
    std::shared_ptr<T> instance(newInstance<T>(a1, a2, a3, a4, a5, a6, a7, a8, a9));
    instance->initialize();
    return instance;
}

}

#endif
