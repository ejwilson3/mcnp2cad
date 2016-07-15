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
class Body;
class RefEntity;
#ifdef PROE
class RefAssembly;
class RefPart;
#endif
class TBPoint;
class Curve;
class Surface;
class Lump;
class BodySM;

class RefVertex;
class RefEdge;
class RefFace;
class RefVolume;
template <class X> class DLIList;

class CUBIT_GEOM_EXPORT RefEntityFactory : public CubitObserver
{
public:

  static RefEntityFactory *instance();
    //- the function used to access the singleton instance


  virtual RefVertex *construct_RefVertex(TBPoint *point = NULL);

  virtual RefEdge *construct_RefEdge(Curve *curve = NULL);

  virtual RefFace *construct_RefFace(Surface *surface = NULL);

  virtual RefVolume *construct_RefVolume(Lump *lump = NULL);

  virtual Body *construct_Body(BodySM *body_sm = NULL);

  virtual RefGroup *construct_RefGroup(const char* name = NULL);

  virtual RefGroup *construct_RefGroup (DLIList<RefEntity*>& entity_list);

  virtual CubitStatus ref_entity_list( char const* keyword,
                                       DLIList<RefEntity*> &entity_list,
                                       const CubitBoolean print_errors = CUBIT_TRUE);
    //- return entities of type keyword from the global list in entity_list;
    //- virtual to allow derived factories to work from their native lists

  virtual void bodies               (DLIList<Body*> &bodies);
  virtual void ref_volumes          (DLIList<RefVolume*> &ref_volumes);
  virtual void ref_groups           (DLIList<RefGroup*> &ref_groups);
  virtual void ref_faces            (DLIList<RefFace*> &ref_faces);
  virtual void ref_edges            (DLIList<RefEdge*> &ref_edges);
  virtual void ref_vertices         (DLIList<RefVertex*> &ref_vertices);
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
  int maxRefAssemblyId;
  int maxRefPartId;
#endif
private:

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

