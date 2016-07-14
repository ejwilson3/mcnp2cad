//- Class:       RefGroup
//- Description: The RefGroup class contains the reference entity structure
//-              for bodies. It contains the standard information such as
//-              meshed status, etc.
//- Owner:       Ted Blacker
//- Checked by:  Greg Sjaardema, 2/28/94
//- Version: $Id: 

#ifndef REFGROUP_HPP
#define REFGROUP_HPP

#include "RefCollection.hpp"

class CUBIT_GEOM_EXPORT RefGroup : public RefCollection,
                 public CubitObserver
{ 	

public:

  virtual CubitStatus add_ref_entity(RefEntity *ref_entity, bool emit_event = false);

  virtual void get_child_entities(DLIList<CubitEntity*>& cub_entity_list);
    //- appends all immediate ref entities owned by this entity on entity_list
    //- Goes down just one dimension.

};

#endif // REFGROUP_HPP

