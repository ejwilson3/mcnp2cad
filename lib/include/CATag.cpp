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
 
/**\file CATag.cpp
 *
 *\author Tim Tautges
 *\author Jason Kraftcheck
 *
 * Original file from SNL TSTT repository was named CATag.
 *
 * Renamed CATag and added to ANL ITAPS repository by J.Kraftcheck,
 * 2007-6-15
 */

#include "CATag.hpp"
#include "RefEntityName.hpp"
#include "RefEntityFactory.hpp"
#include "RefGroup.hpp"
#include "iGeomError.h"

#define CHECK_SIZE(array, type, this_size, retval)  \
  if (0 == array ## _allocated || array ## _allocated < this_size) {\
    if (NULL != array) free(array); \
    array = (type*)malloc(this_size*sizeof(type));\
    array ## _allocated=this_size;\
    if (NULL == array) { \
      CGM_iGeom_setLastError(iBase_MEMORY_ALLOCATION_FAILED);\
      return retval; \
    }\
  }; \
  array ## _size = this_size;


#define RETURN(a) {CGM_iGeom_setLastError(a); return a;}

const char *CGMTagManager::CATag_NAME = "ITAPS_Tag";
const char *CGMTagManager::CATag_NAME_INTERNAL = "ITAPS_TAG";

CubitAttrib *CGMTagManager::CATag_creator(RefEntity* entity, CubitSimpleAttrib *p_csa)
{
  CATag *this_ca = new CATag(&instance(), entity, p_csa);
  return this_ca;
}
 
static CGMTagManager::TagInfo preset_tag_list[] = {
   // tag size      tag name           tag data type  default active
 { 0,              "",                 iBase_BYTES,   NULL,  false },
 { 32,             "NAME",             iBase_BYTES,   NULL,   true },
 { sizeof(int),    "GLOBAL_ID",        iBase_INTEGER, NULL,   true },
 { sizeof(int),    "UNIQUE_ID",        iBase_INTEGER, NULL,   true },
 { sizeof(int),    "MESH_INTERVAL",    iBase_INTEGER, NULL,   true },
 { sizeof(double), "MESH_SIZE",        iBase_DOUBLE,  NULL,   true },
 { 4,              "SIZE_FIRMNESS",    iBase_BYTES,   NULL,   true } };
 

CGMTagManager::TagInfo* const CGMTagManager::presetTagInfo = preset_tag_list;
const int CGMTagManager::numPresetTag = sizeof(preset_tag_list)/sizeof(preset_tag_list[0]);

CGMTagManager::CGMTagManager() 
    : interfaceGroup(NULL)
{
  pcTag = 0;
  tagInfo.push_back(preset_tag_list[0]);
  
    // get the tag number for CATag
  DLIList<int> tag_types;
  int max_type = 0;
  CubitAttribManager *cam = CGMApp::instance()->attrib_manager();
  cam->get_registered_types(tag_types);
  for (int i = 0; i < tag_types.size(); i++) {
    int this_type = tag_types.get_and_step();
    max_type = (max_type < this_type ? this_type : max_type);
  }
  
  max_type++;
  CubitStatus status = cam->register_attrib_type(max_type, CATag_NAME, CATag_NAME_INTERNAL,
                                                 &CGMTagManager::CATag_creator, false, true, 
                                                 true, true, true, false);
  if (CUBIT_FAILURE == status) {
    CGM_iGeom_setLastError( iBase_FAILURE, "Couldn't create cgm attribute for tags." );
  }
  else
    CATag_att_type = max_type;

    // create preset tags, for CGM attributes we want to be visible as tags
    // name - make same as in MBTagConventions
  for (int i = 1; i < numPresetTag; ++i)
    tagNameMap[presetTagInfo[i].tagName] = -i; // neg handles beginning with -1
}


CGMTagManager::~CGMTagManager() 
{

}

iBase_ErrorType CGMTagManager::createTag (/*in*/ const char *tag_name,
                                          /*in*/ const int tag_length,
                                          /*in*/ const int tag_type,
                                          /*in*/ char* default_value,
                                          /*out*/ long *tag_handle)
{
  std::string tmp_name(tag_name);
  TagInfo tmp_info = {tag_length, tmp_name, tag_type, NULL, true};

  std::map<std::string,long>::iterator mit = tagNameMap.find(tmp_name);
  if (mit != tagNameMap.end()) {
    // we found a tag with this name; is it still active?
    bool active = (mit->second > 0 ? tagInfo[mit->second] :
                   presetTagInfo[-mit->second]).isActive;
    *tag_handle = mit->second;
    if (active) {
      CGM_iGeom_setLastError( iBase_TAG_ALREADY_EXISTS );
      return iBase_TAG_ALREADY_EXISTS;
    }

    tagInfo[*tag_handle] = tmp_info;
  }
  else {
    // create a new tag entirely
    tagInfo.push_back(tmp_info);
    *tag_handle = tagInfo.size() - 1;

    // put the name and handle into the map too
    tagNameMap[std::string(tag_name)] = *tag_handle;
  }

  if (default_value != NULL) {
    tagInfo[*tag_handle].defaultValue = (char *) malloc(tag_length);
    memcpy(tagInfo[*tag_handle].defaultValue, default_value, tag_length);
  }

  RETURN(iBase_SUCCESS);
}

int CGMTagManager::getTagSize (/*in*/ const long tag_handle)
{
  CGM_iGeom_clearLastError();
  return (tag_handle > 0 ? 
          tagInfo[tag_handle].tagLength : 
          presetTagInfo[-tag_handle].tagLength);
}

long CGMTagManager::getTagHandle (/*in*/ const char *tag_name)
{
  std::map<std::string,long>::iterator it =
    tagNameMap.find(std::string(tag_name));
  if (it != tagNameMap.end()) {
    bool active = (it->second > 0 ? tagInfo[it->second] :
                   presetTagInfo[-it->second]).isActive;
    if (active) {
      CGM_iGeom_clearLastError();
      return it->second;
    }
  }

  CGM_iGeom_setLastError( iBase_TAG_NOT_FOUND );
  return 0;
}

iBase_ErrorType CGMTagManager::setArrData (/*in*/ ARRAY_IN_DECL(RefEntity*, entity_handles),
                                           /*in*/ const long tag_handle,
                                           /*in*/ const char *tag_values/*, const int tag_values_size*/)
{
  TagInfo *tinfo = (tag_handle > 0 ? &tagInfo[tag_handle] : &presetTagInfo[-tag_handle]);
  int tag_size = tinfo->tagLength;
  
  const char *val_ptr = tag_values;

  iBase_ErrorType result = iBase_SUCCESS, tmp_result;
  
  if (tag_handle < 0) {
    for (int i = 0; i < entity_handles_size; i++) {
      if (NULL == entity_handles[i])
        tmp_result = setPresetTagData(interface_group(), tag_handle, val_ptr, tag_size);
      else
        tmp_result = setPresetTagData(entity_handles[i], tag_handle, val_ptr, tag_size);

      val_ptr += tag_size;
      if (iBase_SUCCESS != tmp_result) result = tmp_result;
    }
    RETURN(result);
  }

  for (int i = 0; i < entity_handles_size; i++) {
    RefEntity *this_ent = (NULL == entity_handles[i] ? interface_group() : 
                           entity_handles[i]);
    CATag *catag = get_catag(this_ent, true);
    assert(NULL != catag);
    catag->set_tag_data(tag_handle, val_ptr);
    val_ptr += tag_size;
  }

  RETURN(iBase_SUCCESS);
}


CATag *CGMTagManager::get_catag(RefEntity *ent, 
                                  const bool create_if_missing) 
{
  CubitAttrib *this_attrib = ent->get_cubit_attrib(CATag_att_type, create_if_missing);
  if (NULL != this_attrib)
    return dynamic_cast<CATag*>(this_attrib);
  else
    return NULL;
}

RefGroup *CGMTagManager::interface_group(const bool create_if_missing) 
{
  if (NULL == interfaceGroup) 
    interfaceGroup = 
      dynamic_cast<RefGroup*>(RefEntityName::instance()->get_refentity("interface_group"));
  
  if (NULL == interfaceGroup && create_if_missing)
    interfaceGroup = RefEntityFactory::instance()->construct_RefGroup("interface_group");

  return interfaceGroup;
}
 
iBase_ErrorType CGMTagManager::setPresetTagData(RefEntity *entity, 
                                                const long tag_handle, 
                                                const char *tag_value, 
                                                const int tag_size) 
{
  switch (-tag_handle) {
    case 1:
        // entity name
      if (presetTagInfo[-tag_handle].tagLength != tag_size) {
        std::string tmp_str = "Tag of type '";
        tmp_str += presetTagInfo[-tag_handle].tagName + "' is the wrong size.";
        CGM_iGeom_setLastError(iBase_INVALID_ARGUMENT, tmp_str.c_str());
        return iBase_INVALID_ARGUMENT;
      }
      entity->entity_name(CubitString(tag_value));
      RETURN(iBase_SUCCESS);
    case 2:
        // entity id
      CGM_iGeom_setLastError( iBase_NOT_SUPPORTED, "Can't set id of entities with this implementation." );
      return iBase_NOT_SUPPORTED;
    case 3:
        // unique id
      CGM_iGeom_setLastError( iBase_NOT_SUPPORTED, "Can't set unique id of entities with this implementation." );
      return iBase_NOT_SUPPORTED;
    case 4: // mesh interval
    case 5: // mesh size
    case 6: // mesh interval firmness
    default:
      CGM_iGeom_setLastError( iBase_NOT_SUPPORTED, "Can't set this tag on entities with this implementation." );
      return iBase_NOT_SUPPORTED;
  }

  CGM_iGeom_setLastError( iBase_TAG_NOT_FOUND );
  return iBase_TAG_NOT_FOUND;
}

CATag::~CATag() 
{
  for (std::map<int, void*>::iterator 
         mit = tagData.begin(); mit != tagData.end(); mit++)
    if (NULL != (*mit).second) free ((*mit).second);
}

CATag::CATag(CGMTagManager *manager, RefEntity *owner, CubitSimpleAttrib *csa_ptr) 
    : CubitAttrib(owner), myManager(manager)
{
  if (NULL != csa_ptr) add_csa_data(csa_ptr);
}

CubitStatus CATag::reset()
{
  for (std::map<int, void*>::iterator 
         mit = tagData.begin(); mit != tagData.end(); mit++)
    if (NULL != (*mit).second) free ((*mit).second);

  tagData.clear();

  return CUBIT_SUCCESS;
}

CubitSimpleAttrib* CATag::cubit_simple_attrib() 
{
    //if (tagData.size() == 0) return NULL;
  
  DLIList<int> int_data;
  DLIList<CubitString*> str_data;
  DLIList<double> dbl_data;

  str_data.append(new CubitString(myManager->CATag_NAME_INTERNAL));

    // int data first gets the # tags on this entity
  int_data.append(tagData.size());

    // for each tag:
  for (std::map<int, void*>::iterator 
         mit = tagData.begin(); mit != tagData.end(); mit++) {
    long tag_handle = (*mit).first;
    CGMTagManager::TagInfo *tinfo = (tag_handle > 0 ? 
                                     &(myManager->tagInfo[tag_handle]) :
                                     &(myManager->presetTagInfo[-tag_handle]));

      // store the name
    str_data.append(new CubitString(tinfo->tagName.c_str()));
    
      // store the length in bytes
    int_data.append(tinfo->tagLength);
    
      // now the data
      // store the raw memory interpreted as an array of ints, padded to a full int
    int tag_ints = tinfo->tagLength/4;
    if (tinfo->tagLength % 4 != 0) tag_ints++;
    
    int *tag_data = reinterpret_cast<int*>((*mit).second);
    for (int i = 0; i < tag_ints; i++)
      int_data.append(tag_data[i]);
  }

    // store the data on the csa
  CubitSimpleAttrib *csa = new CubitSimpleAttrib(&str_data, &dbl_data, &int_data);

  for (int i = 0; i < str_data.size(); i++)
    delete str_data.get_and_step();
  
  return csa;
}

void CATag::add_csa_data(CubitSimpleAttrib *csa_ptr) 
{
    // make sure it's a CATag
  static CubitString my_type("CA_TAG");
  if (csa_ptr->character_type() != my_type) 
    return;

  csa_ptr->int_data_list()->reset();
  csa_ptr->string_data_list()->reset();
  int num_attribs = *(csa_ptr->int_data_list()->get_and_step());

  int *tmp_data;
  
  for (int i = 0; i < num_attribs; i++) {

      // map the attrib name to a tag
    std::map<std::string,long>::iterator pos =
      myManager->tagNameMap.find(std::string(csa_ptr->string_data_list()->get()->c_str()));

    long thandle = 0;
    
    if (pos == myManager->tagNameMap.end()) {
        // tag doesn't exist - create one
      myManager->createTag(csa_ptr->string_data_list()->get()->c_str(),
                           *(csa_ptr->int_data_list()->get()), iBase_BYTES,
                           NULL, &thandle);
    }
    else thandle = (*pos).second;

    
    long tag_handle = thandle;

      // copy the ints to a temporary space we can get a ptr to...
    int int_length = *(csa_ptr->int_data_list()->get_and_step());
    if (int_length % 4 != 0) int_length++;
    tmp_data = (int*) malloc(int_length*sizeof(int));
    for (int j = 0; j < int_length; j++) 
      tmp_data[j] = *(csa_ptr->int_data_list()->get_and_step());

      // now actually set the data
    this->set_tag_data(tag_handle, tmp_data, true);
  }
}

void CATag::print() 
{
  std::cout << "This entity has " << tagData.size() << " tags.  Types are: " << std::endl;
  for (std::map<int,void*>::iterator mit = tagData.begin(); mit != tagData.end(); mit++) 
  {
    if ((*mit).first > 0)
      std::cout << myManager->tagInfo[(*mit).first].tagName << std::endl;
    else
      std::cout << myManager->presetTagInfo[-(*mit).first].tagName << std::endl;
  }
}
  
iBase_ErrorType CATag::set_tag_data(long tag_handle, const void *tag_data, 
                                     const bool can_shallow_copy)
{
  CGMTagManager::TagInfo *tinfo = (tag_handle > 0 ? 
                                   &(myManager->tagInfo[tag_handle]) : 
                                   &(myManager->presetTagInfo[-tag_handle]));
  
    // check if this attribute has this tag
  std::map<int, void*>::iterator tdpos = tagData.find(tag_handle);
  if (tdpos == tagData.end())
    tdpos = tagData.insert(tagData.end(),
                           std::pair<int,void*>(tag_handle, NULL));
    
  if (!can_shallow_copy) {
      // need to copy the data; might need to allocate first
    if ((*tdpos).second == NULL)
      (*tdpos).second = malloc(tinfo->tagLength);

    memcpy((*tdpos).second, tag_data, tinfo->tagLength);
  }
  else {
      // should shallow copy; might have to delete what's there already
    if ((*tdpos).second != NULL) free((*tdpos).second);
  
      // if shallow copying, caller is saying we can copy, so cast away const
    (*tdpos).second = const_cast<void*>(tag_data);
  }

  RETURN(iBase_SUCCESS);
}

CubitStatus CATag::update() 
{
  if (tagData.empty())
    this->delete_attrib(true);

  return CUBIT_SUCCESS;
}
