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

static float CalcMAE(SVDPredictor& p,
                     const SMatrixXf& training_matrix,
                     const SMatrixXf& evaluation_matrix,
                     int k) {
    int n = 0;
    float diff_sum = 0;
    for (int j = 0; j < evaluation_matrix.outerSize(); ++j) {
      for (SMatrixXf::InnerIterator it(evaluation_matrix, j); it; ++it) {
        int user = it.row();
        int item = it.col();
        if (user < training_matrix.rows() && item < training_matrix.cols()) {
          float rating = p.Predict(user, item, k);
          float real_rating = it.value();
          //printf("(%d %d) rating=%f real_rating=%f\n", user, item, rating, real_rating);
          diff_sum += fabs(real_rating - rating);
          //printf("%f %d\n", diff_sum, n);
          n++;
        }
      }
    }

    return diff_sum / n;
}

int main(int argc, char *argv[]) {
  if (argc != 3) {
    return -1;
  }

  // MovidLens data format to sparse file format
  if (std::string(argv[1]) == "gen") {
    MovieLensToRedSVD(argv[2]);
    return 0;
  }

  std::string training_file = argv[1];
  std::string evaluation_file = argv[2];
  SMatrixXf training_matrix;
  SMatrixXf evaluation_matrix;
  ReadMatrix(training_file, training_matrix);

  ReadMatrix(evaluation_file, evaluation_matrix);

  // test
#if 0
  printf("evaluation_matrix(943, 1188) should be 3 = %f\n", evaluation_matrix.coeffRef(943, 1188));
  printf("training_matrix(943, 403) should be 4 = %f\n", training_matrix.coeffRef(943, 403));
  REDSVD::RedSVD red_svd(training_matrix);
  const Eigen::MatrixXf d =  (red_svd.matrixU() * red_svd.singularValues().asDiagonal()) * red_svd.matrixV().transpose();
  printf("ret(%d, %d)=%f should be nearly 4\n", 943, 403, d(943, 403));
  printf("p(943, 1188)=%f should be nearly 3 \n", p.Predict(943, 1188, 800));
  // test
#endif
  SVDPredictor p(training_matrix);

  printf("=== %s \n", training_file.c_str());
  for (int k = 10; k <= p.NumSingularValues();) {
    printf("%d,%f\n", k, CalcMAE(p, training_matrix, evaluation_matrix, k));
    k += 30;
  }
  return 0;
}
