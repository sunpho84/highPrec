#ifndef _HPINTEGRATOR_HPP
#define _HPINTEGRATOR_HPP

#include <PrecFloat.hpp>

#include <iostream>

/// Integrate the passed function f with the double exponential
/// transformation of eq.1.16 of
/// https://core.ac.uk/download/pdf/82080804.pdf
///
/// The function c takes into account the jacobian and the change of variable
///
/// The trapizio integration is carried out recursively, halving the
/// stepsize until the maximal relative precision is reached.
///
/// The value at zero is evaluated before everything and used as a
/// first estimate of the integral, I0. Then, at each iteration i, the
/// value I(i-1) is considered as \sum_i=0^n c(x*2*i*step)*2*step
/// Then, the estimate I(i) is obtained as
/// I(i) = I(i-1)/2 + \sum_i=0^n c(x*(2*i+1)*step)*step
///
/// n is taken to be the first point where the term is negligible
///
/// n might have to be adapted during the i-itertion, in case the
/// needed part of the full summation is re-evaluated
///
/// The iterative procedure is stopped when the result is stable
/// within attainable precisione
template <typename F>
PrecFloat precIntegrateUpToInfinity(F&& f,const double& xMin=0.0)
{
  /// We compute up to the highest precision possible, which needs to
  /// be adjusted in terms of the number of iterations (maybe it might
  /// be enough to increase with the square root?)
  PrecFloat maxAttainableStability=
    PrecFloat::getEpsilon()*10;
  
  const PrecFloat piHalf=
    precPi()/2;
  
  auto c=
    [&f,&piHalf,&xMin](const PrecFloat& t)
    {
      const PrecFloat s=sinh(t);
      const PrecFloat x=exp(piHalf*s)+xMin;
      const PrecFloat jac=piHalf*exp(piHalf*s)*cosh(t);
      const PrecFloat res=f(x)*jac;
      
      // cout<<" t: "<<t<<" x: "<<x<<" res: "<<res<<" jac: "<<jac<<endl;
      
      return res;
    };
  
  PrecFloat sum=c(0)*2;
  PrecFloat extreme=0;
  PrecFloat step=1;
  PrecFloat precSum;
  PrecFloat stability;
  
  do
    {
      precSum=sum;
      
      bool converged=
	false;
      
      sum/=2;
      PrecFloat t=step;
      
      const PrecFloat doubleStep=
	step*2;
      
      bool exitTheLoop=
	false;
      
      while(not exitTheLoop)
	{
	  const PrecFloat contr=
	    c(t)+c(-t);
	  
	  const PrecFloat newSum=
	    sum+contr*step;
	  
	  // using namespace std;
	  // cout<<"t: "<<t<<" step: "<<step<<" contr: "<<contr<<" t>extreme: "<<(t>extreme)<<" converged: "<<converged<<endl;
	  
	  converged=
	    (newSum==sum);
	  
	  exitTheLoop=
	    (converged and t>extreme);
	  
	  if(t>extreme)
	    {
	      extreme=t;
	      t+=step;
	    }
	  else
	    t+=doubleStep;
	  
	  sum=newSum;
	};
      
      step/=2;
      
      // cout<<(sum-1)<<endl;
      
      stability=abs(sum/precSum-1);
       std::cout<<"Stability: "<<stability<<std::endl;
      maxAttainableStability*=2;
      // cout<<"MaxAttainableStability: "<<maxAttainableStability<<endl;
    }
  while(stability>maxAttainableStability);
  
  return sum;
}

#endif
