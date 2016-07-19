//-------------------------------------------------------------------------
// Filename      : TopologyBridge.hpp
//
// Purpose       : This file contains the declarations of the base class 
//                 TopologyBridge.  Sub-classes of TopologyBridge
//                 represent the link between the Cubit representation
//                 of an entity (such as RefVertex) and the GME representation
//                 of an entity (such as an acis VERTEX).
//
// Creator       : Darryl Melander
//
// Creation Date : 01/10/99
//
// Owner         : Darryl Melander
//-------------------------------------------------------------------------

#ifndef MODEL_ENTITY_BRIDGE_HPP
#define MODEL_ENTITY_BRIDGE_HPP


#if !defined(WIN32)
using std::type_info;
#endif

// ********** BEGIN FORWARD DECLARATIONS   **********
class TopologyEntity;
// ********** END FORWARD DECLARATIONS     **********

class CUBIT_GEOM_EXPORT TopologyBridge 
{ public: virtual ~TopologyBridge(); };

#endif

