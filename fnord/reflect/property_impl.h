/**
 * This file is part of the "FnordMetric" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * FnordMetric is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License v3.0. You should have received a
 * copy of the GNU General Public License along with this program. If not, see
 * <http://www.gnu.org/licenses/>.
 */
namespace fnord {
namespace reflect {

template <typename ClassType, typename TargetType>
PropertyReader<ClassType, TargetType>::PropertyReader(
    TargetType* target) :
    target_(target) {}

template <typename ClassType, typename TargetType>
const ClassType& PropertyReader<ClassType, TargetType>::instance() const {
  return instance_;
}

template <typename ClassType, typename TargetType>
template <typename PropertyType>
void PropertyReader<ClassType, TargetType>::prop(
    PropertyType prop,
    uint32_t id,
    const std::string& prop_name,
    bool optional) {
  instance_.*prop = target_->template getProperty<
      typename std::decay<decltype(instance_.*prop)>::type>(
          id,
          prop_name);
}

template <typename ClassType, typename TargetType>
PropertyWriter<ClassType, TargetType>::PropertyWriter(
    const ClassType& instance,
    TargetType* target) :
    instance_(instance),
    target_(target) {}

template <typename ClassType, typename TargetType>
template <typename PropertyType>
void PropertyWriter<ClassType, TargetType>::prop(
    PropertyType prop,
    uint32_t id,
    const std::string& prop_name,
    bool optional) {
  target_->putProperty(id, prop_name, instance_.*prop);
}

template <class ClassType>
template <class TargetType>
void MetaClass<ClassType>::serialize(
    const ClassType& instance,
    TargetType* target) {
  PropertyWriter<ClassType, TargetType> writer(instance, target);
  reflect(&writer);
}

template <class ClassType>
template <class TargetType>
ClassType MetaClass<ClassType>::unserialize(
    TargetType* target) {
  PropertyReader<ClassType, TargetType> reader(target);
  reflect(&reader);
  return reader.instance();
}


}
}
