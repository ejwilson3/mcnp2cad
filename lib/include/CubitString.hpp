//- Class: CubitString
//- 
//- Description: This file defines the CubitString class which is a 
//- simple implementation of a character string. Basic functionality 
//- is provided as well as equality/inequality tests, subscripting into 
//- a string, searching a string for another string, and stream I/O.
//- This class was written by someone else, but I forget who and cannot
//- find where I got it from. I am still looking.
//-
//- Owner: Greg Sjaardema
//- Author: <unknown at this time, I am looking>
//- Checked by: Jim Hipp, August 31, 1994
//- Version: $Id: 

#if !defined(STRING_HPP)
#define STRING_HPP

#include <iostream>

class CubitStringRep;

class CUBIT_UTIL_EXPORT CubitString
{
public:

  CubitString(const CubitString& s);
  //- Copy Constructor

  CubitString(const char *s);
  //- Create a string from a char*

  bool operator!=(const CubitString &s) const;

  const char *c_str() const;
  //- Miscellaneous
  
private:
  CubitStringRep *rep;
};

#endif

