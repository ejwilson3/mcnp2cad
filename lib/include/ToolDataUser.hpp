//- Class: ToolDataUser
//- Owner: Scott Mitchell
//- Description: This is the tool data interface that every class that uses a
//-              tool data needs to specify. E.g. CubitNode and CubitFace
//-              are derived from this class.
//- Checked By: 
//- Version: $Id: 

#ifndef TOOL_DATA_USER_HPP
#define TOOL_DATA_USER_HPP

#ifdef WIN32
class __single_inheritance ToolData;
#else
class ToolData;
#endif

class CUBIT_UTIL_EXPORT ToolDataUser{};


#endif // TOOL_DATA_USER_HPP





