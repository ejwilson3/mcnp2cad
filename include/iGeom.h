#ifndef _ITAPS_iGeom
#define _ITAPS_iGeom

  /** \mainpage The ITAPS Geometry Interface iGeom
   *
   * The ITAPS Geometry Interface iGeom provides a common interface for
   * accessing geometry and data associated with a mesh.  Applications written
   * to use this interface can use a variety of implementations, choosing
   * the one that best meets its needs.  They can also use tools written
   * to this interface.
   *
   * \section ITAPS Data Model
   *
   * The ITAPS interfaces use a data model composed of four basic data types:\n
   * \em Entity: basic topological entities in a model, e.g. vertices, 
   * edges, faces, regions. \n
   * \em Entity \em Set: arbitrary grouping of other entities and sets. 
   * Entity sets also support parent/child relations with other sets which
   * are distinct from entities contained in those sets.  Parent/child links
   * can be used to embed graph relationships between sets, e.g. to 
   * represent topological relationships between the sets. \n
   * \em Interface: the object with which model is associated and on which
   * functions in iGeom are called. \n
   * \em Tag: application data associated with objects of any of the other 
   * data types.  Each tag has a designated name, size, and data type.
   *
   * \section JTAPS Entity Type
   * Each entity has a specific Entity Type.  The Entity 
   * Type is one of VERTEX, EDGE, FACE, and REGION, and is synonymous with
   * the topological dimension of the entity.  Entity Type is an enumerated
   * type in the iBase_EntityType enumeration.
   *
   * \section KTAPS Entity-, Array-, and Iterator-Based Access
   *
   * The iGeom interface provides functions for accessing entities
   * individually, as arrays of entities, or using iterators.  These access
   * methods have different memory versus execution time tradeoffs, 
   * depending on the implementation.
   *
   * \section LTAPS Lists Passed Through Interface
   *
   * Many of the functions in iGeom have arguments corresponding to lists of 
   * objects.  In-type arguments for lists consist of a pointer to an array and
   * a list size.  Lists returned from functions are passed in three arguments,
   * a pointer to the array representing the list, and pointers to the
   * allocated and occupied lengths of the array.  These three arguments are 
   * inout-type arguments, because they can be allocated by the application and
   * passed into the interface to hold the results of the function.  Lists
   * which are pre-allocated must be large enough to hold the results of the
   * function; if this is not the case, an error is generated.  Otherwise, the
   * occupied size is changed to the size output from the function.  If a list
   * argument is unallocated (the list pointer points to a NULL value) or if
   * the incoming value of the allocated size is zero, the list storage will be
   * allocated by the implementation.  IN ALL CASES, MEMORY ALLOCATED BY ITAPS
   * INTERFACE IMPLEMENTATIONS IS DONE USING THE C MALLOC FUNCTION, AND CAN BE
   * DE-ALLOCATED USING THE C FREE FUNCTION.
   *
   */

#include "iBase.h"
#include "iGeom_protos.h"
#ifdef __cplusplus
extern "C" {
#endif

    /**\brief  Type used to store iGeom interface handle
     *
     * Type used to store iGeom interface handle
     */
  typedef struct iGeom_Instance_Private* iGeom_Instance;

    /**\brief  Get a description of the error returned from the last iGeom call
     *
     * Get a description of the error returned from the last iGeom function
     * \param instance iGeom instance handle
     * \param descr Pointer to a character string to be filled with a
     *        description of the error from the last iGeom function
     * \param descr_len Length of the character string pointed to by descr
     */
  void iGeom_getDescription( char* descr,
                             int descr_len );

    /**\brief  Construct a new iGeom instance
     *
     * Construct a new iGeom instance, using implementation-specific options
     * \param options Pointer to implementation-specific options string
     * \param instance Pointer to iGeom instance handle returned from function
     * \param *err Pointer to error type returned from function
     * \param options_len Length of the character string pointed to by options
     */
  void iGeom_newGeom( char const* options,
                      iGeom_Instance* instance_out,
                      int* err,
                      int options_len );

    /**\brief  Save a geom to a file
     *
     * Save a geom to a file.  If entity set is specified, save only the
     * geom contained in that set.
     * \param instance iGeom instance handle
     * \param entity_set_handle Entity set being saved
     * \param name File name to which geom is to be saved
     * \param options Pointer to implementation-specific options string
     * \param *err Pointer to error type returned from function
     * \param name_len Length of the file name character string
     * \param options_len Length of the options character string
     */
  void iGeom_save( char const* name,
                   char const* options,
                   int* err,
                   int name_len,
                   int options_len );

    /**\brief  Get handle of the root set for this instance
     *
     * Get handle of the root set for this instance.  All geom in
     * this instance can be accessed from this set.
     * \param instance iGeom instance handle
     * \param root_set Pointer to set handle returned from function
     * \param *err Pointer to error type returned from function
     */
  void iGeom_getRootSet( iGeom_Instance instance,
                         iBase_EntitySetHandle* root_set,
                         int* err );

    /**\brief  Get entities of specific type in set or instance
     *
     * Get entities of specific type in set or instance.  All entities are
     * requested by specifying iBase_ALL_TYPES.  Specified type must be a value
     * in the iBase_EntityType enumeration.
     * \param instance iGeom instance handle
     * \param entity_set_handle Entity set being queried
     * \param entity_type Type of entities being requested
     * \param entity_topology Topology of entities being requested
     * \param *entity_handles Pointer to array of entity handles returned 
     *        from function
     * \param *entity_handles_allocated Pointer to allocated size of 
     *        entity_handles array
     * \param *entity_handles_size Pointer to occupied size of entity_handles
     *        array
     * \param *err Pointer to error type returned from function
     */
  void iGeom_getEntities( iBase_EntitySetHandle set_handle,
                          int entity_type,
                          iBase_EntityHandle** entity_handles,
                          int* entity_handles_allococated,
                          int* entity_handles_size,
                          int* err );

    /**\brief  Get the number of entities with the specified type in the
     *         instance or set
     *
     * Get the number of entities with the specified type in the instance 
     * or set.  If entity set handle is zero, return information for instance,
     * otherwise for set.  Value of entity type must be from the
     * iBase_EntityType enumeration.  If iBase_ALL_TYPES is specified, total
     * number of entities (excluding entity sets) is returned.
     * \param instance iGeom instance handle
     * \param entity_set_handle Entity set being queried
     * \param entity_type Type of entity requested
     * \param num_type Pointer to number of entities, returned from function
     * \param *err Pointer to error type returned from function
     */
  void iGeom_getNumOfType( iBase_EntitySetHandle set_handle,
                           int entity_type,
                           int* num_out,
                           int* err );

    /**\brief  Get the bounding box of the specified entity
     *
     * Get the bounding box of the specified entity
     * \param instance iGeom instance handle
     * \param entity_handle Entity being queried
     * \param min_x Minimum coordinate of bounding box 
     * \param min_y Minimum coordinate of bounding box 
     * \param min_z Minimum coordinate of bounding box 
     * \param max_x Maximum coordinate of bounding box 
     * \param max_y Maximum coordinate of bounding box 
     * \param max_z Maximum coordinate of bounding box 
     * \param *err Pointer to error type returned from function
     */
  void iGeom_getEntBoundBox( iBase_EntityHandle entity_handle,
                             double* min_x,
                             double* min_y,
                             double* min_z,
                             double* max_x,
                             double* max_y,
                             double* max_z,
                             int* err );

    /**\brief  Make a copy of the specified entity
     *
     * Make a copy of the specified entity
     * \param instance iGeom instance handle
     * \param source entity to be copied
     * \param copy the newly-created entity
     * \param *err Pointer to error type returned from function
     */
  void iGeom_copyEnt( iBase_EntityHandle source,
                      iBase_EntityHandle* copy,
                      int* err );

    /**\brief  Delete specified entity
     *
     * Delete specified entity
     * \param instance iGeom instance handle
     * \param entity_handle Entity to be deleted
     * \param *err Pointer to error type returned from function
     */
  void iGeom_deleteEnt( iBase_EntityHandle entity_handle,
                        int* err );

    /**\brief Create a sphere
     *
     * Create a sphere of the specified radius centered on the origin.
     * \param instance iGeom instance handle
     * \param radius radius of the sphere
     * \param geom_entity Pointer to new entity handle returned from function
     * \param *err Pointer to error type returned from function
     */
  void iGeom_createSphere( double radius,
                           iBase_EntityHandle* geom_entity,
                           int* err );

    /**\brief  Create an axis-oriented box 
     *
     * Create an axis-oriented box of the given dimensions, centered at the
     * origin.
     * \param instance iGeom instance handle
     * \param x x dimension of new box
     * \param y y dimension of new box
     * \param z z dimension of new box
     * \param geom_entity Pointer to new entity handle returned from function
     * \param *err Pointer to error type returned from function
     */
  void iGeom_createBrick( double x,
                          double y,
                          double z,
                          iBase_EntityHandle* geom_entity,
                          int* err );

    /**\brief  Create a cylinder
     *
     * Create a cylinder parallel to the z-axis and centered at the origin (so
     * that its z-coordinate extents are +height/2 and -height/2).
     * \param instance iGeom instance handle
     * \param height The height of the cylinder.
     * \param major_rad The x-axis radius
     * \param minor_rad The y-axis radius. If minor_rad is 0, the cylinder will 
     *        be circular (as if minor_rad == major_rad).
     * \param geom_entity Pointer to new entity handle returned from function
     * \param *err Pointer to error type returned from function
     */
  void iGeom_createCylinder( double height,
                             double major_rad,
                             double minor_rad,
                             iBase_EntityHandle* geom_entity,
                             int* err );

    /**\brief  Create a cone or tapered cylinder
     *
     * Create a cone parallel to the z-axis and centered at the origin (so that
     * its z-coordinate extents are +height/2 and -height/2). The 'base' of the
     * cylinder is at z = -height/2, and the top is at +height/2.
     * \param instance iGeom instance handle
     * \param height The height of the cone.
     * \param major_rad_base The x-axis radius at the base of the cylinder 
     * \param minor_rad_base The y-axis radius at the base.  If minor_rad_base
     *        is 0, the cylinder will be circular (as if minor_rad_base ==
     *        major_rad_base)
     * \param rad_top The x-axis radius at the top of the cone.  The y-axis
     *        radius at the top of the cone will be inferred to keep the aspect 
     *        ratio of the top of the cone the same as the bottom. If rad_top is
     *        0, the cone terminates at a point.
     * \param geom_entity Pointer to new entity handle returned from function
     * \param *err Pointer to error type returned from function
     */
  void iGeom_createCone( double height,
                         double major_rad_base,
                         double minor_rad_base,
                         double rad_top,
                         iBase_EntityHandle* geom_entity,
                         int* err );

    /**\brief  Create a torus
     *
     * Create a torus centered on the origin and encircling the z-axis.
     * \param instance iGeom instance handle
     * \param major_rad The distance from the origin to the center of the
     *        torus's circular cross-section.
     * \param minor_rad The radius of the cross-section.
     * \param geom_entity Pointer to new entity handle returned from function
     * \param *err Pointer to error type returned from function
     */
  void iGeom_createTorus( double major_rad,
                          double minor_rad,
                          iBase_EntityHandle* geom_entity,
                          int* err );

    /**\brief  Move an entity by the given vector
     *
     * Move an entity by translating it along the given vector.
     * \param instance iGeom instance handle
     * \param geom_entity the entity to move
     * \param x x coordinate of the vector
     * \param y y coordinate of the vector
     * \param z z coordinate of the vector
     * \param *err Pointer to error type returned from function
     */
  void iGeom_moveEnt( iBase_EntityHandle geom_entity,
                      double x,
                      double y,
                      double z,
                      int* err );

    /**\brief  Rotate an entity about an axis
     *
     * Rotate an entity by the given angle about the given axis.
     * \param instance iGeom instance handle
     * \param geom_entity the entity to rotate
     * \param angle the rotational angle, in degrees
     * \param axis_x x coordinate of the axis
     * \param axis_y y coordinate of the axis
     * \param axis_z z coordinate of the axis
     * \param *err Pointer to error type returned from function
     */
  void iGeom_rotateEnt( iBase_EntityHandle geom_entity,
                        double angle,
                        double axis_x,
                        double axis_y,
                        double axis_z,
                        int* err );

   /**\brief  Reflect an entity across a plane
     *
     * Reflect an entity across the given plane
     * \param instance iGeom instance handle
     * \param geom_entity the entity to reflect
     * \param point_x  x coordinate of the point that the reflecting plane goes though
     * \param point_y  y coordinate of the point that the reflecting plane goes 
though
     * \param point_z  z coordinate of the point that the reflecting plane goes 
though
     * \param plane_normal_x x coordinate of the plane's normal
     * \param plane_normal_y y coordinate of the plane's normal
     * \param plane_normal_z z coordinate of the plane's normal
     * \param *err Pointer to error type returned from function
     */
  void iGeom_reflectEnt( iBase_EntityHandle geom_entity,
                         double point_x,
                         double point_y,
                         double point_z,
                         double plane_normal_x,
                         double plane_normal_y,
                         double plane_normal_z,
                         int* err );

   /**\brief  Scale an entity in the x, y, and z directions
     *
     * Scale an entity in the x, y, and z directions.
     * \param instance iGeom instance handle
     * \param geom_entity the entity to scale
     * \param point_x  x coordinate of the scaling center
     * \param point_y  y coordinate of the scaling center
     * \param point_z  z coordinate of the scaling center
     * \param scale_x factor to scale by in the x direction
     * \param scale_y factor to scale by in the y direction
     * \param scale_z factor to scale by in the z direction
     * \param *err Pointer to error type returned from function
     */
  void iGeom_scaleEnt( iBase_EntityHandle geom_entity,
                       double point_x,
                       double point_y,
                       double point_z,
                       double scale_x,
                       double scale_y,
                       double scale_z,
                       int* err );

    /**\brief  Geometrically unite entities
     *
     * Geometrically unite the specified entities.
     * \param instance iGeom instance handle
     * \param geom_entities Array of entity handles being united
     * \param geom_entities_size Number of entities in geom_entities array
     * \param geom_entity Pointer to new entity handle returned from function
     * \param *err Pointer to error type returned from function
     */
  void iGeom_uniteEnts( iBase_EntityHandle const* geom_entities,
                        int geom_entities_size,
                        iBase_EntityHandle* geom_entity,
                        int* err );

    /**\brief  Geometrically subtract one entity from another
     *
     * Geometrically subtract the entity tool from the entity blank.
     * \param instance iGeom instance handle
     * \param blank The entity to subtract from
     * \param tool The entity to subtract
     * \param geom_entity Pointer to new entity handle returned from function
     * \param *err Pointer to error type returned from function
     */
  void iGeom_subtractEnts( iBase_EntityHandle blank,
                           iBase_EntityHandle tool,
                           iBase_EntityHandle* geom_entity,
                           int* err );

    /**\brief  Geometrically intersect a pair of entities
     *
     * Geometrically intersect a pair of entities.
     * \param instance iGeom instance handle
     * \param entity1 The entity to intersect
     * \param entity2 The entity to intersect
     * \param geom_entity Pointer to new entity handle returned from function
     * \param *err Pointer to error type returned from function
     */
  void iGeom_intersectEnts( iBase_EntityHandle entity2,
                            iBase_EntityHandle entity1,
                            iBase_EntityHandle* geom_entity,
                            int* err );

    /**\brief  Section (cut) a region with a plane
     *
     * Section (cut) a region with a plane, retaining one of the pieces and
     * discarding the other.
     * \param instance iGeom instance handle
     * \param geom_entity The entity to section
     * \param plane_normal_x x coordinate of the plane's normal
     * \param plane_normal_y y coordinate of the plane's normal
     * \param plane_normal_z z coordinate of the plane's normal
     * \param offset Distance of the plane from the origin
     * \param reverse Keep the piece on the normal's side (=0) or not (=1)
     * \param geom_entity2 Pointer to new entity handle returned from function
     * \param *err Pointer to error type returned from function
     */
  void iGeom_sectionEnt( iBase_EntityHandle geom_entity,
                         double plane_normal_x,
                         double plane_normal_y,
                         double plane_normal_z,
                         double offset,
                         int reverse,
                         iBase_EntityHandle* geom_entity2,
                         int* err );

    /**\brief  Imprint entities
     *
     * Imprint entities by merging coincident surfaces.
     * \param instance iGeom instance handle
     * \param geom_entities Array of entity handles being imprinted
     * \param geom_entities_size Number of entities in geom_entities array
     * \param *err Pointer to error type returned from function
     */
  void iGeom_imprintEnts( iBase_EntityHandle const* geom_entities,
                          int geom_entities_size,
                          int* err );

    /**\brief  Merge ents
     *
     * Merge entities of corresponding topology/geometry within the specified
     * tolerance.
     * \param instance iGeom instance handle
     * \param geom_entities Array of entity handles being imprinted
     * \param geom_entities_size Number of entities in geom_entities array
     * \param tolerance Tolerance within which entities are considered the same
     * \param *err Pointer to error type returned from function
     */
  void iGeom_mergeEnts( iBase_EntityHandle const* geom_entities,
                        int geom_entities_size,
                        double tolerance,
                        int* err );

    /**\brief  Create an entity set
     *
     * Create an entity set, either ordered (isList=1) or unordered 
     * (isList=0).  Unordered entity sets can contain a given entity or 
     * set only once.
     * \param instance iGeom instance handle
     * \param isList If non-zero, an ordered list is created, otherwise an
     *        unordered set is created.
     * \param entity_set_created Entity set created by function
     * \param *err Pointer to error type returned from function
     */
  void iGeom_createEntSet(// int isList,
                           iBase_EntitySetHandle* entity_set_created, 
                           int *err );

    /**\brief  Add an entity to a set
     *
     * Add an entity to a set
     * \param instance iGeom instance handle
     * \param entity_handle The entity being added
     * \param entity_set Pointer to the set being added to
     * \param *err Pointer to error type returned from function
     */
  void iGeom_addEntToSet( iBase_EntityHandle entity_handle,
                          iBase_EntitySetHandle entity_set, 
                          int *err );

    /**\brief  Get size of a tag in units of bytes
     *
     * Get size of a tag in units of bytes
     * \param instance iGeom instance handle
     * \param tag_handle Handle of tag being queried
     * \param tag_size Pointer to tag size returned from function
     * \param *err Pointer to error type returned from function
     */
  void iGeom_getTagSizeBytes( iGeom_Instance instance,
                              iBase_TagHandle tag_handle,
                              int *tag_size, 
                              int *err );

    /**\brief  Get a the handle of an existing tag with the specified name
     *
     * Get a the handle of an existing tag with the specified name
     * \param instance iGeom instance handle
     * \param tag_name Name of tag being queried
     * \param tag_handle Pointer to tag handle returned from function
     * \param *err Pointer to error type returned from function
     * \param tag_name_len Length of tag name string
     */
  void iGeom_getTagHandle( iGeom_Instance instance,
                           const char* tag_name,
                           iBase_TagHandle *tag_handle, 
                           int *err,
                           int tag_name_len );

    /**\brief  Set a tag value of arbitrary type on an entity set
     *
     * Set a tag value of arbitrary type on an entity set. The tag data
     * is passed as void*. tag_value_size specifies the size of the memory
     * pointed to by tag_value in terms of bytes. Applications are free to
     * use this function to set data of any type, not just iBase_BYTES.
     * However, in all cases, the size specified by tag_value_size is
     * always in terms of bytes.
     *
     * \param instance iGeom instance handle
     * \param entity_set_handle Entity set on which tag is being set
     * \param tag_handle Tag being set on an entity set
     * \param tag_value Pointer to tag data being set on entity set
     * \param tag_value_size Size in bytes of tag data
     * \param *err Pointer to error type returned from function
     */
  void iGeom_setEntSetData( iGeom_Instance instance,
                            iBase_EntitySetHandle entity_set_handle,
                            const iBase_TagHandle tag_handle,
                            const void* tag_value,
                           /* const int tag_value_size,AAAAAAAAAAAAAAAAAAAAAA*/
                            int *err );

    /**\brief  Set a tag value of arbitrary type on an entity
     *
     * Set a tag value of arbitrary type on an entity.  Tag data
     * is passed as void*. tag_value_size specifies the size of the
     * memory pointed to by tag_value in terms of bytes. Applications may
     * use this function to set data of any type, not just iBase_BYTES.
     * However, because this function supports data of arbitrary type, in
     * all cases the size specified by tag_value_size is always in terms
     * of bytes.
     *
     * \param instance iGeom instance handle
     * \param entity_handle Entity on which tag is being set
     * \param tag_handle Tag being set on an entity
     * \param tag_value Pointer to tag data being set on entity
     * \param tag_value_size Size in bytes of tag data
     * \param *err Pointer to error type returned from function
     */
  void iGeom_setData( iGeom_Instance instance,
                      iBase_EntityHandle entity_handle,
                      iBase_TagHandle tag_handle,
                      const void* tag_value,
                    /*  int tag_value_size,AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAa*/ 
                      int *err );















//Might need these for libraries

    /**\brief  Initialize an array iterator over specified entity type and size
     *
     * Initialize an array iterator over specified entity type and size for a
     * specified set or instance.  Iterator returned can be used as input to
     * functions returning entities for the iterator.  If all entities of a
     * specified type are to be iterated, specify iBase_ALL_TYPES. Specified
     * type must be a value in the iBase_EntityType enumerations.
     * \param instance iGeom instance handle
     * \param entity_set_handle Entity set being iterated
     * \param requested_entity_type Type of entity to iterate
     * \param requested_array_size Size of chunks of handles returned for each
     *        value of the iterator
     * \param entArr_iterator Pointer to iterator returned from function
     * \param *err Pointer to error type returned from function
     */
  void iGeom_initEntArrIter( iGeom_Instance instance,
                             iBase_EntitySetHandle entity_set_handle,
                             int requested_entity_type,
                             int requested_array_size,
                             iBase_EntityArrIterator* entArr_iterator,
                             int* err );

    /**\brief  Get the data type of the specified tag handle
     *
     * Get the data type of the specified tag handle.  Tag type is a value in
     * the iBase_TagType enumeration.
     * \param instance iGeom instance handle
     * \param tag_handle Handle for the tag being queried
     * \param tag_type Pointer to tag type returned from function
     * \param *err Pointer to error type returned from function
     */
  void iGeom_getTagType( iGeom_Instance instance,
                         iBase_TagHandle tag_handle,
                         int *tag_type, 
                         int *err );

    /**\brief  Remove an entity from a set
     *
     * Remove an entity from a set
     *
     * \param instance iGeom instance handle
     * \param entity_handle The entity being removed
     * \param entity_set Pointer to the set being removed from
     * \param *err Pointer to error type returned from function
     */
  void iGeom_rmvEntFromSet( iGeom_Instance instance,
                            iBase_EntityHandle entity_handle,
                            iBase_EntitySetHandle entity_set, 
                            int *err );
























#ifdef __cplusplus
} /* extern "C" */
#endif

#endif
