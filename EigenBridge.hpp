#ifndef _EIGENBRIDGE_HPP
#define _EIGENBRIDGE_HPP

#include <eigen3/Eigen/Dense>

#include <PrecFloat.hpp>

// Tell Eigen how to deal with PrecFloat numbers

namespace Eigen
{
  template<>
  struct NumTraits<PrecFloat> :
    GenericNumTraits<PrecFloat>
  {
    typedef PrecFloat Real;
    typedef PrecFloat NonInteger;
    typedef PrecFloat Nested;
    
    static inline Real epsilon()
    {
      return 0;
    }
    
    static inline Real dummy_precision()
    {
      return 0;
    }
    
    static inline int digits10()
    {
      return 0;
    }
    
    enum
      {
	IsInteger=0,
	IsSigned=1,
	IsComplex=0,
	RequireInitialization=1,
	ReadCost=6,
	AddCost=150,
	MulCost=100
      };
  };
}

using PrecVect=
  Eigen::Matrix<PrecFloat,Eigen::Dynamic,1>;

using PrecMatr=
  Eigen::Matrix<PrecFloat,Eigen::Dynamic,Eigen::Dynamic>;


#endif
