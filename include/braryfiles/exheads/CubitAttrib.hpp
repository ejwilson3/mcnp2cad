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

class CubitAttrib;//9

class CUBIT_GEOM_EXPORT CubitAttrib//9
{
public:
  CubitAttrib(RefEntity *attrib_owner);//9
    //- bare constructor

  virtual ~CubitAttrib();//9
    //- destructor

  virtual CubitBoolean has_written() const;//9
  virtual void has_written(CubitBoolean set_has_written);//9
    //- get/set hasWritten
  
  void delete_attrib(CubitBoolean set_remove) {deleteAttrib = set_remove;}//9
  CubitBoolean delete_attrib() {return deleteAttrib;}//9
    //- get/set the deleteAttrib flag
 
  virtual CubitSimpleAttrib* cubit_simple_attrib() = 0;//9
    //- return a cubitSimpleAttrib for this CA

  virtual CubitStatus actuate() = 0;//9
    //- actuate this attrib
  
  virtual CubitStatus actuate_list(DLIList<RefEntity*>);//9
    //- actuate this attrib on each of the entities passed in

  virtual CubitStatus update() = 0;//9
    //- update this attrib

  virtual CubitStatus reset() = 0;//9
    //- reset any lists or other info in this attribute
  
  virtual CubitSimpleAttrib *split_owner();//9
    //- split this attrib; pass back a new simple attrib if desired

  virtual void merge_owner(CubitAttrib *deletable_ca_ptr);//9
    //- merge this attrib with deletable_ca_ptr (keep this)

  virtual void transf_owner(const CubitVector &matrow1,//9
                            const CubitVector &matrow2,
                            const CubitVector &matrow3,
                            const CubitVector &translate_vec,
                            const double scale_factor);
    //- transform this attrib with the data passed in

    virtual int int_attrib_type() = 0;//9
    //- return the enumerated type of attribute

//HEADER- RTTI and safe casting functions.
  virtual const type_info& entity_type_info() const//9
     { return typeid(CubitAttrib);}

  virtual void print();//9
    //- print some details about this attrib
    
protected://9

  virtual void remove_attribute();//9
  //- Removes 'this' attribute
  virtual void add_attribute();//9
  //- and adds a new one with the changed data.

  CubitBoolean hasActuated;
    //- flag telling whether this CA has actuated

  CubitBoolean hasUpdated;
    //- flag telling whether this CA has updated

  CubitBoolean hasWritten;
    //- flag telling whether this CA has a corresponding geometry attribute
    //- (on the geometric entity assoc'd with the owner)
  
  CubitBoolean deleteAttrib;//9
    //- flag telling whether this CA should be deleted
};//9


#endif
