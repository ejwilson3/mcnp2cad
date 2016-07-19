//- Class:       CubitAttribUser
//- Owner:       Greg Nielson
//- Description: Base class inherited by the RefEntity class that
//-              gives attribute functionality to the RefEntitys.
//- Checked By:
//- Version: 

#ifndef CUBIT_ATTRIB_USER_HPP
#define CUBIT_ATTRIB_USER_HPP

#if !defined(WIN32)
using std::type_info;
#endif

#include "CubitAttrib.hpp"
#include "CubitSimpleAttrib.hpp"

class CUBIT_GEOM_EXPORT CubitAttribUser
{

public:
  CubitAttrib* get_cubit_attrib (int attrib_type,
                                 CubitBoolean create_if_missing = CUBIT_TRUE);

};

#endif
                     
