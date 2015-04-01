/////////////////////////////////////////////////////////////
// (c) 2002 Remotion		
/////////////////////////////////////////////////////////////

// Starts the plugin registration

#include "c4d.h"

// forward declarations
Bool RegisterASEio(void);
Bool RegisterT3Dio(void);
Bool RegisterIDTagTag(void);

Bool PluginStart(void)
{

	LONG err = 0;
	if (!RegisterASEio()) err++;
	if (!RegisterT3Dio()) err++;
	if (!RegisterIDTagTag()) err++;

	if(err==0){
		GePrint("// Remotion 2003-2008: ASE,T3D Export");
		GeDebugOut(" ASE,T3D Export registred! ");
	}else{
		GeDebugOut(" ASE,T3D Export ERROR %i",err);
	}

	return TRUE;
}

void PluginEnd(void)
{
}

Bool PluginMessage(LONG id, void *data)
{
	//use the following lines to set a plugin priority
	//
	switch (id)
	{
		case C4DPL_INIT_SYS:
			if (!resource.Init()) return FALSE; // don't start plugin without resource
			return TRUE;

		case C4DMSG_PRIORITY: 
			return TRUE;
	}

	return FALSE;
}
