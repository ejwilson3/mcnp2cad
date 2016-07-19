//-------------------------------------------------------------------------
// Filename      : RefEntity.hpp
//
// Purpose       : This file consolidates functionality from several
//                 base classes in CUBIT, and serves as the entry point
//                 to geometry from the meshing and other CUBIT classes
//
// Creator       : Tim Tautges (in it's new state)
//
// Creation Date : 9/99
//-------------------------------------------------------------------------

#ifndef REFENTITY_HPP
#define REFENTITY_HPP

// ********** BEGIN CUBIT INCLUDES         **********
#include "CubitObservable.hpp"
#include "CubitAttribUser.hpp"
#include "ToolDataUser.hpp"
#include "DagType.hpp"

// ********** END CUBIT INCLUDES           **********

// ********** BEGIN ENUM DEFINITIONS     **********

enum AutoMergeStatus {
  AUTO_MERGE_AUTO = 2,
  AUTO_MERGE_ON   = 1,
  AUTO_MERGE_OFF  = 0
};


// ********** END ENUM DEFINITIONS       **********
//! Base class for all geometry entities, body, volume, surface... 
class CUBIT_GEOM_EXPORT RefEntity: public CubitEntity,
                 public CubitObservable,
                 public ToolDataUser,
                 public CubitAttribUser
{
public:
  
  virtual ~RefEntity ();
    //- A pure virtual destructor - this ensures that the class is
    //- non-instantiable.

  //! \brief Sets the name of this RefEntity.
  CubitStatus entity_name (CubitString name);

  //@{
  //- Generic flag for temporary use by algorithms.
  //- Value is volatile and may change unexpectedly.
  virtual void marked(int value);
  virtual int  marked ();
  //@}
  
  //! \brief Setting auto merge status flag.
  virtual void is_mergeable(AutoMergeStatus val);

  //! \brief Get whether all child entities are mergeable.
  virtual bool children_mergeable();
  
  //! Allow unmerging and other operations.  Default in 
  //! RefEntity always returns true.  Provided for
  //! derived classes to override.
  virtual int can_modify();
  
  //! Returns the geometric dimension of the entity. 
  //! vertex == 0, edge == 1, etc.
  virtual int dimension() const; 
  
  //! Appends all immediate (child) RefEntities owned by this RefEntity to 
  //! entity_list. (The query goes down just one dimension.)
  virtual void get_child_ref_entities(DLIList<RefEntity*>& entity_list); 
  
  //! Appends all RefEntities that own this (parent RefEntities) to 
  //! entity_list.
  //! (The query goes up just one dimension. For example, if this is a
  //! vertex, the resulting list contains only RefEdges).
  virtual void get_parent_ref_entities(DLIList<RefEntity*>& entity_list);
  
  //@{
  //- Return TRUE if this is entity, or a direct child (parent) of entity.
  virtual CubitBoolean is_child(RefEntity *entity);
  virtual CubitBoolean is_parent(RefEntity *entity);
  //@}

 
  //! A generic geometric extent function.
  //! Returns volume for RefVolumes, area for RefFaces, length for RefEdge,
  //! and 1.0 for RefVertices
  //! A RefGroup calculates the maximum dimension of its contained
  //! entities and returns the sum of the measures() of all entities
  //! of that dimension.
  //! Default return value is 0.0 for all other entities.
  virtual double measure();
  
  //! Returns the type of measure: (volume, area, length, or N/A)
  virtual CubitString measure_label();
  
  //! Returns the dag type of this enity.
  virtual DagType dag_type() const = 0;

};
#endif

