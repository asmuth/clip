/**
 * This file is part of the "plotfx" project
 *   Copyright (c) 2014 Paul Asmuth, Google Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 
 * * Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 * 
 * * Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 * 
 * * Neither the name of the copyright holder nor the names of its
 *   contributors may be used to endorse or promote products derived from
 *   this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include "domainprovider.h"

namespace plotfx {
namespace chart {

DomainProvider::DomainProvider(
    AnyDomain* domain /* = nullptr */) :
    domain_(domain),
    free_on_destroy_(false) {};

DomainProvider::~DomainProvider() {
  if (free_on_destroy_) {
    delete domain_;
  }
}

AnyDomain* DomainProvider::get() const {
  return domain_;
}

bool DomainProvider::empty() const {
  return domain_ == nullptr;
}

void DomainProvider::reset(
    AnyDomain* domain,
    bool free_on_destroy /* = false */) {
  if (free_on_destroy_) {
    delete domain_;
  }

  domain_ = domain;
  free_on_destroy_ = free_on_destroy;
}

const std::vector<double> DomainProvider::getTicks() const {
  if (empty()) {
    return std::vector<double>{};
  } else {
    return domain_->getTicks();
  }
}

const std::vector<std::pair<double, std::string>>
    DomainProvider::getLabels() const {
  if (empty()) {
    return std::vector<std::pair<double, std::string>>{};
  } else {
    return domain_->getLabels();
  }
}


}
}
