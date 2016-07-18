//-------------------------------------------------------------------------
// Filename      : BridgeManager.hpp
//
// Purpose       : Manages the TopologyBridges being used by
//                 a single TopologyEntity.  Encapsulates the
//                 merging/unmerging of TopologyBridges into/from
//                 a TopologyEntity.
//
// Creator       : Darryl Melander
//
// Creation Date : 02/20/99
//
// Owner         : Darryl Melander
//-------------------------------------------------------------------------
#ifndef BRIDGE_MANAGER_HPP
#define BRIDGE_MANAGER_HPP

#include "TBOwner.hpp"

class CUBIT_GEOM_EXPORT BridgeManager : public TBOwner
{
public:
    // Constructor/Destructor
  BridgeManager(TopologyEntity* owner);
  virtual ~BridgeManager();

  virtual CubitStatus remove_bridge(TopologyBridge* bridge );

  virtual CubitStatus swap_bridge( TopologyBridge* old_tb, 
                                   TopologyBridge* new_tb,
                                   bool reversed );

  virtual void notify_reversed( TopologyBridge* bridge );

private:

  TopologyEntity* const topologyEntity;
  std::vector<TopologyBridge*> mergeList;
};

#endif

