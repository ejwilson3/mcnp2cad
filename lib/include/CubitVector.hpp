//- Class: CubitVector
//-
//- Description: This file defines the CubitVector class which is a
//- standard three-dimensional vector. All relevant arithmetic
//- operators are overloaded so CubitVectors can be used similar to
//- built-in types.
//-
//- Owner: Greg Sjaardema
//- Checked by: Randy Lober, January 94
//- Version: $Id: 

#ifndef CUBITVECTOR_HPP
#define CUBITVECTOR_HPP

#include "CubitDefines.h"
#include "CubitVectorStruct.h"

class CUBIT_UTIL_EXPORT CubitVector
{
public:
  
    //- Heading: Constructors and Destructor
  CubitVector();  //- Default constructor.
  
  CubitVector(const double x, const double y, const double z);
    //- Constructor: create vector from three components

  void get_xyz( double &x, double &y, double &z ) const; //- Get x, y, z components

  double normalize();
    //- Normalize (set magnitude equal to 1) vector - return the magnitude

  double length() const;
    //- Calculate the length of the vector.
    //- Use {length_squared()} if only comparing lengths, not adding.

  double length_squared() const;
    //- Calculate the squared length of the vector.
    //- Faster than {length()} since it eliminates the square root if
    //- only comparing other lengths.

    //- Heading: Operator Overloads  *****************************
  CubitVector&  operator+=(const CubitVector &vec);
    //- Compound Assignment: addition: {this = this + vec}

  CubitVector& operator*=(const CubitVector &vec);
    //- Compound Assignment: cross product: {this = this * vec},
    //- non-commutative
  
  CubitVector& operator*=(const double scalar);
    //- Compound Assignment: multiplication: {this = this * scalar}

  friend CubitVector operator*(const CubitVector &v1, 
                               const CubitVector &v2);
    //- vector cross product, non-commutative

  CubitVector &operator=(const CubitVectorStruct &from);

  operator CubitVectorStruct() 
    {
      CubitVectorStruct to = {xVal, yVal, zVal};
      return to;
    }

private:
  
  double xVal;  //- x component of vector.
  double yVal;  //- y component of vector.
  double zVal;  //- z component of vector.
};

inline void CubitVector::get_xyz(double &xOut, double &yOut, double &zOut) const
{
  xOut = xVal; 
  yOut = yVal; 
  zOut = zVal;
}
inline CubitVector& CubitVector::operator+=(const CubitVector &v)
{
  xVal += v.xVal;
  yVal += v.yVal;
  zVal += v.zVal;
  return *this;
}

inline CubitVector& CubitVector::operator*=(const CubitVector &v)
{
  double xcross, ycross, zcross;
  xcross = yVal * v.zVal - zVal * v.yVal;
  ycross = zVal * v.xVal - xVal * v.zVal;
  zcross = xVal * v.yVal - yVal * v.xVal;
  xVal = xcross;
  yVal = ycross;
  zVal = zcross;
  return *this;
}

inline CubitVector::CubitVector()
    : xVal(0), yVal(0), zVal(0)
{}

inline CubitVector::CubitVector(const double xIn,
                                const double yIn,
                                const double zIn)
    : xVal(xIn), yVal(yIn), zVal(zIn)
{}

// Scale all values by scalar.
inline CubitVector& CubitVector::operator*=(const double scalar)
{
  xVal *= scalar;
  yVal *= scalar;
  zVal *= scalar;
  return *this;
}

// Cross products.
// vector1 cross vector2
inline CubitVector operator*(const CubitVector &vector1,
                      const CubitVector &vector2)
{
  return CubitVector(vector1) *= vector2;
}

inline double CubitVector::length_squared() const
{
  return( xVal*xVal + yVal*yVal + zVal*zVal );
}

inline double CubitVector::length() const
{
  return( sqrt(xVal*xVal + yVal*yVal + zVal*zVal) );
}

inline double CubitVector::normalize()
{
  double mag = length();
  if (mag != 0)
  {
    xVal = xVal / mag;
    yVal = yVal / mag;
    zVal = zVal / mag;
  }
  return mag;
}

#endif

