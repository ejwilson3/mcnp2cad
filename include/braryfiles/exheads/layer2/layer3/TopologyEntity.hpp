//-------------------------------------------------------------------------
// Filename      : TopologyEntity.hpp
//
// Purpose       : This file contains the declarations of the class 
//                 TopologyEntity.
//                 This class is the base class of all the topological 
//                 entities in the Virtual Geometry Interface subsystem.
//
// Special Notes : This is a pure virtual class.
//
// Creator       : Malcolm Panthaki 
//
// Creation Date : 07/11/96
//
// Owner         : Malcolm J. Panthaki
//-------------------------------------------------------------------------

#ifndef TOPOLOGY_ENTITY_HPP
#define TOPOLOGY_ENTITY_HPP

// ********** BEGIN STANDARD INCLUDES      **********
// ********** END STANDARD INCLUDES        **********

// ********** BEGIN CUBIT INCLUDES         **********
#include "ModelEntity.hpp"
#include "BridgeManager.hpp"

// ********** END CUBIT INCLUDES           **********

class CUBIT_GEOM_EXPORT TopologyEntity: public ModelEntity
{
public :

  static const char* get_class_name()
     { return "TopologyEntity"; }
  
  CubitStatus ref_volumes(DLIList<RefVolume*>& ref_volume_list);

  Body *body();
    //R CubitStatus
    //R- CUBIT_SUCCESS/FAILURE
    //O body/ref_volume/ref_face/ref_edge/ref_vertex/shell/loop/chain/covolume/
    //O coface/coedge/covertex _list
    //O- List of Bodies/RefVolumes/RefFaces/RefEdges/RefVertex'es/Loops/
    //O- Shells/Chains/CoVolumes/CoFaces/CoEdge/CoVertices
    //- Append the appropriate list of TopologyEntities associated with
    //- this TopologyEntity.  
    //- There are 2 types of queries involved:
    //- a) Return all "owned" entities such as all the RefFaces owned by
    //-    a RefVolume.
    //- b) Return all "owning" entities such as all the Refvolumes that
    //-    own this RefFace
    //- CUBIT_FAILURE is returned if this entity has the same type as
    //- the requested list - e.g., if ref_edges is called on a RefEdge
    //- object. 
    //-
    //this

  int num_ref_faces();
  int num_ref_edges();
  int num_ref_vertices();

private:
  BridgeManager bridgeMan;
  
};
#endif

