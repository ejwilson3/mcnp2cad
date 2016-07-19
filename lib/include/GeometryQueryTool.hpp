/**
 * \file GeometryQueryTool.hpp
 *
 * \brief This class provides the interface for all the query-only geometry
 *        related operations to the outside.
 *
 * This class is implemented using the Singleton pattern.
 *
 * \author Tim Tautges
 *
 * \date 2/2001
 *
 */


#ifndef GEOMETRYQUERYTOOL_HPP
#define GEOMETRYQUERYTOOL_HPP

#if !defined(WIN32)
using std::type_info;
#endif

#include "GeometryQueryEngine.hpp"
#include "CGMHistory.hpp"

class BasicTopologyEntity;

class CUBIT_GEOM_EXPORT GeometryQueryTool
{
public :

  static GeometryQueryTool* instance( GeometryQueryEngine* gqePtr = NULL);

#ifdef PROE
  void ref_parts (DLIList<RefPart*> &ref_parts);
  void ref_assemblies (DLIList<RefAssembly*> &ref_assemblies);
#endif //PROE

  //! \brief Number of bodies in current session.
  int num_bodies() const;

  //! \brief Number of surfaces in current session.
  int num_ref_faces() const;

  //! \brief Number of curves in current session.
  int num_ref_edges() const;

  //! \brief Number of vertices in current session.
  int num_ref_vertices() const;

  //! \brief Set the major/minor version of the active geometry engine.
  CubitStatus set_export_allint_version(int version);

  //! \brief Deletes a body.
  CubitStatus delete_Body( Body* body_ptr );


  //! \brief Deletes free RefEnties
  CubitStatus delete_RefEntity( RefEntity* ref_entity_ptr );
 
  //! \brief Sets geometry factor. 
  static void set_geometry_factor( double fac );

  //! \brief Gets geometry factor. 
  static double get_geometry_factor();
 
  //! \brief Translate a Body some delta.
  CubitStatus translate( Body* entity, const CubitVector& delta, bool check_to_transform = true,
                         bool preview = false );

  //! \brief Translate a BasicTopologyEntity some delta.
  CubitStatus translate( BasicTopologyEntity* entity, const CubitVector& delta, bool check_to_transform = true,
                         bool preview = false );

  //! \brief Rotate a Body an angle about an axis. 
  CubitStatus rotate( Body* entity, const CubitVector& axis, double degrees, bool check_to_transform = true,
                      bool preview = false );

  //! \brief Rotate a BacisTopologyEntity an angle about an axis.
  CubitStatus rotate( BasicTopologyEntity* entity, 
                      const CubitVector& axis, 
                      double degrees,
                      bool check_to_transform = true,
                      bool preview = false);

  //! \brief Scale a Body different factors in x, y, and z.
  CubitStatus scale( Body* entity,const CubitVector& point, const CubitVector& factors, bool check_to_transform = true, bool preview = false);

  //! \brief Scale a BasicTopologyEntity different factors in x, y, and z.
  CubitStatus scale( BasicTopologyEntity* entity,const CubitVector& point, const CubitVector& factors,bool check_to_transform = true,
                     bool preview = false);

  //! \brief Reflect a list of bodies about a plane defined by an axis.
  CubitStatus reflect( DLIList<Body*> bodies,const CubitVector& point, const CubitVector& axis, bool preview = false );
  
  //! \brief Reflect a BasicTopologyEntity about a plane defined by an axis.
  CubitStatus reflect( BasicTopologyEntity* entity, 
                       const CubitVector& point,
                       const CubitVector& axis,
                       bool check_to_transform = true,
                       bool preview = false);

};

#endif

