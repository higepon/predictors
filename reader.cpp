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
#include <vector>
#include <algorithm>
#include <map>
#include "./reader.h"

using std::vector;
using std::string;
using std::map;
using std::pair;
using std::istringstream;
using std::stringstream;
using std::ifstream;
using std::make_pair;
using std::max;

// redsvd by Daisuke Okanohara
static void convertFV2Mat(const vector<REDSVD::fv_t>& fvs,
                          REDSVD::SMatrixXf& A) {
  int maxID = 0;
  size_t nonZeroNum = 0;
  for (size_t i = 0; i < fvs.size(); ++i) {
    const REDSVD::fv_t& fv(fvs[i]);
    for (size_t j = 0; j < fv.size(); ++j) {
      maxID = max(fv[j].first+1, maxID);
    }
    nonZeroNum += fv.size();
  }
  A.resize(fvs.size(), maxID);
  A.reserve(nonZeroNum);
  for (size_t i = 0; i < fvs.size(); ++i) {
    A.startVec(i);
    const REDSVD::fv_t& fv(fvs[i]);
    for (size_t j = 0; j < fv.size(); ++j) {
      A.insertBack(i, fv[j].first) = fv[j].second;
    }
  }
  A.finalize();
}

void MovieLensToRedSVD(const string& path) {
  map<int, vector< pair<int, float> > > ret;
  ifstream in;
  in.open(path.c_str());
  if (in.is_open()) {
    while (!in.eof()) {
      string line;
      getline(in, line);
      int user;
      int item;
      int rating;
      stringstream ss(line);
      ss >> user;
      ss >> item;
      ss >> rating;
      ret[user].push_back(pair<int, float>(item, rating));
    }
    in.close();
  }
  for (map<int, vector< pair<int, float> > >::const_iterator it = ret.begin();
       it != ret.end(); ++it) {
    for (vector< pair<int, float> >::const_iterator it2 = (*it).second.begin();
         it2 != (*it).second.end(); ++it2) {
      printf("%d:%f ", (*it2).first, (*it2).second);
    }
    printf("\n");
  }
}

static void ReadLine(const string& line, REDSVD::fv_t& fv) {
  istringstream is(line);
  int id;
  char sep;
  float val;
  while (is >> id >> sep >> val) {
    fv.push_back(make_pair(id, val));
  }
  sort(fv.begin(), fv.end());
  fv.erase(unique(fv.begin(), fv.end()), fv.end());
}

void ReadMatrix(const string& fn, REDSVD::SMatrixXf& A) {
  vector<REDSVD::fv_t> fvs;
  ifstream ifs(fn.c_str());
  if (!ifs) {
    throw string("failed to open") + fn;
  }

  for (string line; getline(ifs, line); ) {
    REDSVD::fv_t fv;
    ReadLine(line, fv);
    fvs.push_back(fv);
  }
  convertFV2Mat(fvs, A);
}
