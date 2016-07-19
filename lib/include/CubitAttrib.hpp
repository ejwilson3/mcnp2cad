//- Class:          CubitAttrib
//- Owner:          Greg Nielson
//- Description:    pure virtual base class from which all the specific
//-                 cubit attributes are derived.
//- Checked By:
//- Version:

#ifndef CUBIT_ATTRIB_HPP
#define CUBIT_ATTRIB_HPP

#include <typeinfo>
#if !defined(WIN32)
using std::type_info;
#endif

#include "CubitVector.hpp"

#include "CGMApp.hpp"

class CUBIT_GEOM_EXPORT CubitAttrib
{
public:
  CubitAttrib(RefEntity *attrib_owner);
    //- bare constructor

  virtual ~CubitAttrib();
    //- destructor

  virtual CubitBoolean has_written() const;
  virtual void has_written(CubitBoolean set_has_written);
    //- get/set hasWritten
  
  void delete_attrib(CubitBoolean set_remove) {deleteAttrib = set_remove;}
  CubitBoolean delete_attrib() {return deleteAttrib;}
    //- get/set the deleteAttrib flag
 
  virtual CubitSimpleAttrib* cubit_simple_attrib() = 0;
    //- return a cubitSimpleAttrib for this CA

  virtual CubitStatus actuate() = 0;
    //- actuate this attrib

  virtual CubitStatus actuate_list(DLIList<RefEntity*>);
    //- actuate this attrib on each of the entities passed in

  virtual CubitStatus update() = 0;
    //- update this attrib

  virtual CubitStatus reset() = 0;
    //- reset any lists or other info in this attribute
  
  virtual CubitSimpleAttrib *split_owner();
    //- split this attrib; pass back a new simple attrib if desired

  virtual void merge_owner(CubitAttrib *deletable_ca_ptr);
    //- merge this attrib with deletable_ca_ptr (keep this)

//HEADER- RTTI and safe casting functions.
  virtual const type_info& entity_type_info() const
     { return typeid(CubitAttrib);}

protected:

  virtual void remove_attribute();
  //- Removes 'this' attribute
  virtual void add_attribute();
  //- and adds a new one with the changed data.

  CubitBoolean hasActuated;
    //- flag telling whether this CA has actuated

  CubitBoolean hasUpdated;
    //- flag telling whether this CA has updated

  CubitBoolean hasWritten;
    //- flag telling whether this CA has a corresponding geometry attribute
    //- (on the geometric entity assoc'd with the owner)
  
  CubitBoolean deleteAttrib;
    //- flag telling whether this CA should be deleted
};


#endif
