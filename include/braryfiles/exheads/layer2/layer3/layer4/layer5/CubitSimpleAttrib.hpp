//- Class: CubitSimpleAttrib
//- Owner: Greg Nielson
//- Description: This class is used to carry information from
//-              the cubit attribute classes to the solid model
//-               attributes.
//- Checked By:
//- Version: $Id:

#ifndef CUBIT_SIMPLE_ATTRIB_HPP
#define CUBIT_SIMPLE_ATTRIB_HPP

#if defined(WIN32)
#pragma warning(push)
#pragma warning(disable : 4251)  // hide warnings about template dll exports
#endif

#include "CubitString.hpp"

class CUBIT_GEOM_EXPORT CubitSimpleAttrib {
private:
  CubitString characterType;  // defines the attribute type 

  DLIList<CubitString*> stringDataList;  // List of CubitString attribute data

  DLIList<double*> doubleDataList;  // List of double attribute data

  DLIList<int*> intDataList;  // List of int attribute data

public:

  CubitSimpleAttrib(DLIList<CubitString*> *string_list,
                    DLIList<double> *double_list = NULL,
                    DLIList<int> *int_list = NULL);

  CubitString character_type(); // {return characterType;}
  //- returns the type of attribure

 // static void initialize_settings();

  DLIList<CubitString*>* string_data_list() {return &stringDataList;} 

  DLIList<int*>* int_data_list() {return &intDataList;}
  
};


#if defined(WIN32)
#pragma warning(pop)
#endif

#endif








