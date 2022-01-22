#include <eigen3/Eigen/Dense>

#include <mpfr.h>

/// Undef this to disable actual usage of the high preceision
//#define FAKE_HP

/// Structure to represent arbitray precision real number
struct PrecFloat
{
#ifndef FAKE_HP
  static inline int& _defaultPrecision()
  {
    static int _val;
    
    return _val;
  }
#endif
  
  /// Sets the default precision
  static void setDefaultPrecision(const int& n)
  {
#ifndef FAKE_HP
    _defaultPrecision()=n;
#endif
  }
  
  /// Gets the default precision
  static int getDefaultPrecision()
  {
    return
#ifndef FAKE_HP
      _defaultPrecision()
#else
      53
#endif
      ;
  }
  
  /// Gets the number of decimal digits that can be printed
  static int getNDigits()
  {
    return
#ifndef FAKE_HP
      getDefaultPrecision()*log10(2.0)
#else
      16
#endif
      ;
  }
  
  /// Returns the current smaller number
  static PrecFloat getEpsilon()
  {
    return
      pow((PrecFloat)2,-getDefaultPrecision());
  }
  
  /// Storage
#ifndef FAKE_HP
  mpfr_t data{};
#else
  double data;
#endif
  
  inline friend std::ostream& operator<<(std::ostream& os,const PrecFloat& f)
  {
#ifndef FAKE_HP
    const bool fixed=
      os.flags()&std::ios_base::fixed;
       
    constexpr int lenFormat=10;
    char format[lenFormat];
    snprintf(format,lenFormat,"%%" "." "%td" "R%c",os.precision(),(fixed?'f':'g'));
    
    constexpr int lenOut=1024;
    char out[lenOut];
    mpfr_snprintf(out,lenOut,format,f.data);
    
    os<<out;
#else
    os<<f.data;
#endif
    
    return os;
  }
  
  /// Returns the internal data
  double get() const
  {
#ifdef FAKE_HP
    return data;
#else
    return
      mpfr_get_d(data,MPFR_RNDD);
#endif
  }
  
  /// Assignment
  PrecFloat& operator=(const double& in)
  {
#ifdef FAKE_HP
    data=in;
#else
    mpfr_set_d(data,in,MPFR_RNDD);
#endif
    
    return
      *this;
  }
  
  /// Assign from another number
  PrecFloat& operator=(const PrecFloat& oth)
  {
#ifdef FAKE_HP
    data=oth.data;
#else
    mpfr_set(data,oth.data,MPFR_RNDD);
#endif
    
    return
      *this;
  }
  
  /// Default initialization
  PrecFloat()
  {
#ifdef FAKE_HP
#else
    mpfr_init2(data,_defaultPrecision());
#endif
  }
  
  /// Copy constructor
  PrecFloat(const PrecFloat& oth) :
    PrecFloat()
  {
#ifdef FAKE_HP
    data=oth.data;
#else
    mpfr_set(data,oth.data,MPFR_RNDD);
#endif
  }
  
  /////////////////////////////////////////////////////////////////
  
#ifdef FAKE_HP
#define PROVIDE_CONVERSION_FROM(TYPE,MPFR_TAG)		\
  PrecFloat(const TYPE& in)				\
  {							\
    data=in;						\
  }
#else
#define PROVIDE_CONVERSION_FROM(TYPE,MPFR_TAG)		\
  PrecFloat(const TYPE& in) :				\
    PrecFloat()						\
  {							\
    mpfr_set_ ## MPFR_TAG(data,in,MPFR_RNDD);		\
  }
#endif
  
  PROVIDE_CONVERSION_FROM(double,d);
  PROVIDE_CONVERSION_FROM(int,si);
  PROVIDE_CONVERSION_FROM(unsigned long int,ui);
  
#undef PROVIDE_CONVERSION_FROM
  
  /// Destructor
  ~PrecFloat()
  {
#ifdef FAKE_HP
#else
    mpfr_clear(data);
#endif
  }
  
  /////////////////////////////////////////////////////////////////
  
#ifdef FAKE_HP
#define BINARY_HELPER(NAME,MPFR_NAME)		\
  out.data=NAME(in1.data,in2.data)
#else
#define BINARY_HELPER(NAME,MPFR_NAME)			\
  MPFR_NAME(out.data,in1.data,in2.data,MPFR_RNDD)
#endif
  
#define PROVIDE_BINARY_FUNCTION(NAME,MPFR_NAME)		\
  							\
  inline friend PrecFloat NAME(const PrecFloat& in1,	\
			       const PrecFloat& in2)	\
  {							\
    PrecFloat out;					\
    							\
    BINARY_HELPER(NAME,MPFR_NAME);			\
    							\
    return						\
      out;						\
  }
  
  PROVIDE_BINARY_FUNCTION(pow,mpfr_pow)
  
#undef PROVIDE_BINARY_FUNCTION
#undef BINARY_HELPER
  
  /////////////////////////////////////////////////////////////////
  
#ifdef FAKE_HP
#define BINARY_COMPARISON_OPERATOR_HELPER(NAME,MPFR_NAME)	\
  data NAME in.data
#else
#define BINARY_COMPARISON_OPERATOR_HELPER(NAME,MPFR_NAME)	\
  MPFR_NAME(data,in.data)
#endif
  
#define PROVIDE_BINARY_COMPARISON_OPERATOR(NAME,MPFR_NAME)	\
  								\
  inline bool operator NAME(const PrecFloat& in) const	\
  {								\
    return							\
      BINARY_COMPARISON_OPERATOR_HELPER(NAME,MPFR_NAME);	\
  }

  PROVIDE_BINARY_COMPARISON_OPERATOR(<,mpfr_less_p);
  PROVIDE_BINARY_COMPARISON_OPERATOR(<=,mpfr_lessequal_p);
  PROVIDE_BINARY_COMPARISON_OPERATOR(>,mpfr_greater_p);
  PROVIDE_BINARY_COMPARISON_OPERATOR(>=,mpfr_greaterequal_p);
  PROVIDE_BINARY_COMPARISON_OPERATOR(==,mpfr_equal_p);
  PROVIDE_BINARY_COMPARISON_OPERATOR(!=,!mpfr_equal_p);
  
#undef BINARY_COMPARISON_OPERATOR_HELPER
#undef BINARY_COMPARISON_OPERATOR
  
  /////////////////////////////////////////////////////////////////
  
  // Providing the binary operator as nonmember function allows to
  // take into account automatically also the cases in which the first
  // operand is not a PrecFloat
  
#ifdef FAKE_HP
#define BINARY_OPERATOR_HELPER(NAME,MPFR_NAME)\
  out.data=in1.data NAME in2.data
#else
#define BINARY_OPERATOR_HELPER(NAME,MPFR_NAME)\
  MPFR_NAME(out.data,in1.data,in2.data,MPFR_RNDD)
#endif
  
#define PROVIDE_SELF_BINARY_OPERATOR(NAME)			\
  								\
  inline PrecFloat& operator NAME ## =(const PrecFloat& in)	\
    {								\
      return							\
	(*this)=(*this)NAME in;					\
    }
  
#define PROVIDE_BINARY_OPERATOR(NAME,MPFR_NAME)			\
  								\
  friend inline PrecFloat operator NAME(const PrecFloat& in1,	\
					const PrecFloat& in2)	\
  {								\
    PrecFloat out;						\
  								\
    BINARY_OPERATOR_HELPER(NAME,MPFR_NAME);			\
								\
    return out;							\
  }								\
  								\
  PROVIDE_SELF_BINARY_OPERATOR(NAME);				\
  
  PROVIDE_BINARY_OPERATOR(+,mpfr_add)
  PROVIDE_BINARY_OPERATOR(-,mpfr_sub)
  PROVIDE_BINARY_OPERATOR(*,mpfr_mul)
  PROVIDE_BINARY_OPERATOR(/,mpfr_div)
  
#undef BINARY_OPERATOR_HELPER
#undef PROVIDE_BINARY_OPERATOR
#undef PROVIDE_SELF_BINARY_OPERATOR
  
  /// Negation
  PrecFloat operator-() const
  {
    PrecFloat out;
    
#ifdef FAKE_HP
    out.data=-data;
#else
    mpfr_neg(out.data,this->data,MPFR_RNDD);
#endif
    
    return
      out;
  }
};

/////////////////////////////////////////////////////////////////

#ifdef FAKE_HP
#define UNARY_HELPER(NAME,MPFR_NAME)\
  out.data=NAME(in.data)
#else
#define UNARY_HELPER(NAME,MPFR_NAME)\
  MPFR_NAME(out.data,in.data,MPFR_RNDD)
#endif

#define PROVIDE_UNARY_FUNCTION(NAME,MPFR_NAME)	\
						\
  inline PrecFloat NAME(const PrecFloat& in)	\
{						\
  PrecFloat out;				\
						\
  UNARY_HELPER(NAME,MPFR_NAME);			\
						\
  return					\
    out;					\
}

PROVIDE_UNARY_FUNCTION(exp,mpfr_exp)
PROVIDE_UNARY_FUNCTION(abs,mpfr_abs)
PROVIDE_UNARY_FUNCTION(sqrt,mpfr_sqrt)
PROVIDE_UNARY_FUNCTION(asin,mpfr_asin)
PROVIDE_UNARY_FUNCTION(acos,mpfr_acos)
PROVIDE_UNARY_FUNCTION(sin,mpfr_sin)
PROVIDE_UNARY_FUNCTION(cos,mpfr_cos)
PROVIDE_UNARY_FUNCTION(sinh,mpfr_sinh)
PROVIDE_UNARY_FUNCTION(cosh,mpfr_cosh)
PROVIDE_UNARY_FUNCTION(erf,mpfr_erf)
PROVIDE_UNARY_FUNCTION(erfc,mpfr_erfc)

#undef PROVIDE_UNARY_FUNCTION
#undef UNARY_HELPER

/// Returns the square
inline PrecFloat sqr(const PrecFloat& in)
{
  return in*in;
}

/////////////////////////////////////////////////////////////////

/// Precise definition of Pi
inline PrecFloat precPi()
{
  PrecFloat out;
  
#ifdef FAKE_HP
  out.data=M_PI;
#else
  mpfr_const_pi(out.data,MPFR_RNDD);
#endif
  
  return
    out;
}

/////////////////////////////////////////////////////////////////

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

/////////////////////////////////////////////////////////////////

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
PrecFloat integrateUpToInfinite(F&& f,const double& xMin=0.0)
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
      // cout<<"Stability: "<<stability<<endl;
      maxAttainableStability*=2;
      // cout<<"MaxAttainableStability: "<<maxAttainableStability<<endl;
    }
  while(stability>maxAttainableStability);
  
  return sum;
}

#ifdef MAIN
int PrecFloat::_defaultPrecision;
#endif
