//- Class: CubitBox
//-
//- Description: This file defines the CubitBox class which represents
//- an axis-aligned rectangular box which can be uses as a bounding box.
//-
//- Owner: Greg Sjaardema
//- Checked by: 
//- Version: $Id: 

#ifndef CUBITBOX_HPP
#define CUBITBOX_HPP

#include "CubitBoxStruct.h"

class CUBIT_UTIL_EXPORT CubitBox
{
public:
 
  ~CubitBox(); 
    //- destructor

  CubitVector minimum()  const;
  CubitVector maximum()  const;

  operator CubitBoxStruct() 
    {
      CubitBoxStruct to;
      to.minimum_ = minimum_;
      to.maximum_ = maximum_;
      return to;
    }

private:
  
  CubitVector minimum_; //- X, Y, and Z position of minimum corner
  CubitVector maximum_; //- X, Y, and Z position of maximum corner
};
#endif

