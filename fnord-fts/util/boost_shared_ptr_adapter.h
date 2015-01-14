#ifndef __BOOST_SHAREDPTR_ADAPTER_H_
#define __BOOST_SHAREDPTR_ADAPTER_H_

/* Use, modification and distribution is subject to the
 * Boost Software License, Version 1.0. (See accompanying
 * file LICENSE_1_0.txt or http://www.boost.org/LICENSE_1_0.txt)
 */
namespace boost {
  template<class T> const T* get_pointer(const std::shared_ptr<T>& ptr) 
  {
    return ptr.get();
  }

  template<class T> T* get_pointer(std::shared_ptr<T>& ptr)
  {
    return ptr.get();
  }
}

#endif
