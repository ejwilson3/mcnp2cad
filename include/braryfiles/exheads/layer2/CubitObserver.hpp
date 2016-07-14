//- Class: CubitObserver
//- Description: Observer class - *very* rudimentary implementation
//- Owner:  Tim Tautges

#ifndef CUBITOBSERVER_HPP
#define CUBITOBSERVER_HPP

#include "CubitEvent.hpp"
#include "DLIList.hpp"

class CubitObservable;

class CUBIT_UTIL_EXPORT CubitObserver
{
public:

  CubitStatus register_static_observer(CubitObserver *observer);
    //- add this observer to the static observer list

};

#endif

