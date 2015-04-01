#ifndef __MYPRINT_H
#define __MYPRINT_H
//Remotion 2003

#include "c4d_general.h"
#include "c4d_baseobject.h"
#include "c4d_graphview_enum.h"
#include "c4d_raytrace.h"


//""""""""""""""""""""""""""""""""""""""
inline Vector vnorm(const Vector v)	//VNORM
{
	//return v / Len(v);
	return v / Sqrt(v.x*v.x+v.y*v.y+v.z*v.z);
}

//""""""""""""""""  PRINT String  """"""""""""""""""""""
inline void print(String t)
{
  GePrint(t);
}

/*
//""""""""""""""""  PRINT Bool  """"""""""""""""""""""
inline void print(Bool t)
{
  if (t) GePrint("TRUE");
  else GePrint("FALSE");
}
*/


//""""""""""""""""  PRINT BaseObject  """"""""""""""""""""""
inline void print(BaseObject *t)
{
	if (t) GePrint(t->GetName());
}

//""""""""""""""""  PRINT REAL  """"""""""""""""""""""
inline void print(Real t)
{
  GePrint(RealToString(t,-1,12));
}

//""""""""""""""""  PRINT LONG  """"""""""""""""""""""
inline void print(LONG t)
{
  GePrint(LongToString(t));
}

//""""""""""""""""  PRINT S LONG  """"""""""""""""""""""
inline void print(String s, LONG t)
{
  GePrint(s+LongToString(t));
}

//""""""""""""""""  PRINT S REAL  """"""""""""""""""""""
inline void print(String s, Real t)
{
  GePrint(s+RealToString(t));
}

//""""""""""""""""  PRINT S VECTOR  """"""""""""""""""""""
inline void print(String s, Vector t)
{
  GePrint(s+RealToString(t.x)+" | "+RealToString(t.y)+" | "+RealToString(t.z));
}


//""""""""""""""""  PRINT LONG  """"""""""""""""""""""
inline void print(CHAR t)
{
  GePrint(LongToString(t));
}

//""""""""""""""""  PRINT VECTOR """"""""""""""""""""""
inline void print(Vector t)
{
  GePrint(RealToString(t.x)+" | "+RealToString(t.y)+" | "+RealToString(t.z));
}


//""""""""""""""""  PRINT VECTOR """"""""""""""""""""""
inline void print(UVWStruct u)
{
  GePrint(RealToString(u.a.x)+" ~ "+RealToString(u.a.y)+" ~ "+RealToString(u.a.z));
  GePrint(RealToString(u.b.x)+" ~ "+RealToString(u.b.y)+" ~ "+RealToString(u.b.z));
  GePrint(RealToString(u.c.x)+" ~ "+RealToString(u.c.y)+" ~ "+RealToString(u.c.z));
  GePrint(RealToString(u.d.x)+" ~ "+RealToString(u.d.y)+" ~ "+RealToString(u.d.z));
}

//""""""""""""""""  PRINT VECTOR """"""""""""""""""""""
inline void print(PolyVector u)
{
  GePrint(RealToString(u.a.x)+" ~ "+RealToString(u.a.y)+" ~ "+RealToString(u.a.z));
  GePrint(RealToString(u.b.x)+" ~ "+RealToString(u.b.y)+" ~ "+RealToString(u.b.z));
  GePrint(RealToString(u.c.x)+" ~ "+RealToString(u.c.y)+" ~ "+RealToString(u.c.z));
  GePrint(RealToString(u.d.x)+" ~ "+RealToString(u.d.y)+" ~ "+RealToString(u.d.z));
}


//""""""""""""""""  PRINT VECTOR """"""""""""""""""""""
inline void print(Matrix t)
{
  GePrint(RealToString(t.off.x)+" ~ "+RealToString(t.off.y)+" ~ "+RealToString(t.off.z));
}


inline void print(GeDataType type)
{
	switch(type){
		case DA_NIL:GePrint("DA_NIL");break;
		case DA_LONG:GePrint("DA_LONG");break;
		case DA_REAL:GePrint("DA_REAL");break;
		case DA_TIME:GePrint("DA_TIME");break;
		case DA_VECTOR:GePrint("DA_VECTOR");break;
		case DA_MATRIX:GePrint("DA_MATRIX");break;
		case DA_BYTEARRAY:GePrint("DA_BYTEARRAY");break;
		case DA_STRING:GePrint("DA_STRING");break;
		case DA_FILENAME:GePrint("DA_FILENAME");break;
		case DA_CONTAINER:GePrint("DA_CONTAINER");break;
		case DA_ALIASLINK:GePrint("DA_ALIASLINK");break;
		case DA_MARKER:GePrint("DA_MARKER");break;
		case DA_MISSINGPLUG:GePrint("DA_MISSINGPLUG");break;
		//case :GePrint("");break;
		default: GePrint("Unknown "+LongToString(type));
	}
}

inline void print(GvError type)
{
	switch(type){
		//case GV_IO_ERR_NONE:GePrint("GV_IO_ERR_NONE");break;
		case GV_CALC_ERR_NONE:GePrint("GV_CALC_ERR_NONE");break;
		case GV_CALC_ERR_NO_MEMORY:GePrint("GV_CALC_ERR_NO_MEMORY");break;
		case GV_CALC_ERR_UNDEFINED:GePrint("GV_CALC_ERR_UNDEFINED");break;
		case GV_CALC_ERR_NOT_IMPLEMENTED:GePrint("GV_CALC_ERR_NOT_IMPLEMENTED");break;
		case GV_CALC_ERR_NOT_INITIALIZED:GePrint("GV_CALC_ERR_NOT_INITIALIZED");break;
		case GV_CALC_ERR_NOT_VALID:GePrint("GV_CALC_ERR_NOT_VALID");break;
		case GV_CALC_ERR_NO_OUTPORT:GePrint("GV_CALC_ERR_NO_OUTPORT");break;
		case GV_CALC_ERR_NO_PORT:GePrint("GV_CALC_ERR_NO_PORT");break;
		case GV_CALC_ERR_DIVISION_BY_ZERO:GePrint("GV_CALC_ERR_DIVISION_BY_ZERO");break;
		case GV_CALC_ERR_TYPE_MISMATCH:GePrint("GV_CALC_ERR_TYPE_MISMATCH");break;
		case GV_CALC_ERR_INIT_FAILED:GePrint("GV_CALC_ERR_INIT_FAILED");break;
		case GV_CALC_ERR_QUERY_FAILED:GePrint("GV_CALC_ERR_QUERY_FAILED");break;
		case GV_CALC_ERR_CALCULATION_FAILED:GePrint("GV_CALC_ERR_CALCULATION_FAILED");break;
		case GV_CALC_ERR_PARAMETER:GePrint("GV_CALC_ERR_PARAMETER");break;
		case GV_IO_ERR_NO_MEMORY:GePrint("GV_IO_ERR_NO_MEMORY");break;
		case GV_IO_ERR_FILE_NOT_FOUND:GePrint("GV_IO_ERR_FILE_NOT_FOUND");break;
		case GV_IO_ERR_READ:GePrint("GV_IO_ERR_READ");break;
		case GV_IO_ERR_WRITE:GePrint("GV_IO_ERR_WRITE");break;
		case GV_IO_ERR_NOT_A_GROUPNODE:GePrint("GV_IO_ERR_NOT_A_GROUPNODE");break;
		case GV_NR_OF_SYSTEM_ERRORS:GePrint("GV_NR_OF_SYSTEM_ERRORS");break;
		case GV_CALC_ERR_USER_DEFINED:GePrint("GV_CALC_ERR_USER_DEFINED");break;

		//case :GePrint("");break;
		default: GePrint("Unknown "+LongToString(type));
	}
}


#endif//end