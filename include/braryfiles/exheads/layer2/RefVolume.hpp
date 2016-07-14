//-------------------------------------------------------------------------
// Filename      : RefVolume.hpp 
//
// Purpose       : This file contains the declarations of the class 
//                 RefVolume.
//
// Special Notes : 
//
// Creator       : Malcolm J. Panthaki
//
// Creation Date : 07/11/96
//
// Owner         : Malcolm J. Panthaki
//-------------------------------------------------------------------------

#ifndef REFVOLUME_HPP
#define REFVOLUME_HPP

// ********** BEGIN CUBIT INCLUDES         **********

#include "BasicTopologyEntity.hpp"
// ********** END CUBIT INCLUDES           **********

//! RefVolume class.
class CUBIT_GEOM_EXPORT RefVolume : public BasicTopologyEntity
{
public :

  //! Gets the owning Body of this RefVolume.
  Body* get_body_ptr() ;
};

#endif

