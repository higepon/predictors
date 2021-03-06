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
#ifndef SVD_SVD_PREDICTOR_H_
#define SVD_SVD_PREDICTOR_H_

#include <tr1/unordered_map>
#include "redsvd.hpp"

class SVDPredictor {
 public:
  explicit SVDPredictor(const REDSVD::SMatrixXf& m);
  float Predict(int user, int item, int k);
  int NumSingularValues() const {
    return sv_.rows();
  }

 private:
  float RatingMeanByUser(int user);

  const REDSVD::SMatrixXf& m_;
  REDSVD::RedSVD red_svd_;
  const Eigen::VectorXf& sv_;
  const Eigen::MatrixXf& v_;
  const Eigen::MatrixXf& u_;
  std::tr1::unordered_map<int, float> cache_;
};

#endif  // SVD_SVD_PREDICTOR_H_
