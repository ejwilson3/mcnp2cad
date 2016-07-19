#ifndef __CUBITATTRIBMANAGER_HPP__
#define __CUBITATTRIBMANAGER_HPP__

class RefEntity;
class CubitAttrib;
class CubitSimpleAttrib;

#include "DLIList.hpp"
#include "CubitGeomConfigure.h"

#if defined(WIN32)
#pragma warning(push)
#pragma warning(disable : 4251)  // hide warnings about template dll exports
#endif

#include <map>

typedef CubitAttrib* (*CACreateFunction)(RefEntity*, CubitSimpleAttrib*);

class CUBIT_GEOM_EXPORT CubitAttribManager
{
public:

  // stores factory function and settings for an attribute
  // returns a key that can be used to refer to the attribute type to avoid search by name
  CubitStatus register_attrib_type( const int att_type,
                                     const char* att_type_name,
                                     const char* att_internal_name,
                                     CACreateFunction p_create_function,
                                     CubitBoolean auto_actuate_flag,
                                     CubitBoolean auto_update_flag,
                                     CubitBoolean auto_write_flag,
                                     CubitBoolean auto_read_flag,
                                     CubitBoolean actuate_in_constructor,
                                     CubitBoolean actuate_after_geom_changes);
// actuateInConstructor flag: if this flag is set to true,
// this attribute will be actuated in a constructor, otherwise
// it will be actuated after a geometry operation or geometry
// import; usually, attributes which need full topological information
// must be actuated after the geometry operation is finished
//
// actuateAfterGeomChanges flag: if this flag is set to true,
// this attribute will be actuated only after all changes to geometry
// have been done.  Implies that if an attribute changes geometry,
// or if it doesn't matter, this flag should be false (the default value); 
// if it requires the geometry to be in its final state, this flag 
// should be true

  void get_registered_types(DLIList<int> &types);

  void auto_flag(int flag);

};

#if defined(WIN32)
#pragma warning(pop)
#endif

#endif //__CUBITATTRIBMANAGER_HPP__

