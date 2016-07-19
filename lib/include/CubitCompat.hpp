#ifndef CUBIT_COMPAT_HPP
#define CUBIT_COMPAT_HPP


CubitStatus 
CubitCompat_export_solid_model( DLIList<RefEntity*>& ref_entity_list,
                                const char* filename,
                                const char * filetype,
                                int &num_ents_exported,
                                const CubitString &cubit_version,
                                const char* logfile_name = NULL );
#endif /* CUBIT_COMPAT_HPP */

