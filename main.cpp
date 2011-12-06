/*
 *   Copyright (c) 2011  Higepon(Taro Minowa) <higepon@users.sourceforge.jp>
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
#include <string>
#include "./reader.h"
#include "./svd_predictor.h"

using REDSVD::SMatrixXf;

static float CalcMAE(const SVDPredictor& p,
                     const SMatrixXf& learn_matrix,
                     const SMatrixXf& check_matrix,
                     int k) {
    int n = 0;
    float diff_sum = 0;
    for (int j = 0; j < check_matrix.outerSize(); ++j) {
      for (SMatrixXf::InnerIterator it(check_matrix, j); it; ++it) {
        int user = it.row();
        int item = it.col();
        if (user < learn_matrix.rows() && item < learn_matrix.cols()) {
          float rating = p.Predict(user, item, k);
          float real_rating = it.value();
          diff_sum += fabs(real_rating - rating);
          n++;
        }
      }
    }
    return diff_sum / n;
}

int main(int argc, char *argv[]) {
  if (argc == 2 && std::string(argv[1]) == "gen") {
    //    MovieLensToRedSVD("/Users/taro/Desktop/ml-100k/u.check.data");
    MovieLensToRedSVD("/Users/taro/Desktop/ml-100k/u.data");
    return 0;
  }

  const char* ANSWER_FILE = "/Users/taro/higepon/svd/check.data";
  //    const char* LEARNING_FILE = "/Users/taro/higepon/svd/learning.data";
    const char* LEARNING_FILE = "/Users/taro/higepon/svd/all.data";
  SMatrixXf learn_matrix;
  SMatrixXf answer_matrix;
  ReadMatrix(LEARNING_FILE, learn_matrix);
  ReadMatrix(ANSWER_FILE, answer_matrix);
  SVDPredictor p(learn_matrix);
  for (int k = 10; k <= p.NumSingularValues();) {
    printf("%d,%f\n", k, CalcMAE(p, answer_matrix, learn_matrix, k));
    k += 30;
  }
  return 0;
}
