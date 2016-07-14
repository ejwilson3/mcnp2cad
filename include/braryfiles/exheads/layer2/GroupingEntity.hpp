//-------------------------------------------------------------------------
// Filename      : GroupingEntity.hpp
//
// Purpose       : This file contains the declarations of the class 
//                 GroupingEntity.
//                 This class is the base class of all the grouping entities
//                 Body, Shell, Loop, Chain.
//
// Special Notes : Each GroupingEntity is associated with a set of 
//                 SenseEntity's (SE's). These SE's are ordered in a 
//                 list.  Hence the GroupingEntity interface
//                 not only provides the ability to get the entire list
//                 of SE's, but also allows you to ask for the "first"
//                 associated SE.  The SE interface thus provides a
//                 function to ask for the "next" SE.  The linked
//                 list of SE's ends when the next function returns a 
//                 NULL pointer. 
//
//                 The same is true of GroupingEntities themselves. They
//                 are an ordered list in the DAG, implying that a next()
//                 function needs to be provided.
//
//                 This is a pure virtual class.
//
// Creator       : Xuechen Liu 
//
// Creation Date : 07/11/96
//
// Owner         : Malcolm J. Panthaki
//-------------------------------------------------------------------------

#ifndef GROUPING_ENTITY_HPP
#define GROUPING_ENTITY_HPP

// ********** BEGIN CUBIT INCLUDES         **********
#include "TopologyEntity.hpp"
// ********** END CUBIT INCLUDES           **********

// ********** BEGIN FORWARD DECLARATIONS   **********
class SenseEntity;
class BasicTopologyEntity;

// ********** END FORWARD DECLARATIONS     **********

class CUBIT_GEOM_EXPORT GroupingEntity : public TopologyEntity
{

private :
   
  BasicTopologyEntity* myParent;
  GroupingEntity* nextInParent;
  GroupingEntity* prevInParent;
  SenseEntity* firstSenseEntity;
  SenseEntity* lastSenseEntity;
 
};

#endif

