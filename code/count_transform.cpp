#include<iostream>
#include<cstdio>
#include<cstdlib>
#include<fstream>
#include<algorithm>
#include<cmath>
#include<cstring>

using namespace std;

const int W = 1936;
const int H = 2592;
const int N = W * H;
const int P = 10;
const double PI = 3.1415926;

double x[N], y[N];
double r;
int cnt[W][H];
bool mk[W][H][P];
double seg[P + 1];
int n;

inline double sqr(double x) {
  return x * x;
}

double dist(double x1, double y1, double x2, double y2) {
  return sqrt(sqr(x1 - x2) + sqr(y1 - y2));
}

void output() {
  int min = n, max = 0;
  int maxi, maxj;
  for (int i = 0; i < W; ++i)
    for (int j = 0; j < H; ++j) {
      if (cnt[i][j] < min)
	min = cnt[i][j];
      if (cnt[i][j] > max)
	max = cnt[i][j], maxi = i, maxj = j;
    }
  printf("%d %d\n", min, max);
  printf("%d %d\n", maxi, maxj);
  for (int i = 0; i < W; ++i){
    for (int j = 0; j < H; ++j)
      printf("%d ", cnt[i][j]);
    printf("\n");
  }
}

void prepare() {
  double step = 2 * PI / P, theta = -PI;
  for (int t = 0; t <= P ; ++t, theta += step)
    seg[t] = theta;

}

void mark() {
  prepare();
  memset(mk, 0, sizeof(mk));
  memset(cnt, 0, sizeof(cnt));

  int L = (int)sqrt(r) + 1;
  for (int k = 0; k < n; ++k) {
    for (int i = (int)x[k] - L; i <= (int)x[k] + L; ++i)
      if (i >= 0 && i < W)
	for (int j = (int)y[k] - L; j <= (int)y[k] + L; ++j)
	  if(j >= 0 && j < H) {
	    double theta = atan2(y[k] - j, x[k] - i);
	    for (int t = 0; t < P; ++t)
	      if (theta > seg[t] && theta <= seg[t + 1]) {
		mk[i][j][t] = 1;
		break;
	      }
	  }
  }
  for (int i = 0; i < W; ++i)
    for (int j = 0; j < H; ++j)
      for (int t = 0; t < P; ++t)
	if (mk[i][j][t])
	  ++cnt[i][j];
}

void work() {
  memset(cnt, 0, sizeof(cnt));

  int L = (int)sqrt(r) + 1;
  for (int k = 0; k < n; ++k)
    for (int i = (int)x[k] - L; i <= (int)x[k] + L; ++i)
      if (i >= 0 && i < W)
	for (int j = (int)y[k] - L; j <= (int)y[k] + L; ++j)
	  if(j >= 0 && j < H)
	      ++cnt[i][j];
}

int main(int argc, const char* argv[]) {
  ifstream fin;

  if (argc < 3)
    return 0;

  n = 0;
  r = atof(argv[2]);
  fin.open(argv[1], ios::in);
  while (!fin.eof()) {
    fin >> x[n] >> y[n];
    ++n;
  }
  //  work();
  mark();
  output();
  fin.close();
  return 1;
}
