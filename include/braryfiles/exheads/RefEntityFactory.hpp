//-------------------------------------------------------------------------
// Filename      : RefEntityFactory.hpp
//
// Purpose       : This file produces RefEntity and derived classes; it is
//                 the interface through which RE's should be created;
//                 applications of the geometry module can provide their
//                 own factory, otherwise this factory is used
//
// Creator       : Tim Tautges (in it's new state)
//
// Creation Date : 9/99
//-------------------------------------------------------------------------

#ifndef REFENTITYFACTORY_HPP
#define REFENTITYFACTORY_HPP

#include "CubitObserver.hpp"
#include "CubitGeomConfigure.h" 

class RefGroup;
class RefEntity;
#ifdef PROE
class RefAssembly;
class RefPart;
#endif

template <class X> class DLIList;

class CUBIT_GEOM_EXPORT RefEntityFactory : public CubitObserver
{
public:

  static RefEntityFactory *instance();
    //- the function used to access the singleton instance

  virtual RefGroup *construct_RefGroup(const char* name = NULL);

  virtual CubitStatus ref_entity_list( char const* keyword,
                                       DLIList<RefEntity*> &entity_list,
                                       const CubitBoolean print_errors = CUBIT_TRUE);
    //- return entities of type keyword from the global list in entity_list;
    //- virtual to allow derived factories to work from their native lists

#ifdef PROE
  virtual void ref_parts			(DLIList<RefPart*> &ref_parts);
  virtual void ref_assemblies		(DLIList<RefAssembly*> &ref_assemblies);
#endif

  virtual int num_bodies() const;
  virtual int num_ref_volumes() const;
  virtual int num_ref_faces() const;
  virtual int num_ref_edges() const;
  virtual int num_ref_vertices() const;

  void compress_ref_ids(const char *entity_type, int retain_max_id);
    //- take out holes in the id space; make virtual so that factory can work
    //- off of native lists

  void reset_ids();
    //- resets all the id counters to 0

  CubitStatus notify_observer(CubitObservable *observable,
                              const CubitEvent &observer_event,
                              CubitBoolean from_observable = CUBIT_FALSE);
    //- handle MODEL_ENTITY_DESTRUCTED/MODEL_ENTITY_CONSTRUCTED events

protected: 

  static RefEntityFactory *instance_;
    //- the singleton instance
  
#ifdef PROE
  DLIList<RefAssembly*> *refAssemblyList;
  DLIList<RefPart*> *refPartList;
#endif

};

inline RefEntityFactory *RefEntityFactory::instance()
{
  if (instance_ == NULL) {
    instance_ = new RefEntityFactory();
    instance_->register_static_observer(instance_); 
  }
  
  return instance_;
}

#endif

