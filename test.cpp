#include <iostream>

#include "highPrec.hpp"

using namespace std;

int main()
{
  PrecFloat::setDefaultPrecision(1024);
  
  const int n=128;
  
  PrecMatr m(n,n);
  m.setRandom();

  PrecVect v(n);
  v.setRandom();
  
  const PrecMatr i=
    m.inverse();
  
  const PrecFloat matrixForm=
    v.transpose()*i*v;
  cout<<matrixForm.get()<<endl;
  
  cout<<"Check inversion: "<<(i*m-PrecMatr::Identity(n,n)).squaredNorm().get()<<" "<<(m*i-PrecMatr::Identity(n,n)).squaredNorm().get()<<endl;
  
  /////////////////////////////////////////////////////////////////
  
  cout<<"Integration test"<<endl;
  PrecFloat::setDefaultPrecision(1024);
  cout.precision(PrecFloat::getNDigits());
  
  const PrecFloat numeric=
    integrateBetween0andInfinite([](const PrecFloat& x)
    {
      return exp(-sqr(x-0.5)/2);
    });
  cout<<numeric<<endl;
  
  const PrecFloat exact=
    sqrt(precPi()/2)*erfc(-(PrecFloat)0.5/sqrt(PrecFloat(2)));
  cout<<exact<<endl;

  cout.precision(16);
  cout<<"Relative error: "<<(numeric/exact-1)<<endl;
  
  return 0;
}
