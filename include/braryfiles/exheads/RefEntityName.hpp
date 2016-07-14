//- Class: RefEntityName
//-
//- Description: This file maintains a map between a name and a
//- refentity. Names must be unique, but a refentity may have 
//- multiple names. This is a singleton class.
//-
//- Owner: Greg Sjaardema
//- Checked by: 
//- Version: $Id: 

#ifndef REFENTITYNAME_HPP
#define REFENTITYNAME_HPP

class CUBIT_GEOM_EXPORT RefEntityName
{
public:

  static RefEntityName *instance();
  
  RefEntity* get_refentity(const CubitString &name);
  //- Returns a pointer to the RefEntity with this name.

};
#endif


