//-------------------------------------------------------------------------
// Copyright Notice
//
// Copyright (c) 1996 
// by Malcolm J. Panthaki, DBA, and the University of New Mexico.
//-------------------------------------------------------------------------

//-------------------------------------------------------------------------
// Filename      : ModelEntity.hpp
//
// Purpose       : This class serves as the pure abstract class of
//                 all the Model-related classes. Each ModelEntity Object
//		   can be considered a Node in the DAG.
//
// Special Notes : This is a pure virtual class to prevent instantiation.
//
// Creator       : Raikanta Sahu
//
// Creation Date : 10/15/96
//
// Owner         : Malcolm J. Panthaki
//-------------------------------------------------------------------------

#ifndef MODEL_ENTITY_HPP
#define MODEL_ENTITY_HPP

class CUBIT_GEOM_EXPORT ModelEntity
{

 public:
   virtual ~ModelEntity();

protected:

   cBit deactivatedStatus_ : 1;
     //- The deactivated flag
} ;

#endif

