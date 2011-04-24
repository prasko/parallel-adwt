/*
  Copyright: (c) 2011 Matija Osrecki <matija.osrecki@fer.hr>
 */

#include <vector>

#include "sysutil.h"

void Decimator::decimate(const signal &s, signal &re, signal &ro) {
    for(int i = 0; i < (int)s.size(); ++i) {
      if(i % 2 == 0) 
        re.push_back(s[i] * constant_);
      else 
        ro.push_back(s[i] * constant_);
    }
}

template <typename T> 
inline T vector_access(const std::vector<T> &array, const int i) {
  return (i < 0 || i >= (int)array.size() ? 0 : array[i]);
}

void PolySystem::addMember(const int offset, const double coef) {
  members_.push_back(std::make_pair(offset, coef));
}

void PolySystem::process(const signal &s, signal &res) {
  for(int i = 0; i < (int)s.size(); ++i) {
    double sum = 0;

    for(int j = 0; j < (int)members_.size(); ++j)
      sum += members_[j].second * vector_access(s, i - members_[j].first);

    res.push_back(sum * constant_);
  }
}

void Multiply2::multiply(const signal &s1, const signal &s2, signal &res) {
  for(int i = 0; i < (int)std::min(s1.size(), s2.size()); ++i) {
    res.push_back(s1[i] * s2[i] * constant_);
  }

  res.resize(std::max(s1.size(), s2.size()));
}

void Add2::add(const signal &s1, const signal &s2, const double c2, signal &r) {
  int n = s1.size(), m = s2.size(), l = std::max(n, m);

  for(int i = 0; i < l; ++i) {
    if(i >= n) {
      r.push_back(s2[i] * constant_);
    } else if(i >= m) {
      r.push_back(s1[i] * constant_);
    } else {
      r.push_back((s1[i] + s2[i] * c2) * constant_);
    }
  }
}

void Sumator::init(signal &result) { 
  result_ = &result; 
}

void Sumator::sumSignal(const signal &signal, const double coef) {
  if(signal.size() > result_->size())
    result_->resize(signal.size());
    
  for(int i = 0; i < (int)signal.size(); ++i) 
    *(result_->begin() + i) += coef * signal[i];
}
