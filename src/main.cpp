#include <cstdio>
#include <vector>

using namespace std;

void adwt(std::vector<double> &input,
          std::vector<double> &A,
          std::vector<double> &D);

int main() {
  int width;

  scanf("%d", &width);

  vector<double> input(width);
  
  for(int i = 0; i < width; ++i) {
    scanf("%lf", &input[i]);
  }
  
  vector<double> resultA;
  vector<double> resultD;

  adwt(input, resultA, resultD);

  
  return 0;
}
