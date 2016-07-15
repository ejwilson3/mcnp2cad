//- Class:       MergeTool
//- Description: Location of all Merge and Unmerge functionality
//- Owner:       Steven Jankovich
//- Created: 27 April 1998
//- Checked By:  
//- Version: $Id: 

#ifndef MERGETOOL_HPP
#define MERGETOOL_HPP 

#if !defined(WIN32)
using std::type_info;
#endif

class CUBIT_GEOM_EXPORT MergeTool
{

   public :
   static MergeTool* instance();
     // Returns a static pointer to unique instance of this class.
   
  //! Compare all RefFaces, RefEdges, and RefVertices in 
  //! refbody_list and merge the matches
  CubitStatus merge_bodies( DLIList<Body*>& refbody_list );
   
  //! Compare all RefFaces, RefEdges, and RefVertices in 
  //! vol_list and merge the matches
  CubitStatus merge_volumes( DLIList<RefVolume*>& vol_list,
                             CubitBoolean print_info = CUBIT_TRUE );

  //! Compare all input RefFaces and merge the matches.
  //! Uses AbstractTree rather than O(nlogn) comparisons.
  CubitStatus merge_reffaces( DLIList<RefFace*>& refface_list,
                               CubitBoolean print_info = CUBIT_TRUE);

  //! Compare all input RefEdges and merge the matches
  //! BE CAREFUL with the should_clean_out flag.  If you set
  //! it to false, then YOU (the caller) are responsible for
  //! cleaning out the deactivated geometry.
  //! Merges specified curves.  If list contains < 20 curves, use old_merge_ref_edges.
  CubitStatus merge_refedges( DLIList<RefEdge*>& refedge_list,
                               CubitBoolean should_clean_out = CUBIT_TRUE,
                               CubitBoolean print_info = CUBIT_TRUE);

  //! Compare all input RefVertices and merge the matches.
  CubitStatus merge_refvertices( DLIList<RefVertex*>& refvertex_list,
                                 CubitBoolean print_info = CUBIT_TRUE );
   
};
#endif

