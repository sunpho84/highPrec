#include <iostream>

#include "highPrec.hpp"

using namespace std;

int main()
{
  PrecFloat::setDefaultPrecision(1024);
  
  const int n=128;
  
  PrecMatr m(n,n);
  m.setRandom();
  
  const PrecMatr i=m.inverse();
  
  cout<<"Check inversion: "<<(i*m-PrecMatr::Identity(n,n)).squaredNorm().get()<<" "<<(m*i-PrecMatr::Identity(n,n)).squaredNorm().get()<<endl;
  
  return 0;
}
