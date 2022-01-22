#include <iostream>

#include <highPrec.hpp>

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
  
  const double x0=0.5,s=0.05;
  
  /// Gaussian centered in x0 with sigma=s
  const auto f=
    [&x0,&s](const PrecFloat& x)
    {
      return exp(-sqr((x-x0)/s)/2);
    };
  
  const PrecFloat numeric=
    integrateUpToInfinite(f);
  cout<<numeric<<endl;
  
  const PrecFloat exact=
    sqrt(precPi()/2)*s*erfc(-(PrecFloat)x0/(sqrt(PrecFloat(2))*s));
  cout<<exact<<endl;
  
  cout.precision(16);
  cout<<"Relative error: "<<(numeric/exact-1)<<endl;
  
  return 0;
}
