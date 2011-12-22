/*
 *   Copyright (c) 2011  Higepon(Taro Minowa)  <higepon@labs.cybozu.co.jp>
 *
 *   Redistribution and use in source and binary forms, with or without
 *   modification, are permitted provided that the following conditions
 *   are met:
 *
 *   1. Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *
 *   2. Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *
 *   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *   A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *   OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *   SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 *   TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 *   PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 *   LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 *   NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#include "./svd_predictor.h"

SVDPredictor::SVDPredictor(const REDSVD::SMatrixXf& m)
    : m_(m),
      red_svd_(REDSVD::RedSVD(m)),
      sv_(red_svd_.singularValues()),
      v_(red_svd_.matrixV()),
      u_(red_svd_.matrixU()) {
}

float SVDPredictor::Predict(int user, int item, int k) {
  assert(k <= sv_.rows());
  float ret = RatingMeanByUser(user);
  for (int i = 0; i < k; i++) {
    ret += u_(user, i) * sv_(i, 0) * v_(item, i);
  }
#if 0
  const Eigen::MatrixXf d =  (u_ * sv_.asDiagonal()) * v_.transpose();
  printf("ret(%d, %d)=%f\n", user, item, d(user, item) +  RatingMeanByUser(user));
#endif
  return ret;
}

float SVDPredictor::RatingMeanByUser(int user) {
  std::tr1::unordered_map<int, float>::iterator it = cache_.find(user);
  if (it != cache_.end()) {
    return (*it).second;
  }
  float sum = 0;
  int n = 0;
  for (int k = 0; k < m_.outerSize(); ++k) {
    for (REDSVD::SMatrixXf::InnerIterator it(m_, k); it; ++it) {
      if (user == it.row()) {
        sum += it.value();
        n++;
      }
    }
  }
  float mean;
  if (n == 0) {
    mean = 0;
  } else {
    mean = sum / n;
  }
  cache_[user] = mean;
  return mean;
}
