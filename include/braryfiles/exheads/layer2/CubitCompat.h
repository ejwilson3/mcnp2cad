/* geom/CubitCompat.h.  Generated from CubitCompat.h.in by configure.  */
#ifndef CUBIT_COMPAT_H
#define CUBIT_COMPAT_H

#ifdef __cplusplus

#include "CubitString.hpp"

struct ModelImportOptions
{
  //Acis options
  unsigned short heal_step : 1;
  unsigned short print_results : 1;
  unsigned short import_bodies : 1;
  unsigned short import_surfaces : 1;
  unsigned short import_curves : 1;
  unsigned short import_vertices : 1;
  unsigned short free_surfaces : 1;
  unsigned short merge_globally : 1;

  //Granite option
  unsigned short no_assembly_level_features : 1;

  //Granite, Acis option
  CubitString logfile_name;
};
#endif

#endif /* CUBIT_COMPAT_H */
