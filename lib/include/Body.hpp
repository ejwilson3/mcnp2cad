/*-------------------------------------------------------------------------
 * Filename      : Body.hpp
 *
 * Purpose       : This class represents the topological entity, Body,
 *                 which is the highest level abstraction for a complete
 *                 geometric model.
 *
 * Special Notes : Body is a GroupingEntity in the TopologyEntity hierarchy.
 *                 It is also a RefEntity and get a lot of its functionality
 *                 and data from the RefEntity base class for meshing
 *                 purposes.
 *
 *                 The user is provided a handle to Bodies.
 *
 * Creator       : 
 *
 * Creation Date : 
 *
 * Owner         : 
 *-------------------------------------------------------------------------
 */

#ifndef BODY_HPP
#define BODY_HPP

#include "GroupingEntity.hpp"
#include "RefVolume.hpp"

/*
class CubitTransformMatrix;

//! Body class.
*/class CUBIT_GEOM_EXPORT Body : public GroupingEntity,
             public RefEntity
{
public :

  /*- the factory is allowed to call the (private) constructors */
  friend class RefEntityFactory;

} ;
#endif

