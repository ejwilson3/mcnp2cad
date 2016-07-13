/**
 * Copyright 2006 Sandia Corporation.  Under the terms of Contract
 * DE-AC04-94AL85000 with Sandia Coroporation, the U.S. Government
 * retains certain rights in this software.
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 */
 
/**\file iGeom_CGMA.cc
 *
 *\author Tim Tautges
 *\author Jason Kraftcheck
 *
 * Original file from SNL TSTT repository was named TSTTG_CGM.cpp.
 *
 * Renamed iGeom_CGMA.cc and added to ANL ITAPS repository by J.Kraftcheck,
 * 2007-6-15
 */
#include "InitCGMA.hpp"
#include "GeometryQueryTool.hpp"
#include "CubitCompat.hpp"

#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#ifdef ITAPS_SHIM
# include "iGeom_private.h"
# include "iGeom_binding.h"
# include "CGM_iGeom.h"
# define ITAPS_API static
#else
# include "iGeom.h"
# define ITAPS_API
#endif

const bool debug = false;

#define RETURN(a) do {CGM_iGeom_setLastError((*err = a)); return; } while(false)
#define ERROR(a, b) do {CGM_iGeom_setLastError((*err = a), b); return; } while(false)

#define ARRAY_IN(b) \
  b 

#define ARRAY_INOUT(b) \
  b, b ## _allocated, b ## _size  

// Check the array size, and allocate the array if necessary.
// Free the array upon leaving scope unless KEEP_ARRAY
// is invoked.
#define ALLOC_CHECK_ARRAY(array, this_size) \
  iGeomArrayManager array ## _manager ( reinterpret_cast<void**>(array), *(array ## _allocated), *(array ## _size), this_size, sizeof(**array), err ); \
  if (iBase_SUCCESS != *err) return

#define KEEP_ARRAY(array) \
  array ## _manager .keep_array()

// Check the array size, and allocate the array if necessary.
// Do NOT free the array upon leaving scope.
#define ALLOC_CHECK_ARRAY_NOFAIL(array, this_size) \
  ALLOC_CHECK_ARRAY(array, this_size); KEEP_ARRAY(array)

#define TAG_HANDLE(handle) reinterpret_cast<long>(handle)

#define ENTITY_HANDLE(handle) reinterpret_cast<RefEntity*>(handle)
#define SET_HANDLE(handle) reinterpret_cast<RefGroup*>(handle)

#define ENTITY_HANDLE_ARRAY(array) reinterpret_cast<RefEntity**>(array)
#define SET_HANDLE_ARRAY(array) reinterpret_cast<RefGroup**>(array)

#define ENTITY_HANDLE_CONST_ARRAY(array) reinterpret_cast<RefEntity* const*>(array)
#define SET_HANDLE_CONST_ARRAY(array) reinterpret_cast<RefGroup* const*>(array)

#define ENTITY_HANDLE_ARRAY_PTR(array) reinterpret_cast<RefEntity***>(array)
#define SET_HANDLE_ARRAY_PTR(array) reinterpret_cast<RefGroup***>(array)

#define CAST_TO_VOID(ptr) reinterpret_cast<void*>(ptr)
#define TAG_HANDLE_ARRAY_INOUT(a) reinterpret_cast<long**>(a), a ## _allocated, a ## _size

#define TM reinterpret_cast<CGMTagManager*>(instance)

const double RAD_TO_DEG = 180.0 / acos(-1.0);
const double DEG_TO_RAD = 1.0 / RAD_TO_DEG;

#include "CATag.hpp"
#include "iGeomError.h"

#include "RefEntityFactory.hpp"
#include "RefGroup.hpp"
#include "Body.hpp"
#include "RefVertex.hpp"
#include "RefEdge.hpp"
#include "RefFace.hpp"

#ifdef USE_MPI
#include "CGMReadParallel.hpp"
#endif

#include "GeometryModifyTool.hpp"
#include "MergeTool.hpp"

#define gqt GeometryQueryTool::instance()
#define gmt GeometryModifyTool::instance()

const char *iGeom_entity_type_names[] = {"vertex", "curve", "surface", "body"};
const char *cgm_type_names[] = {"vertex", "curve", "surface", "volume", "body"};

// Implement RAII pattern for allocated arrays
class iGeomArrayManager
{
  void** arrayPtr;

public:


  iGeomArrayManager( void** array_ptr,
                     int& array_allocated_space,
                     int& array_size,
                     int count,
                     int val_size,
                     int* err ) : arrayPtr(0)
  {
    if (!*array_ptr) {
      *array_ptr = malloc(val_size * count);
      array_allocated_space = array_size = count;
      if (!*array_ptr) {
        ERROR(iBase_MEMORY_ALLOCATION_FAILED, "Couldn't allocate array.");
      }
      arrayPtr = array_ptr;
    }
    else {
      array_size = count;
      if (array_allocated_space < count) {
        ERROR(iBase_BAD_ARRAY_DIMENSION, 
          "Allocated array not large enough to hold returned contents.");
      }
    }
    RETURN(iBase_SUCCESS);
  }
  
  ~iGeomArrayManager() 
  {
    if (arrayPtr) {
      free(*arrayPtr);
      *arrayPtr = 0;
    }
  }
  
  void keep_array()
    { arrayPtr = 0; }
};


// declare private-type functions here, so they aren't visible outside
// this implementation file

/* Helper functions 
 * Declare these outside the 'extern "C"' block because they are not
 * extern.
 */
 
static void tokenize( const std::string& str, 
                      std::vector<std::string>& tokens );

// Expect option of the form "NAME=VALUE".
// If NAME portion matches, pass back VALUE and return true.
// Otherwise, leave 'value' unchanged and return false.
static bool match_option( const std::string& opt,
                          const char* name,
                          std::string& value );
 
static CubitStatus
iGeom_bounding_box( RefEntity* entity, CubitVector& minc, CubitVector& maxc );

/** Smits' algorithm */
static inline void box_min_max( double dir,
                                double min,
                                double max,
                                double pt,
                                double& tmin,
                                double& tmax );


static
int count_ibase_type( int ibase_type, 
                      const DLIList<CubitEntity*>& list, 
                      int* err );

static 
void copy_ibase_type( int ibase_type, 
                      const DLIList<CubitEntity*>& list,
                      iBase_EntityHandle** entity_handles,
                      int* entity_handles_alloc,
                      int* entity_handles_size,
                      int* err );
static 
void append_all_ibase_type( int ibase_type, 
                            DLIList<RefEntity*>& target_list,
                            int* err );

static CubitStatus init_cgm( const std::string& engine )
{
  CubitStatus status;
  if (engine.empty()) 
    status = InitCGMA::initialize_cgma();
  else
    status = InitCGMA::initialize_cgma( engine.c_str() );
 
// sometimes can't have following, depending on CGM version
  // CGMApp::instance()->attrib_manager()->silent_flag(true);

  CGMApp::instance()->attrib_manager()->auto_flag(true);
  
  return status;
}

extern "C" {

#ifdef ITAPS_SHIM
static struct iGeom_vtable CGM_iGeom_vtable = {
  iGeom_getDescription,
  iGeom_save,
  iGeom_getRootSet,
  iGeom_getEntities,
  iGeom_getNumOfType,
  iGeom_getEntBoundBox,
  iGeom_copyEnt,
  iGeom_deleteEnt,
  iGeom_createSphere,
  iGeom_createBrick,
  iGeom_createCylinder,
  iGeom_createCone,
  iGeom_createTorus,
  iGeom_moveEnt,
  iGeom_rotateEnt,
  iGeom_reflectEnt,
  iGeom_scaleEnt,
  iGeom_uniteEnts,
  iGeom_subtractEnts,
  iGeom_intersectEnts,
  iGeom_sectionEnt,
  iGeom_imprintEnts,
  iGeom_mergeEnts,
  iGeom_createEntSet,
  iGeom_getEntSets,
  iGeom_addEntToSet,
  iGeom_getTagSizeBytes,
  iGeom_getTagHandle,
  iGeom_setEntSetData,
  iGeom_setEntSetIntData,
  iGeom_setData,
};

void CGM_iGeom_register()
{
  iGeom_register("cgm", iGeom_newGeom);
}
#endif


ITAPS_API void
iGeom_getErrorType( int *error_type )
{
  *error_type = CGM_iGeom_getLastErrorType();
}

ITAPS_API void
iGeom_getDescription( char* description_buffer,
                      int description_buffer_length )
{
  CGM_iGeom_getLastErrorDesc(description_buffer, description_buffer_length);
}

ITAPS_API void
iGeom_newGeom( const char* options,
               iGeom_Instance* instance_out,
               int* err,
               const int options_size ) 
{
    // scan options for default engine option
  std::string engine;
  if (options && options_size) {
    std::string tmp(options, options_size);
    char f[] = ";engine="; f[0]=tmp[0]; // correct delimiter
    size_t p = tmp.find( f );
    if (p != std::string::npos) { // if we found engine option
      p += strlen(f); // advance to value (past '=')
      size_t e = tmp.find( tmp[0], p ); // find end delimiter
      if (e == std::string::npos) // if no end delim, then must be last option
        engine = tmp.substr( p, std::string::npos );
      else
        engine = tmp.substr( p, e-p );
    }
  }
  
    // initialize static var with result so that call happens only once
  static const CubitStatus status = init_cgm( engine );
    // but check the result for every call
  if (CUBIT_SUCCESS != status)
    RETURN (iBase_FAILURE);

    // return the tagmanager as the instance
#ifdef ITAPS_SHIM
  CGMTagManager::instance().vtable = &CGM_iGeom_vtable;
#endif
  *instance_out = reinterpret_cast<iGeom_Instance>(&CGMTagManager::instance());
  RETURN(iBase_SUCCESS);
}

ITAPS_API void
iGeom_save( /*in*/ const char *name,
            /*in*/ const char* options,
            int* err,
            int name_len,
            int options_len )
{
    // make sure strings are null terminated
  std::string name_buf(name, name_len);
  name = name_buf.c_str();
  
    // parse options
  std::string file_type;
  std::vector<std::string> opts;
  tokenize( std::string(options, options_len), opts );
  for (std::vector<std::string>::iterator i = opts.begin(); i != opts.end(); ++i)
  {
    if (!match_option( *i, "TYPE", file_type )) // e.g TYPE=ACIS_SAT
      ERROR( iBase_INVALID_ARGUMENT, i->c_str() );
  }

    // if no type, check file type for known extensions
  if (file_type.empty()) {
    size_t name_len = name_buf.length();
    if (name_buf.find(".igs") < name_len ||
        name_buf.find(".IGS") < name_len ||
        name_buf.find(".iges") < name_len ||
        name_buf.find(".IGES") < name_len)
      file_type = "IGES";
    else if (name_buf.find(".stp") < name_len ||
             name_buf.find(".STP") < name_len ||
             name_buf.find(".step") < name_len ||
             name_buf.find(".STEP") < name_len)
      file_type = "STEP";
    else if (name_buf.find(".sat") < name_len ||
             name_buf.find(".SAT") < name_len)
      file_type = "ACIS_SAT";
    else if (name_buf.find(".occ") < name_len ||
             name_buf.find(".brep") < name_len ||
             name_buf.find(".OCC") < name_len ||
             name_buf.find(".BREP") < name_len)
      file_type = "OCC";
    else
      ERROR(iBase_FAILURE, "Unknown geometry file extension and no file type.");
  }
  
  /** 
   * Work around AcisQueryEngine log file handling bug:
   * If the export format is iges or step, be sure the log file name is not null
   */
  const char* logfile_name = NULL;
  if( file_type == "IGES" ){
    logfile_name = "igeom_iges_export.log";
  }
  else if( file_type == "STEP" ){
    logfile_name = "igeom_step_export.log";
  }

    // process options (none right now...)
  DLIList<RefEntity*> bodies;
  int num_ents_exported;
  CubitString cubit_version(" (iGeom)");
  CubitStatus status = CubitCompat_export_solid_model(bodies, name, file_type.c_str(),
                                               num_ents_exported, cubit_version, logfile_name );
  if (CUBIT_SUCCESS != status) 
    ERROR(iBase_FAILURE, "Trouble saving geometry file.");

  RETURN(iBase_SUCCESS);
}

ITAPS_API void
iGeom_getRootSet( iGeom_Instance,
                  iBase_EntitySetHandle* root,
                  int* err )
{
  *root = NULL;
  RETURN(iBase_SUCCESS);
}

ITAPS_API void
iGeom_getEntities( /*in*/ iBase_EntitySetHandle set_handle,
                   /*in*/ int gentity_type,
                   /*out*/ iBase_EntityHandle **gentity_handles,
                   int *gentity_handles_allocated,
                   int *gentity_handles_size,
                   int* err )
{
  if (RefGroup *this_set = SET_HANDLE(set_handle)) {
    static DLIList<CubitEntity*> centities;
    centities.clean_out();
    this_set->get_child_entities(centities);
    copy_ibase_type( gentity_type, centities, 
                     gentity_handles,
                     gentity_handles_allocated,
                     gentity_handles_size,
                     err );
  }
  else {
    static DLIList<RefEntity*> dim_entities;
    dim_entities.clean_out();
    append_all_ibase_type( gentity_type, dim_entities, err );
    if (iBase_SUCCESS != *err)
      return;
    
    ALLOC_CHECK_ARRAY_NOFAIL(gentity_handles, dim_entities.size());
    dim_entities.copy_to((RefEntity**)*gentity_handles);
    RETURN(iBase_SUCCESS);
  }  
}

/**
 * Return number of gentities of specified dimension in this set, or in
 * whole model.
 * @param set_handle Entity set being queried (if 0, whole model)
 * @param gentity_dimension Dimension of entities being queried
 * @return Number of entities
 */



ITAPS_API void
iGeom_getNumOfType( /*in*/ iBase_EntitySetHandle set_handle,
                    /*in*/ int gentity_type,
                    int* count,
                    int* err )
{
  const RefGroup *this_set = SET_HANDLE(set_handle);
  if (0 == this_set) {
    switch (gentity_type) {
      case iBase_ALL_TYPES:
        *count  = GeometryQueryTool::instance()->num_bodies();
        *count += GeometryQueryTool::instance()->num_ref_faces();
        *count += GeometryQueryTool::instance()->num_ref_edges();
        *count += GeometryQueryTool::instance()->num_ref_vertices();
        break;
      case iBase_REGION:
        *count = GeometryQueryTool::instance()->num_bodies();
        break;
      case iBase_FACE:
        *count = GeometryQueryTool::instance()->num_ref_faces();
        break;
      case iBase_EDGE:
        *count = GeometryQueryTool::instance()->num_ref_edges();
        break;
      case iBase_VERTEX:
        *count = GeometryQueryTool::instance()->num_ref_vertices();
        break;
      default:
        RETURN(iBase_BAD_TYPE_AND_TOPO);
        break;
    }
    RETURN (iBase_SUCCESS);
  }
  else {
    static DLIList<CubitEntity*> centities;
    centities.clean_out();
    const_cast<RefGroup*>(this_set)->get_child_entities(centities);
    *count = count_ibase_type( gentity_type, centities, err );
  }
}

/**
 *   Get the size of the data associated with a given tag handle.
 */
ITAPS_API void
iGeom_getTagSizeBytes( iGeom_Instance instance,
                       /*in*/ iBase_TagHandle tag_handle,
                       int* tag_size,
                       int* err )
{
  *tag_size = TM->getTagSize(TAG_HANDLE(tag_handle));
  RETURN(iBase_SUCCESS);
}


/**
 *     Get the tag handle associated with a given string name.
 */
ITAPS_API void
iGeom_getTagHandle( iGeom_Instance instance,
                    /*in*/ const char *tag_name,
                    iBase_TagHandle* tag_handle,
                    int* err,
                    int tag_name_len )
{
    // make sure string is null-terminated
  std::string tag_name_buf( tag_name, tag_name_len );
  tag_name = tag_name_buf.c_str();
  *tag_handle = reinterpret_cast<iBase_TagHandle>(static_cast<size_t>(TM->getTagHandle( tag_name )));

    // XXX: this seems really wrong...
  iGeom_getErrorType(err);
}


/**
 *   Allows the user to set the tag data values on an array of gentity
 *   handles
 */
ITAPS_API void
iGeom_setData( iGeom_Instance instance,
               /*in*/ iBase_EntityHandle entity_handle,
               /*in*/ iBase_TagHandle tag_handle,
               /*in*/ const void *tag_value_tmp,
               int* err )
{
  const char *tag_value = reinterpret_cast<const char *>(tag_value_tmp);
  RefEntity *tmp_entity = ENTITY_HANDLE(entity_handle);
  iBase_ErrorType retval = TM->setArrData(&tmp_entity, 1, 
                                          TAG_HANDLE(tag_handle), 
                                          ARRAY_IN(tag_value));
  RETURN(retval);
}


/**
 *   Set the tag data associated with a given tag handle on the mesh or
 *   gentity_set
 */
ITAPS_API void
iGeom_setEntSetData( iGeom_Instance instance,
                     /*in*/ iBase_EntitySetHandle entity_set,
                     /*in*/ iBase_TagHandle tag_handle,
                     /*in*/ const void *tag_value_tmp,
                     int* err )
{
  const char *tag_value = reinterpret_cast<const char *>(tag_value_tmp);
    // have to go through RefEntity* so that RefEntity** gets set right
  RefEntity *tmp_entity = SET_HANDLE(entity_set);
  iBase_ErrorType retval = TM->setArrData(&tmp_entity, 1, TAG_HANDLE(tag_handle), 
                                          ARRAY_IN(tag_value));
  RETURN(retval);
}


ITAPS_API void
iGeom_setEntSetIntData( iGeom_Instance instance,
                        /*in*/ iBase_EntitySetHandle entity_set,
                        /*in*/ iBase_TagHandle tag_handle,
                        /*in*/ int tag_value,
                        int* err )
{
  iGeom_setEntSetData(instance, entity_set, tag_handle, 
                      &tag_value, err);
}

ITAPS_API void
iGeom_getEntBoundBox( /*in*/ iBase_EntityHandle entity_handle,
                      /*out*/ double* min_x,
                      /*out*/ double* min_y,
                      /*out*/ double* min_z,
                      /*out*/ double* max_x,
                      /*out*/ double* max_y,
                      /*out*/ double* max_z,
                      int* err )
{
  RefEntity* entity = (RefEntity*)entity_handle;
  CubitVector minc, maxc;
  CubitStatus status = iGeom_bounding_box( entity, minc, maxc );
  minc.get_xyz( *min_x, *min_y, *min_z );
  maxc.get_xyz( *max_x, *max_y, *max_z );
  RETURN( (status == CUBIT_SUCCESS ? iBase_SUCCESS : iBase_FAILURE) );
}


/**
 * This function allows a new gentity_set to be created.  The user may 
 * set the multiset, ordered, isMesh, flags as needed; otherwise default values 
 * (all false) will be used.  On creation, Entitysets are empty of
 * entities and contained in the parent geometry interface.  They must be
 * explicitly filled with entities using the addGentities call and
 * relationships with other Entitysets must be done through the
 * addEntityset and parent/child relationship calls.
 * @param multiset If true, gentities can appear more than once in this gentity_set
 * @param ordered If true, order of addition and removal is maintained for
 *   this gentity_set
 * @param gentity_set_created Entity_set created by this function
 */


ITAPS_API void
iGeom_createEntSet( /*out*/ iBase_EntitySetHandle *entity_set,
                    int* err )
{
  RefGroup* grp = RefEntityFactory::instance()->construct_RefGroup();
  *entity_set = reinterpret_cast<iBase_EntitySetHandle>(grp);
    // need to set a tag denoting multiset or not...
  if (*entity_set == NULL) {
    RETURN(iBase_FAILURE);
  }
  
  else {
    RETURN(iBase_SUCCESS);
  }
}

ITAPS_API void
iGeom_addEntToSet( /*in*/ iBase_EntityHandle entity_to_add,
                   /*inout*/ iBase_EntitySetHandle entity_set_handle,
                   int* err )
{
  if (NULL == entity_to_add) RETURN(iBase_INVALID_ARGUMENT);
  
  CubitStatus status = SET_HANDLE(entity_set_handle)->
    add_ref_entity(const_cast<RefEntity*>(ENTITY_HANDLE(entity_to_add)));
  
  if (CUBIT_SUCCESS != status) {
    ERROR(iBase_FAILURE, "Problem adding entity to another set.");
  }

  RETURN(iBase_SUCCESS);
}


ITAPS_API void
iGeom_copyEnt( /*in*/ iBase_EntityHandle geom_entity,
               /*out*/ iBase_EntityHandle *geom_entity2,
               int* err )
{
  Body *this_body = dynamic_cast<Body*>(ENTITY_HANDLE(geom_entity));
  RefVolume *this_vol = dynamic_cast<RefVolume*>(ENTITY_HANDLE(geom_entity));
  if (NULL != this_vol || NULL != this_body) {
      // need to get the associated body, since cgm only supports copying bodies,
      // not volumes
    if (NULL == this_body) {
      this_body = this_vol->get_body_ptr();
      if (NULL == this_body) {
        ERROR(iBase_FAILURE, "Can't get body from volume.");
      }
    }

    RefEntity *temp_entity = gmt->copy_body(this_body);
    *geom_entity2 = reinterpret_cast<iBase_EntityHandle>(temp_entity);
  }
  else {
    RefEntity *this_ent = ENTITY_HANDLE(geom_entity);
    RefEntity *temp_entity = gmt->copy_refentity(this_ent);
    *geom_entity2 = reinterpret_cast<iBase_EntityHandle>(temp_entity);
  }

  if (NULL == *geom_entity2) {
    ERROR(iBase_FAILURE, "NULL returned from CGM copy.");
  }
  
  RETURN(iBase_SUCCESS);
}

ITAPS_API void
iGeom_deleteEnt( /*in*/ iBase_EntityHandle geom_entity,
                 int* err )
{
  RefEntity *this_ent = ENTITY_HANDLE(geom_entity);

    // special case: if this is a volume, delete the body instead
  CubitStatus result;
  RefVolume *this_vol = CAST_TO(this_ent, RefVolume);
  if (NULL != this_vol)
    result = gqt->delete_Body(this_vol->body());
  else    
    result = gqt->delete_RefEntity(this_ent);

  if (CUBIT_FAILURE == result) {
    ERROR(iBase_FAILURE, "Problems deleting entity.");
  }

    // check to see if this was last thing deleted; if so, reset ids
  RefEntityFactory *rfi = RefEntityFactory::instance();
  if (rfi->num_bodies() == 0 && 
      rfi->num_ref_volumes() == 0 &&
      rfi->num_ref_faces() == 0 &&
      rfi->num_ref_edges() == 0 &&
      rfi->num_ref_vertices() == 0)
    rfi->reset_ids();
  
  RETURN(iBase_SUCCESS);
}


ITAPS_API void
iGeom_createSphere( double radius,
                    iBase_EntityHandle *geom_entity,
                    int* err )
{
  if (radius <= 0.0) {
    ERROR(iBase_INVALID_ARGUMENT, "Sphere radius must be must be positive.");
  }
  
  RefEntity* tmp_body = gmt->sphere( radius );
  *geom_entity = reinterpret_cast<iBase_EntityHandle>(tmp_body);
  RETURN ((tmp_body ? iBase_SUCCESS : iBase_FAILURE));
}


ITAPS_API void
iGeom_createBrick( /*in*/ double x,
                   /*in*/ double y,
                   /*in*/ double z,
                   /*out*/ iBase_EntityHandle *geom_entity,
                   int* err )
{
  double tmp_x = x;
  double tmp_y = y;
  double tmp_z = z;
  
  if (0.0 == y && 0.0 == z) {
    tmp_y = x;
    tmp_z = x;
  }
  
  if (0.0 >= tmp_x || 0.0 >= tmp_y || 0.0 >= tmp_z) {
    ERROR(iBase_INVALID_ARGUMENT, "Dimensions must be >= 0, or y & z must both be zero.");
  }
    
  RefEntity *temp_body = gmt->brick(tmp_x, tmp_y, tmp_z);
  *geom_entity = reinterpret_cast<iBase_EntityHandle>(temp_body);

  if (NULL == *geom_entity) {
    RETURN(iBase_FAILURE);
  }

  RETURN(iBase_SUCCESS);
}
          

ITAPS_API void
iGeom_createCylinder( /*in*/ double height,
                      /*in*/ double major_rad,
                      /*in*/ double minor_rad,
                      /*out*/ iBase_EntityHandle *geom_entity,
                      int* err )
{
  double tmp_minor = (0.0 == minor_rad ? major_rad : minor_rad);
  RefEntity *temp_body = 
    gmt->cylinder(height, major_rad, tmp_minor, major_rad);
  *geom_entity = reinterpret_cast<iBase_EntityHandle>(temp_body);


  if (NULL == *geom_entity) {
    RETURN(iBase_FAILURE);
  }

  RETURN(iBase_SUCCESS);
}
      
ITAPS_API void
iGeom_createCone( /*in*/ double height,
                  /*in*/ double major_rad_base,
                  /*in*/ double minor_rad_base,
                  /*in*/ double rad_top,
                  /*out*/ iBase_EntityHandle *geom_entity,
                  int* err )
{
  double tmp_minor = (0.0 == minor_rad_base ? major_rad_base : minor_rad_base);
  RefEntity *temp_body = 
    gmt->cylinder(height, major_rad_base, tmp_minor, rad_top);
  *geom_entity = reinterpret_cast<iBase_EntityHandle>(temp_body);


  if (NULL == *geom_entity) {
    RETURN(iBase_FAILURE);
  }

  RETURN(iBase_SUCCESS);
}
    
ITAPS_API void
iGeom_createTorus( /*in*/ double major_rad,
                   /*in*/ double minor_rad,
                   /*out*/ iBase_EntityHandle *geom_entity,
                   int* err )
{
  if (minor_rad >= major_rad) {
    ERROR(iBase_INVALID_ARGUMENT, "Major radius must be greater than minor radius for tori.");
  }
  
  RefEntity *temp_body = gmt->torus(major_rad, minor_rad);
  *geom_entity = reinterpret_cast<iBase_EntityHandle>(temp_body);
   
  if (NULL == *geom_entity) {
    RETURN(iBase_FAILURE);
  }

  RETURN(iBase_SUCCESS);
}

ITAPS_API void
iGeom_moveEnt( /*inout*/ iBase_EntityHandle geom_entity,
               /*in*/ double x,
               /*in*/ double y,
               /*in*/ double z,
               int* err )
{
  CubitVector vec(x, y, z);
  Body *this_bod = dynamic_cast<Body*>(ENTITY_HANDLE(geom_entity));
  CubitStatus result;
  if (NULL != this_bod) {
    result = gqt->translate(this_bod, vec);
    if (CUBIT_SUCCESS != result) {
      ERROR(iBase_FAILURE, "Failed to move body.");
      RETURN(iBase_FAILURE);
    }
    
    RETURN(iBase_SUCCESS);
  }
  
  BasicTopologyEntity *this_bte = dynamic_cast<BasicTopologyEntity*>(ENTITY_HANDLE(geom_entity));
  if (NULL != this_bte) {
      // non-body move; check to see if there are any siblings to this entity in the
      // same body; if so, we can't move it; if not, get the body and move that; if
      // there is no body, it's a free entity and we can move it anyway
    Body *this_body = this_bte->body();
    if (NULL == this_body) {
      result = gqt->translate(this_bte, vec);
      if (CUBIT_SUCCESS != result) {
        ERROR(iBase_FAILURE, "Failed to move entity.");
      }
    }
    else {
      int num_sibs = -1;
      switch (this_bte->dimension()) {
        case 0: num_sibs = this_body->num_ref_vertices(); break;
        case 1: num_sibs = this_body->num_ref_edges(); break;
        case 2: num_sibs = this_body->num_ref_faces(); break;
      }
      if (num_sibs == 1) {
          // ok to move the body instead
        result = gqt->translate(this_body, vec);
        if (CUBIT_SUCCESS != result) {
          ERROR(iBase_FAILURE, "Failed to move body even only one entity of that"
                             " dimension in the body.");
        }
      }
      else {
        ERROR(iBase_FAILURE, "Too many siblings for an entity to move it.");
      }
    }

    RETURN(iBase_SUCCESS);
  }
  
  ERROR(iBase_INVALID_ENTITY_TYPE, "Wrong type of entity specified for move.");
}


ITAPS_API void
iGeom_rotateEnt( /*inout*/ iBase_EntityHandle geom_entity,
                 /*in*/ double angle,
                 /*in*/ double axis_normal_x,
                 /*in*/ double axis_normal_y,
                 /*in*/ double axis_normal_z,
                 int* err )
{
  CubitVector this_axis(axis_normal_x, axis_normal_y, axis_normal_z);
  Body *this_bod = dynamic_cast<Body*>(ENTITY_HANDLE(geom_entity));
  CubitStatus result;
  if (NULL != this_bod) {
    result = gqt->rotate(this_bod, this_axis, angle);
    if (CUBIT_SUCCESS != result) {
      ERROR(iBase_FAILURE, "Failed to rotate body.");
    }
    
    RETURN(iBase_SUCCESS);
  }
  
  BasicTopologyEntity *this_bte = dynamic_cast<BasicTopologyEntity*>(ENTITY_HANDLE(geom_entity));
  if (NULL != this_bte) {
    result = gqt->rotate(this_bte, this_axis, angle);
    if (CUBIT_SUCCESS != result) {
      ERROR(iBase_FAILURE, "Failed to rotate entity.");
    }
    
    RETURN(iBase_SUCCESS);
  }
  
  ERROR(iBase_INVALID_ENTITY_TYPE, "Wrong type of entity specified for move.");
}

ITAPS_API void
iGeom_reflectEnt( /*inout*/ iBase_EntityHandle geom_entity,
                  /*in*/ double point_x,
                  /*in*/ double point_y,
                  /*in*/ double point_z,
                  /*in*/ double plane_normal_x,
                  /*in*/ double plane_normal_y,
                  /*in*/ double plane_normal_z,
                  int* err )
{
  CubitVector this_plane(plane_normal_x, plane_normal_y, plane_normal_z);
  CubitVector point(point_x, point_y, point_z);
  Body *this_bod = dynamic_cast<Body*>(ENTITY_HANDLE(geom_entity));
  DLIList<Body*> bods;
  bods.append(this_bod);
  CubitStatus result;
  if (NULL != this_bod) {
    result = gqt->reflect(bods, point , this_plane);
    if (CUBIT_SUCCESS != result) {
      ERROR(iBase_FAILURE, "Failed to reflect body.");
    }
    
    RETURN(iBase_SUCCESS);
  }
  
  BasicTopologyEntity *this_bte = dynamic_cast<BasicTopologyEntity*>(ENTITY_HANDLE(geom_entity));
  if (NULL != this_bte) {
    result = gqt->reflect(this_bte, point, this_plane);
    if (CUBIT_SUCCESS != result) {
      ERROR(iBase_FAILURE, "Failed to reflect entity.");
    }
    
    RETURN(iBase_SUCCESS);
  }
  
  ERROR(iBase_INVALID_ENTITY_TYPE, "Wrong type of entity specified for reflect.");
}

ITAPS_API void
iGeom_scaleEnt( /*inout*/ iBase_EntityHandle geom_entity,
                /*in*/ double point_x,
                /*in*/ double point_y,
                /*in*/ double point_z,
                /*in*/ double scale_x,
                /*in*/ double scale_y,
                /*in*/ double scale_z,
                int* err )
{
  CubitVector factor(scale_x, scale_y, scale_z);
  CubitVector point(point_x, point_y, point_z);
  Body *this_bod = dynamic_cast<Body*>(ENTITY_HANDLE(geom_entity));
  CubitStatus result;
  if (NULL != this_bod) {
    result = gqt->scale(this_bod, point, factor);
    if (CUBIT_SUCCESS != result) {
      ERROR(iBase_FAILURE, "Failed to scale body.");
    }
    
    RETURN(iBase_SUCCESS);
  }
  
  BasicTopologyEntity *this_bte = dynamic_cast<BasicTopologyEntity*>(ENTITY_HANDLE(geom_entity));
    // non-body move; check to see if there are any siblings to this entity in the
    // same body; if so, we can't move it; if not, get the body and move that; if
    // there is no body, it's a free entity and we can move it anyway
  Body *this_body = this_bte->body();
  if (NULL == this_body && NULL != this_bte) {
    result = gqt->scale(this_bte, point, factor);
    if (CUBIT_SUCCESS != result) {
      ERROR(iBase_FAILURE, "Failed to scale entity.");
    }
  }
  else if (NULL != this_body && NULL != this_bte) {
    int num_sibs = -1;
    switch (this_bte->dimension()) {
      case 0: num_sibs = this_body->num_ref_vertices(); break;
      case 1: num_sibs = this_body->num_ref_edges(); break;
      case 2: num_sibs = this_body->num_ref_faces(); break;
          // for TSTT, always scale volumes
      case 3: num_sibs = 1; break;
    }
    if (num_sibs == 1) {
        // ok to scale the body instead
      result = gqt->scale(this_body, point, factor);
      if (CUBIT_SUCCESS != result) {
        ERROR(iBase_FAILURE, "Failed to scale body even only one entity of that"
                           " dimension in the body.");
      }
    }
    else {
      ERROR(iBase_FAILURE, "Too many siblings for an entity to scale it.");
    }
  }

  RETURN(iBase_SUCCESS);
}

ITAPS_API void
iGeom_uniteEnts( /*in*/ iBase_EntityHandle const* geom_entities,
                 int geom_entities_size,
                 /*out*/ iBase_EntityHandle *geom_entity,
                 int* err )
{
  DLIList<Body*> bods, orig_bods;
  RefEntity* const* handle_array = ENTITY_HANDLE_CONST_ARRAY(geom_entities);
  for (int i = 0; i < geom_entities_size; i++) {
    Body *this_body = dynamic_cast<Body*>(handle_array[i]);
    if (NULL != this_body) {
      Body *new_body = gmt->copy_body(this_body);
      if (NULL != new_body) {
        bods.append(this_body);
        orig_bods.append(new_body);
      }
    }
  }
  if (bods.size() < geom_entities_size) {
    ERROR(iBase_INVALID_ARGUMENT, "Not all entities input were regions.");
    for (int i = bods.size(); i > 0; i--)
      gqt->delete_RefEntity(bods.get_and_step());
    
    RETURN(iBase_SUCCESS);
  }
  
  DLIList<Body*> new_bods;
  CubitStatus result = gmt->unite(bods, new_bods, false);
  if (CUBIT_SUCCESS != result || 1 != new_bods.size()) {
    ERROR(iBase_FAILURE, "Unite failed.");
  }
    
  else {
    *geom_entity = reinterpret_cast<iBase_EntityHandle>(dynamic_cast<RefEntity*>(new_bods.get()));
    for (int i = orig_bods.size(); i > 0; i--)
      gqt->delete_RefEntity(orig_bods.get_and_step());
  }

  RETURN(iBase_SUCCESS);
}

ITAPS_API void
iGeom_subtractEnts( /*in*/ iBase_EntityHandle blank,
                    /*in*/ iBase_EntityHandle tool,
                    /*out*/ iBase_EntityHandle *geom_entity,
                    int* err )
{
  Body *this_blank = dynamic_cast<Body*>(ENTITY_HANDLE(blank));
  Body *blank_copy = gmt->copy_body(this_blank);
  if (NULL == blank_copy) {
    ERROR(iBase_FAILURE, "Trouble copying blank.");
  }
  Body *this_tool = dynamic_cast<Body*>(ENTITY_HANDLE(tool));
  Body *tool_copy = gmt->copy_body(this_tool);
  if (NULL == tool_copy) {
    ERROR(iBase_FAILURE, "Trouble copying tool.");
    gqt->delete_RefEntity(blank_copy);
    RETURN(iBase_FAILURE);
  }

  DLIList<Body*> blank_list, new_body_list;
  blank_list.append(blank_copy);
  
  RefEntity *new_body = NULL;
  CubitStatus result = gmt->subtract(tool_copy, blank_list, new_body_list);
  if (CUBIT_SUCCESS != result || 0 == new_body_list.size()) {
    ERROR(iBase_FAILURE, "Subtract failed.");
  }
  else {
    new_body = new_body_list.get();
    *geom_entity = reinterpret_cast<iBase_EntityHandle>(new_body);
    gqt->delete_RefEntity(this_blank);
    gqt->delete_RefEntity(this_tool);
  }

  RETURN(iBase_SUCCESS);
}

ITAPS_API void
iGeom_intersectEnts( /*in*/ iBase_EntityHandle ent1,
                     /*in*/ iBase_EntityHandle ent2,
                     /*out*/ iBase_EntityHandle *geom_entity,
                     int* err )
{
  Body *this_ent1 = dynamic_cast<Body*>(ENTITY_HANDLE(ent1));
  Body *ent1_copy = gmt->copy_body(this_ent1);
  if (NULL == ent1_copy) {
    ERROR(iBase_FAILURE, "Trouble copying blank.");
  }
  Body *this_ent2 = dynamic_cast<Body*>(ENTITY_HANDLE(ent2));
  Body *ent2_copy = gmt->copy_body(this_ent2);
  if (NULL == ent2_copy) {
    ERROR(iBase_FAILURE, "Trouble copying tool.");
    gqt->delete_RefEntity(ent1_copy);
    RETURN(iBase_FAILURE);
  }

  DLIList<Body*> ent1_list, new_body_list;
  ent1_list.append(ent1_copy);
  
  RefEntity *new_body = NULL;
  CubitStatus result = gmt->intersect(ent2_copy, ent1_list, new_body_list);
  if (CUBIT_SUCCESS != result || 0 == new_body_list.size()) {
    ERROR(iBase_FAILURE, "Intersect failed.");
  }
  else {
    new_body = new_body_list.get();
    *geom_entity = reinterpret_cast<iBase_EntityHandle>(new_body);
    gqt->delete_RefEntity(this_ent2);
    gqt->delete_RefEntity(this_ent1);
  }

  RETURN(iBase_SUCCESS);
}

ITAPS_API void
iGeom_sectionEnt( /*inout*/ iBase_EntityHandle geom_entity,
                  /*in*/ double plane_normal_x,
                  /*in*/ double plane_normal_y,
                  /*in*/ double plane_normal_z,
                  /*in*/ double offset,
                  /*in*/ int reverse,
                  /*out*/ iBase_EntityHandle *geom_entity2,
                  int* err )
{
  Body *this_body = dynamic_cast<Body*>(ENTITY_HANDLE(geom_entity));
  if (NULL == this_body) {
    RefVolume *this_vol = dynamic_cast<RefVolume*>(ENTITY_HANDLE(geom_entity));
    if (NULL != this_vol)
      this_body = this_vol->get_body_ptr();
  }
  if (NULL == this_body) {
    ERROR(iBase_INVALID_ARGUMENT, "Can only section bodies.");
  }

  CubitVector normal(plane_normal_x, plane_normal_y, plane_normal_z);
  if (normal.length_squared() == 0.0) {
    ERROR(iBase_INVALID_ARGUMENT, "Zero-length vector input.");
  }
  
  CubitVector point1 = normal * CubitVector(1.0, 0.0, 0.0);
  if (point1.length_squared() == 0.0)
    point1 = normal * CubitVector(0.0, 1.0, 0.0);
    
  CubitVector point2 = normal * point1;
  CubitVector point3(0.0, 0.0, 0.0);

  if (0.0 != offset) {
    normal.normalize();
    normal *= offset;
    point1 += normal;
    point2 += normal;
    point3 += normal;
  }

  
  DLIList<Body*> blank_list, new_body_list;
  blank_list.append(gmt->copy_body(this_body));
  CubitStatus result = gmt->section(blank_list, point1, point2, point3,
                                    new_body_list, !reverse,
                                    false);
  if (CUBIT_SUCCESS != result || 0 == new_body_list.size()) {
    gqt->delete_RefEntity(blank_list.get());
    ERROR(iBase_FAILURE, "Section failed.");
  }
    
  else {
      // need to assign it to a RE* first so the void cast gets done right
    RefEntity *new_body = new_body_list.get();
    *geom_entity2 = reinterpret_cast<iBase_EntityHandle>(new_body);
      // also, delete the original body, now that the section worked
    gqt->delete_RefEntity(this_body);
  }

  RETURN(iBase_SUCCESS);
}

ITAPS_API void
iGeom_imprintEnts( /*in*/ iBase_EntityHandle const* gentity_handles,
                   int gentity_handles_size,
                   int* err )
{
  if (gentity_handles_size < 1) // GMT::imprint segfaults if passed an empty list
    RETURN(iBase_SUCCESS);

  DLIList<Body*> bods;
  DLIList<RefVolume*> vols, temp_vols;
  RefEntity* const* handle_array = ENTITY_HANDLE_CONST_ARRAY(gentity_handles);
  CubitStatus status = CUBIT_SUCCESS;
  for (int i = 0; i < gentity_handles_size; i++) {
    Body *temp_bod = dynamic_cast<Body*>(handle_array[i]);
    if (NULL != temp_bod) {
      bods.append_unique(temp_bod);
      continue;
    }
    
    RefVolume *temp_vol = dynamic_cast<RefVolume*>(handle_array[i]);
    if (NULL != temp_vol) {
      TopologyEntity *topo_ent = dynamic_cast<TopologyEntity*>(handle_array[i]);
      if (NULL == topo_ent) {
        status = CUBIT_FAILURE;
        continue;
      }
      temp_bod = topo_ent->body();
      if (NULL == temp_bod) {
        status = CUBIT_FAILURE;
        continue;
      }
      bods.append_unique(temp_bod);
      continue;
    }

      // if we've gotten here, it's an error
    status = CUBIT_FAILURE;
  }
  
  if (CUBIT_SUCCESS != status) RETURN(iBase_FAILURE);

  DLIList<Body*> temp_bods;
  status = GeometryModifyTool::instance()->imprint(bods, temp_bods, false);
  
  RETURN(iBase_SUCCESS);
}
  
ITAPS_API void
iGeom_mergeEnts( /*in*/ iBase_EntityHandle const* gentity_handles,
                 int gentity_handles_size,
                 double tolerance,
                 int* err )
{
  double old_factor = GeometryQueryTool::instance()->get_geometry_factor();
  if (tolerance != old_factor) 
    GeometryQueryTool::instance()->set_geometry_factor(tolerance*1.0e6);
  else old_factor = 0.0;
  
  DLIList<Body*> bods;
  DLIList<RefVolume*> vols, temp_vols;
  DLIList<RefFace*> faces;
  DLIList<RefEdge*> edges;
  DLIList<RefVertex*> verts;
  RefEntity* const* handle_array = ENTITY_HANDLE_CONST_ARRAY(gentity_handles);
  for (int i = 0; i < gentity_handles_size; i++) {
    TopologyEntity *topo_ent = dynamic_cast<TopologyEntity*>(handle_array[i]);
    if (NULL == topo_ent) continue;
    Body *temp_bod;
    RefVolume *temp_vol;
    RefFace *temp_face;
    RefEdge *temp_edge;
    RefVertex *temp_vert;
    switch (handle_array[i]->dimension()) {
      case -1:
          // it should be a body
        temp_bod = dynamic_cast<Body*>(handle_array[i]);
        if (NULL == temp_bod) RETURN(iBase_FAILURE);
        temp_vols.clean_out();
        topo_ent->ref_volumes(temp_vols);
        vols += temp_vols;
        break;
      case 0:
        temp_vert = dynamic_cast<RefVertex*>(handle_array[i]);
        if (NULL == temp_vert) RETURN(iBase_FAILURE);
        verts.append(temp_vert);
        break;
      case 1:
        temp_edge = dynamic_cast<RefEdge*>(handle_array[i]);
        if (NULL == temp_edge) RETURN(iBase_FAILURE);
        edges.append(temp_edge);
        break;
      case 2:
        temp_face = dynamic_cast<RefFace*>(handle_array[i]);
        if (NULL == temp_face) RETURN(iBase_FAILURE);
        faces.append(temp_face);
        break;
      case 3:
        temp_vol = dynamic_cast<RefVolume*>(handle_array[i]);
        if (NULL == temp_vol) RETURN(iBase_FAILURE);
        vols.append(temp_vol);
        break;
    }
  }
  
  CubitStatus status = CUBIT_SUCCESS, temp_status;
    
  if (verts.size() != 0) {
    temp_status = MergeTool::instance()->merge_refvertices(verts, false);
    if (CUBIT_SUCCESS != temp_status) status = temp_status;
  }
    
  if (edges.size() != 0) {
    temp_status = MergeTool::instance()->merge_refedges(edges, true, false);
    if (CUBIT_SUCCESS != temp_status) status = temp_status;
  }
    
  if (faces.size() != 0) {
    temp_status = MergeTool::instance()->merge_reffaces(faces, false);
    if (CUBIT_SUCCESS != temp_status) status = temp_status;
  }
    
  if (vols.size() != 0) {
    temp_status = MergeTool::instance()->merge_volumes(vols, false);
    if (CUBIT_SUCCESS != temp_status) status = temp_status;
  }
    
  if (bods.size() != 0) {
    temp_status = MergeTool::instance()->merge_bodies(bods);
    if (CUBIT_SUCCESS != temp_status) status = temp_status;
  }

  if (0 != old_factor)
    GeometryQueryTool::instance()->set_geometry_factor(old_factor);
    
  if (CUBIT_SUCCESS != status) {
    RETURN(iBase_FAILURE);
  }
  
  else {
    RETURN(iBase_SUCCESS);
  }
}

} //extern "C"

/********************* HELPER FUNCTION IMPLEMENTATIONS ***************************/

static void tokenize( const std::string& str, 
                      std::vector<std::string>& tokens )
{
  char delim =  str[0];
  std::string::size_type last = str.find_first_not_of( delim, 1 );
  std::string::size_type pos  = str.find_first_of( delim, last );
  while (std::string::npos != pos && std::string::npos != last) {
    tokens.push_back( str.substr( last, pos - last ) );
    last = str.find_first_not_of( delim, pos );
    pos  = str.find_first_of( delim, last ); 
  }
}

// Expect option of the form "NAME=VALUE".
// If NAME portion matches, pass back VALUE and return true.
// Otherwise, leave 'value' unchanged and return false.
static bool match_option( const std::string& opt,
                          const char* name,
                          std::string& value )
{
  std::string::size_type len = strlen( name );
  if (opt[len] != '=')
    return false;
  if (opt.compare( 0, len, name, len ))
    return false;
  value = opt.substr( len + 1 );
  return true;
}

static CubitStatus
iGeom_bounding_box( RefEntity* entity, CubitVector& minc, CubitVector& maxc )
{
  CubitBox box;
  if (BasicTopologyEntity* bte = dynamic_cast<BasicTopologyEntity*>(entity))
    box = bte->bounding_box();
  else if(Body* body = dynamic_cast<Body*>(entity))
    box = body->bounding_box();
  else {
    CGM_iGeom_setLastError(iBase_INVALID_ENTITY_HANDLE, "Entities passed into gentityBoundingBox must be vertex, edge, face, or region."); 
    return CUBIT_FAILURE;
  }
  
  minc = box.minimum();
  maxc = box.maximum();
  return CUBIT_SUCCESS;
}

/** We could use Smits' algorithm here, but only if we turned of
    floating-point exceptions */
static inline void box_min_max( double dir,
                                double min,
                                double max,
                                double pt,
                                double& tmin,
                                double& tmax )
{
  if (dir == 0) {
    tmin = -INFINITY;
    tmax = INFINITY;
  }
  else if (dir > 0.0) {
    tmin = (min - pt) / dir;
    tmax = (max - pt) / dir;
  }
  else {
    tmin = (max - pt) / dir;
    tmax = (min - pt) / dir;
  }
}



template <typename T> static inline 
int count_type( const DLIList<CubitEntity*>& list )
{
  int count = 0, size = list.size();
  for (int i = 0; i < size; ++i)
    if (dynamic_cast<T*>(list[i]))
      ++count;
  return count;
}

static inline
int count_ibase_type( int ibase_type, const DLIList<CubitEntity*>& list, int* err )
{
  *err = iBase_SUCCESS;
  switch (ibase_type) {
    case iBase_ALL_TYPES: return list.size() - count_type<RefGroup>(list);
    case iBase_REGION:    return count_type<Body>(list);
    case iBase_FACE:      return count_type<RefFace>(list);
    case iBase_EDGE:      return count_type<RefEdge>(list);
    case iBase_VERTEX:    return count_type<RefVertex>(list);
    default:
      *err = iBase_INVALID_ENTITY_TYPE;
      CGM_iGeom_setLastError( *err );
      return -1;
  }
}

template <typename TARGET_TYPE, typename LIST_TYPE> static inline 
void append_type( const DLIList<CubitEntity*>& source_list,
                  DLIList<LIST_TYPE*>& target_list )
{
  int size = source_list.size();
  for (int i = 0; i < size; ++i) 
    if (TARGET_TYPE* ent = dynamic_cast<TARGET_TYPE*>(source_list[i]))
      target_list.append(ent);
}

template <typename SKIP_TYPE, typename LIST_TYPE> static inline 
void append_not_type( const DLIList<CubitEntity*>& source_list,
                      DLIList<LIST_TYPE*>& target_list )
{
  int size = source_list.size();
  for (int i = 0; i < size; ++i) 
    if (!dynamic_cast<SKIP_TYPE*>(source_list[i]))
      if (RefEntity* ent = dynamic_cast<LIST_TYPE*>(source_list[i]))
        target_list.append(ent);
}

// Returns count of entities appended.
template <typename TARGET_TYPE> static inline 
int append_type( const DLIList<CubitEntity*>& source_list,
                 iBase_EntityHandle* array, int array_size )
{
  RefEntity* re_ptr;
  int len = source_list.size();
  int count = 0;
  for (int i = 0; i < len; ++i) {
    if (TARGET_TYPE* ent = dynamic_cast<TARGET_TYPE*>(source_list[i])) {
      if (count < array_size)
        array[count] = reinterpret_cast<iBase_EntityHandle>(re_ptr = ent);
      ++count;
    }
  }
  return count;
}

template <typename SKIP_TYPE> static inline 
int append_not_type( const DLIList<CubitEntity*>& source_list,
                     iBase_EntityHandle* array, int array_size )
{
  int len = source_list.size();
  int count = 0;
  for (int i = 0; i < len; ++i) {
    if (!dynamic_cast<SKIP_TYPE*>(source_list[i])) {
      if (count == array_size) 
        return -1;
      else if (RefEntity* ent = dynamic_cast<RefEntity*>(source_list[i]))
        array[count++] = reinterpret_cast<iBase_EntityHandle>(ent);
    }
  }
  return count;
}

static 
void copy_ibase_type( int ibase_type, 
                      const DLIList<CubitEntity*>& list,
                      iBase_EntityHandle** entity_handles,
                      int* entity_handles_alloc,
                      int* entity_handles_size,
                      int* err )
{
  int count;
  if (*entity_handles_alloc == 0) {
    count = count_ibase_type( ibase_type, list, err );
    if (count < 0)
      return;
    *entity_handles = (iBase_EntityHandle*)malloc( count * sizeof(iBase_EntityHandle) );
    if (!*entity_handles) 
      RETURN(iBase_MEMORY_ALLOCATION_FAILED);
    *entity_handles_alloc = count;
  }
  
  switch (ibase_type) {
    case iBase_ALL_TYPES:
      count = append_not_type<RefGroup>(list,*entity_handles, *entity_handles_alloc);
      break;
    case iBase_REGION:
      count = append_type<Body>(list,*entity_handles, *entity_handles_alloc);
      break;
    case iBase_FACE:
      count = append_type<RefFace>(list,*entity_handles, *entity_handles_alloc);
      break;
    case iBase_EDGE:
      count = append_type<RefEdge>(list,*entity_handles, *entity_handles_alloc);
      break;
    case iBase_VERTEX:
      count = append_type<RefVertex>(list,*entity_handles, *entity_handles_alloc);
      break;
    default:
      RETURN(iBase_INVALID_ENTITY_TYPE);
      break;
  }
  
  *entity_handles_size = count;
  if (count > *entity_handles_alloc)
    RETURN(iBase_BAD_ARRAY_DIMENSION);
  
  RETURN(iBase_SUCCESS);
}


static 
void append_all_ibase_type( int ibase_type, 
                            DLIList<RefEntity*>& target_list,
                            int* err )
{
  RefEntityFactory *const ref = RefEntityFactory::instance();
  if (ibase_type == iBase_ALL_TYPES) {
    for (int i = 0; i < 4; ++i) {
      DLIList<RefEntity*> tmp;
      ref->ref_entity_list( iGeom_entity_type_names[i], tmp );
      target_list += tmp;
    }
  }
  else if (abs(ibase_type) < iBase_ALL_TYPES) {
    ref->ref_entity_list( iGeom_entity_type_names[ibase_type], target_list );
  }
  else {
    RETURN(iBase_INVALID_ENTITY_TYPE);
  }
  
  RETURN(iBase_SUCCESS);
}

