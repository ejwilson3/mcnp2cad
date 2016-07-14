/**
 * \file GeometryModifyTool.hpp
 *
 * \brief This class provides the interface for all the geometry
 *        related operations to the outside.
 *
 * This class is implemented using the Singleton pattern.
 *
 * \author Xuechen Liu
 *
 * \date 07/11/96
 *
 */

#ifndef GEOMETRYMODIFYTOOL_HPP 
#define GEOMETRYMODIFYTOOL_HPP


class GeometryModifyEngine;

//! Interface class for modifying geometry. 
class CUBIT_GEOM_EXPORT GeometryModifyTool
{

  friend class MergeTool;
public :

  /*!
    *  \return GeometryModifyTool* - Pointer to the singleton GeometryModifyTool object
    *  \arg SMEPtr
    *   Pointer to a SolidModelingEngine object. The default value
    *   is set to NULL.
    *
    *  Return a pointer to the only instance of the class with the default
    *  solid modeling engine (SME) set to the argument, if it is not NULL.
    *  If the argument is NULL, return the pointer to the existing
    *  instance without modifying the default SME. A valid SMEPtr *must*
    *  be supplied at the time of the first call to this function.
    *  Hence, this instance function should specifically be called at
    *  startup with a valid non-NULL input SMEPtr.
    */
  static GeometryModifyTool* instance( GeometryModifyEngine* GMEPtr = NULL);

  /*! Creates an ACIS sphere and assigns it to a Body $
    *  {radius} input radius
    *  Returns the new Body or NULL
    */
  //! \brief Creates a sphere.
  Body* sphere( double radius,
                int x_shift = 0,
                int y_shift = 0,
                int z_shift = 0,
                double inner_radius = 0,
                bool delete_side = false );
  
  /*!  Creates an ACIS cuboid and assigns it to a Body $
    *  {wid, dep, hi} input width, depth, height
    *  Returns the new Body or NULL
    */
  //! \brief Creates a brick (cube).
  Body* brick( double wid, double dep, double hi );

  //! \brief Creates a clyinder. 
  Body* cylinder( double hi, double r1, double r2, double r3 );

  /*!  Creates an ACIS torus and assigns it to a Body $
    *  {r1} input major_radius $
    *  {r2} input minor_radius
    *  Returns the new Body or NULL
    */
  //! \brief Creates a torus. 
  Body* torus( double r1, double r2 );

  /*!  \return  Body*
    *  \return - A pointer to a newly created body.
    *  \arg body_ptr
    *  A pointer to a Body which is to used to make copy.
    *
    *  This function makes a copy of the input Body and returns a
    *  pointer to the newly created copy. The input Body and the newly
    *  created Body are geometrically identical. If the copying
    *  operation fails for some reason, the function returns NULL.
    */
  //! \brief Copy a Body.
  Body* copy_body ( Body* body_ptr );

  /*!  Takes a RefEntity (RefVertex, RefEdge, or RefFace) and creates a
    *  copy of it using the make_RefXxxxx commands.
    */
  //! \brief Copy a RefEntity. 
  RefEntity* copy_refentity( RefEntity *old_entity );

  //! \brief Boolean unite.
  CubitStatus unite( DLIList<Body*> &bodies,
                     DLIList<Body*> &newBodies,
                     bool keep_old = false );

  //! \brief Boolean subtract.  Subtracts the tool from all the blanks.
  CubitStatus subtract( Body* tool_body, 
                        DLIList<Body*> &from_bodies,
                        DLIList<Body*> &new_bodies,
                        bool imprint = false,
                        bool keep_old = false );

  //! \brief Boolean intersect.
  CubitStatus intersect( Body *tool_body_ptr, 
                         DLIList<Body*> &from_bodies,
                         DLIList<Body*> &new_bodies,
                         bool keep_old = false,
                         bool preview = false );
  /*!  Cuts the given bodies with the points defining a plane.
    *  This is mainly used for vis purposes to section a body with
    *  a plane and only retain half of it.
    */

  //! \brief Cuts body with plane, throwing away one side.
  CubitStatus section( DLIList<Body*> &section_body_list,
                       const CubitVector &point_1,
                       const CubitVector &point_2,
                       const CubitVector &point_3,
                       DLIList<Body*> &new_body_list,
                       CubitBoolean keep_normal_side,
                       CubitBoolean keep_old = CUBIT_FALSE );

#ifdef CGM_KCM  
  //! \brief Convert mesh entities to ACIS geometry.
  CubitStatus mesh2brep(std::vector<double> &xvals,
                        std::vector<double> &yvals,
                        std::vector<double> &zvals,
                        std::vector<unsigned int> &tri_connectivity,
                        DLIList<BodySM*> &new_body_sms);
#endif

#ifdef CAT
  CubitStatus webcut_across_translate( DLIList<Body*>& body_list,
                                       RefFace* plane_surf1,
                                       RefFace* plane_surf2,
                                       DLIList<Body*>& results_list,
                                       ImprintType imprint_type = NO_IMPRINT,
                                       CubitBoolean merge = CUBIT_FALSE,
                                       CubitBoolean preview = CUBIT_FALSE);
  /**<  Webcuts with a flat plate to make a body suitable for single-single
    *   sweeping.  Only experimental at this point.
    */
#endif

  //! \brief Imprints a group of bodies with one another.
  CubitStatus imprint( DLIList<Body*> &from_body_list,
                       DLIList<Body*> &new_body_list,
                       CubitBoolean keep_old = CUBIT_FALSE );

  /*!  Imprints a list of Bodies with a list of RefEdges.  Useful for
    *  splitting surfaces.  If edge pierces a surface a hardpoint will
    *  result at the pierce location.  Interface is free of ACIS but
    *  currently only works if all entities are ACIS entities.
    */

};

#endif

