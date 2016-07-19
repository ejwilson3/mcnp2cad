//-------------------------------------------------------------------------
// Copyright Notice
//
// Copyright (c) 1996 
// by Malcolm J. Panthaki, DBA, and the University of New Mexico.
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
//
// Filename      : RefFace.hpp
//
// Purpose       : A RefFace is a BasicTopologyEntity.  It is a topological
//                 entity that represents a bounded surface.  A RefFace is a
//                 contiguous point set, but can have multiple "holes"
//                 within it. It is a topologically 2-D entity.
//
//                 A RefFace can be meshed.                
//
// Special Notes : 
//
// Creator       : Malcolm J. Panthaki
//
// Creation Date : 07/11/96
//
// Owner         : Malcolm J. Panthaki
//-------------------------------------------------------------------------

#ifndef REFFACE_HPP
#define REFFACE_HPP

class CUBIT_GEOM_EXPORT RefFace : public BasicTopologyEntity{};
#endif

