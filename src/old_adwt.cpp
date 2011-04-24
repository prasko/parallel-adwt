#include <vector>
#include <list>
#include <cstdio>

using namespace std;

inline int oddIndex(int index) { return index * 2 + 1; }
inline int evenIndex(int index) { return index * 2; }

void printList(list<double> &l) {
  for(list<double>::iterator lit = l.begin(); lit != l.end(); ++lit) {
    printf("%.3lf\t", *lit);
  }
  printf("\n");
}

void compressArray(vector<double> &input, 
		   vector<double> &outA, 
		   vector<double> &outD) {

  int inputSize = (int)input.size();

  // adjust size

  if(inputSize % 2) {
    input.push_back(0.0);
    inputSize++;
  }

  // decimate input array

  list<double> workA;
  list<double> workD;
  
  for(int i = 0; i < inputSize; ++i) {
    if(i % 2) {
      workD.push_back(input[i]);
    } else {
      workA.push_back(input[i]);
    }
  }

  // *** INTERPOLATION ***

  // first degree interpolation

  list<double> interpolation1;

  double pastValue1 = 0.0;

  for(list<double>::iterator lit = workA.begin(); lit != workA.end(); ++lit) {
    double currValue = *lit;
    interpolation1.push_back((pastValue1 + currValue) / 2); 
    pastValue1 = currValue;		     
  }

  interpolation1.push_back(pastValue1 / 2);
  
  // delay correction

  workA.push_front(0.0);
  workD.push_front(0.0);

  interpolation1.push_front(0.0);
  interpolation1.push_back(0.0);

  printf("Linear interpolation:\n");
  printList(interpolation1);
  printf("\n");

  // third degree interpolation

  list<double> interpolation3;

  pastValue1 = 0.0;
  double pastValue2 = 0.0;

  interpolation1.push_back(0.0);

  for(list<double>::iterator lit = interpolation1.begin(); 
      lit != interpolation1.end(); ++lit) {

    if(lit == interpolation1.begin()) {
      continue;
    }

    double currValue = *lit;

    interpolation3.push_back((pastValue1 - 2 * pastValue2 + currValue) / 8);

    pastValue1 = pastValue2;
    pastValue2 = currValue;
  }

  interpolation1.pop_back();

  // delay correction
  
  workA.push_back(0.0);
  workA.push_front(0.0);
  workD.push_back(0.0);
  workD.push_front(0.0);

  printf("Cubic interpolation:\n");
  printList(interpolation3);
  printf("\n");

  // sum down

  list<double>::iterator dLit = workD.begin();
  list<double>::iterator i1Lit = interpolation1.begin();
  list<double>::iterator i3Lit = interpolation3.begin();

  while(dLit != workD.end()) {
    
    *dLit = *dLit - *i1Lit + *i3Lit;

    dLit++;
    i1Lit++;
    i3Lit++;
  }

  // *** UPDATE ***

  // linear update
  
  list<double> update1;

  workD.push_back(0.0);
  
  pastValue1 = 0.0;
  for(list<double>::iterator lit = workD.begin(); lit != workD.end(); ++lit) {
    double currValue = *lit;

    update1.push_back((pastValue1 + currValue) / 4);

    pastValue1 = currValue;
  }

  workD.pop_back();

  // delay correction

  workD.push_front(0.0);
  workA.push_back(0.0);
  
  // sum up

  list<double>::iterator up1Lit = update1.begin();
  list<double>::iterator aLit = workA.begin();

  while(aLit != workA.end()) {

    *aLit += *up1Lit;

    aLit++;
    up1Lit++;
  }

  // copy back

  for(aLit = workA.begin(); aLit != workA.end(); ++aLit) {
    outA.push_back(*aLit);
  }

  for(dLit = workD.begin(); dLit != workD.end(); ++dLit) {
    outD.push_back(*dLit);
  }

}
