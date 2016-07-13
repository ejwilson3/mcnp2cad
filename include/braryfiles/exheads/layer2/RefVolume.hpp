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

// ********** BEGIN STANDARD INCLUDES      **********
// ********** END STANDARD INCLUDES        **********

// ********** BEGIN MOTIF INCLUDES         **********
// ********** END MOTIF INCLUDES           **********

// ********** BEGIN OPEN INVENTOR INCLUDES **********
// ********** END OPEN INVENTOR INCLUDES   **********

// ********** BEGIN ACIS INCLUDES          **********
// ********** END ACIS INCLUDES            **********

// ********** BEGIN CUBIT INCLUDES         **********

#include "CastTo.hpp"
#include "BasicTopologyEntity.hpp"
#include "Lump.hpp"
// lists
#include "DLIList.hpp"
// ********** END CUBIT INCLUDES           **********

// ********** BEGIN MACRO DEFINITIONS      **********
// ********** END MACRO DEFINITIONS        **********

// ********** BEGIN ENUM DECLARATIONS      **********
// ********** END ENUM DECLARATIONS        **********

// ********** BEGIN FORWARD DECLARATIONS   **********

// ********** END FORWARD DECLARATIONS     **********

//! RefVolume class.
class CUBIT_GEOM_EXPORT RefVolume : public BasicTopologyEntity
{
public :
  
  typedef RefFace ChildType;

  friend class RefEntityFactory;
    //- the factory is allowed to call the (private) constructors

    /* constructors/destructors */

  virtual ~RefVolume() ;
    //- The destructor

    /* topology */

  //! Gets the dag type.
  DagType dag_type() const { return DagType::ref_volume_type(); }  

  //! Gets the type info of this RefVolume.
  const type_info& entity_type_info() const { return typeid(RefVolume); }

  //! Gets the class name for a volume: "Volume".
  static const char* get_class_name()
  {
    return "Volume";
  }

  //! Gets the class name for a volume: "Volume".
  virtual const char* class_name() const
  {
    return get_class_name();
  }
  
  //! Gets the lump pointer.
  Lump* get_lump_ptr() ;

  //!R Lump*
  //!R- A pointer to the Lump to which the current 
  //!R- volume  points. 
  //! This function returns a pointer to the Lump
  //! to which the current volume points.
  Lump const* get_lump_ptr() const ;

  //! Gets the owning Body of this RefVolume.
  Body* get_body_ptr() ;

  //! Return the number of connected components bounding this volume.
  //! Note: this counts the number of ref_edge-connected ref_face 
  //! components, which may not be the same as the number of acis shells.
  int num_boundary_components();

  //!R CubitBoolean
  //!R-CUBIT_TRUE/CUBIT_FALSE
  //!I RefVolume*
  //!O CubitBoolean
  //!O- If the two RefVolumes are spatially equal within the GEOMETRY_RESABS*
  //! the tolerance_factor, then CUBIT_TRUE will be returned.  Otherwise
  //! CUBIT_FALSE is returned.
  //! The comparison is done by checking the bounding boxes of the
  //! RefVolumes.
  CubitBoolean about_spatially_equal ( RefVolume* ref_vol_ptr_2,
                                       double tolerance_factor);

  //! returns the genus of the volume, where
  //! g = 1 - .5(v - e + f - gs), and v,e,f = # vertices, edges, faces,
  //! and gs = summed genus of surfaces
  int genus();

    /* geometry */

  //! Returns centroid of the RefVolume
  virtual CubitVector center_point();
  
  //! Returns true if all Shells of RefVolume are sheets.
  CubitBoolean is_sheet();
  
//**********Graphics Related Functions**********//

  //! returns dimension of the actual entity. 
  virtual int dimension() const; 

    /* other functions */

  virtual CubitString measure_label();

  //@{
  //! Get the mass properties from the underlying lump
  CubitStatus mass_properties( CubitVector &centroid, double &volume );
  CubitStatus mass_properties( CubitVector principal_axes[3], 
                               CubitVector &principal_moments, 
                               CubitVector &centroid, 
                               double &volume );
  //@}

  //! Do a an api entity check.
  int validate();

protected:

  RefVolume(Lump* lumpPtr) ;
    //- The constructor with a pointer to a Lump.

private:

   void initialize();
   //- Initializes all the member data


  RefVolume( const RefVolume& );
  void operator=( const RefVolume& );
};

#endif

