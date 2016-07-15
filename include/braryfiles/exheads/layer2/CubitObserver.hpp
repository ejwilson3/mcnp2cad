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
  
  virtual ~CubitObserver();

  virtual CubitStatus notify_observer(
     CubitObservable *observable, const CubitEvent &observer_event,
     CubitBoolean from_observable = CUBIT_FALSE) = 0;
    // This should be called through the
    // CubitObservable::notify_observers generic notify function.
    // NOTE: VERY IMPORTANT: when implementing this function, leaf
    // classes should call register_observable() or
    // unregister_observable() if the event is one of construction or
    // destruction, resp.; this makes sure the observable knows we're
    // observing it, or unhooks us from the observable.  A count of
    // observables being observed is also kept, to prevent stale
    // pointers on observables

  CubitStatus register_static_observer(CubitObserver *observer);
    //- add this observer to the static observer list

};

#endif

