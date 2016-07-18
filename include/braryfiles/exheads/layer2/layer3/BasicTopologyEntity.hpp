//-------------------------------------------------------------------------
// Filename      : BasicTopologyEntity.hpp
//
// Purpose       : This is the interface to the BasicTopologyEntity base class.
//                 The main characteristic of specific BasicTopoEntities is
//                 that they form the basis of a topological description
//                 of a solid model.  Having ordered lists of these would
//                 be a complete and unambiguous description of a solid
//                 model.
//
//                 The other characteristic of BasicTopoEntities is that 
//                 they contain ("own") various mesh-related entities. 
//
// Special Notes : Although each BasicTopologyEntity is associated with a set of 
//                 GroupingEntity's, there is no member data in a BasicTopologyEntity
//                 that stores this list.  Instead, this connection is
//                 established within the Directed Acyclic Graph (DAG)
//                 datastructure that captures the relationships between
//                 the various TopologyEntity's in the Model.  Each
//                 TopologyEntity has a member datum which is a pointer
//                 to a ModelEntity.  This
//                 pointer is a link from the TopologyEntity to the node
//                 in the Model DAG that represents its "position" and 
//                 "links" within the Model. 
//
//                 Each BasicTopologyEntity (with the exception of RefVertex) is
//                 associated with a set of GroupingEntity's (GrE's).  These 
//                 GrE's are ordered in a list.  Hence, the BasicTopologyEntity 
//                 interface not only provides the ability to get the entire 
//                 list of GrE's, but also allows you to ask for the "first"
//                 associated GrE. By extension, the GrE interface, provides 
//                 a function to ask for the "next" GrE.  The linked 
//                 list of GrE's ends when the "next" function returns a 
//                 NULL pointer. 
//
//                 Each BasicTopologyEntity HasA GeometryEntity pointer.  This 
//                 pointer is stored explicitly as a member datum because 
//                 Geometry Entity's are *not* represented by nodes in the 
//                 Model DAG.
//
//                 This is a pure virtual class, to prevent instantiation.
//
// Creator       : Malcolm J. Panthaki
//
// Creation Date : 10/14/96
//
// Owner         : Malcolm J. Panthaki
//-------------------------------------------------------------------------

#ifndef BASIC_TOPOLOGY_ENTITY_HPP
#define BASIC_TOPOLOGY_ENTITY_HPP

// ********** BEGIN CUBIT INCLUDES         **********

#include "RefEntity.hpp"
#include "CubitBox.hpp"

// ********** END CUBIT INCLUDES           **********

class CUBIT_GEOM_EXPORT BasicTopologyEntity : public TopologyEntity,
                            public RefEntity
{};
#endif


