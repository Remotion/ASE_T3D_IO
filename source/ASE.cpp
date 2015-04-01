//////////////////////////////////////////////////////////////
// REMOTION ASE : FILTER PLUGINS							//
// ASE loader and saver 								//
/////////////////////////////////////////////////////////////
// VERSION    : CINEMA 4D R8								//
/////////////////////////////////////////////////////////////
// (c) 2002 Remotion										//	
/////////////////////////////////////////////////////////////
// HELP -> http://www.solosnake.fsnet.co.uk/main/ase.htm#meshface	//
// HELP -> http://nate.scuzzy.net/docs/ase/					//
// HELP -> http://quixoft.hypermart.net/ase.txt				//
// HELP -> http://zeropage.org/moin11/wiki/moin.cgi/ase_20parse_20ex
/////////////////////////////////////////////////////////////
//	Create -> 29.10.02	 ASE saver					
//  TO DO:	UVW Corectur???
/////////////////////////////////////////////////////////////

#include "c4d_file.h"
#include "c4d_memory.h"
#include "c4d_general.h"
#include "c4d_basebitmap.h"
//#include "c4d_symbols.h"
#include "c4d_resource.h"
#include "c4d_gui.h"
#include "c4d_basedocument.h"
#include "c4d_basetag.h"
#include "c4d_filterplugin.h"
#include "c4d_baseselect.h"
#include "Ttexture.h"

#include "myprint.h"
#include "aseexport.h"

#define ASE_LOADER_ID	1011244
#define ASE_SAVER_ID	1011245

//-------------------------------
#include "TIDTag.h"
#define DI_IDTAG_TAG_ID 1011248
//-------------------------------

//-----------------------------------------
struct ASEEdata{
	ASEEdata(void);
	Bool ReadSetting(BaseContainer *data);

	Bool savenormals;
	Bool reversenormals;
	Bool matObjecPrefix;
	Bool joinob;

	LONG	commadigits;
};
ASEEdata::ASEEdata(void)
{
	savenormals = FALSE;
	reversenormals = TRUE;
	matObjecPrefix = TRUE;
	joinob	= FALSE;

	commadigits = 6;
}
Bool ASEEdata::ReadSetting(BaseContainer *data)
{
	savenormals = data->GetBool(ASEE_SAVENORMALS);
	reversenormals	= data->GetBool(ASEE_REVERSENORMALS);
	matObjecPrefix	= data->GetBool(ASEE_MATOBPREFIX);
	joinob = data->GetBool(ASEE_JOINOBJECTS);

	commadigits = data->GetLong(ASEE_COMMADIGITS);
	return TRUE;
}
//-----------------------------------------

//################### T3DLoaderData #########################
class ASELoaderData : public SceneLoaderData
{
	private:

	public:
		virtual Bool Identify(PluginSceneLoader *node, const Filename &name, UCHAR *probe, LONG size);
		virtual LONG Load(PluginSceneLoader *node, const Filename &name, BaseDocument *doc, LONG filterflags, String *error, BaseThread *bt);

		Bool ReadLine(BaseFile *bf, String *v);
		Bool ReadVector(String s,String &id,Vector v);
		//String ReplaceSpaces(String s);
		//Bool ReadPolygon();

		static NodeData *Alloc(void) { return gNew ASELoaderData; }
};
//################### T3DSaverData #########################
class ASESaverData : public SceneSaverData
{
	private:	
		CHAR		*mem;
		BaseFile	*file;
		LONG CountTriangles(CPolygon *vadr, LONG vcnt, LONG &quadcnt);
		LONG CountObjects(PolygonObject *sorc, LONG cnt);
		LONG GetMatIDCount(BaseContainer  *idtagdata, BaseDocument *doc);
		Bool GetSelList(BaseContainer  *idtagdata, BaseDocument *doc, BaseSelect **sel);
		LONG GetID(LONG i);
		Bool WriteLine(const String &v);
		Bool WriteObject(BaseDocument *doc, PolygonObject *sorc, LONG &materialNumber, ASEEdata &aseedata);
		Bool WriteMaterialList(BaseDocument *doc, PolygonObject *sorc, ASEEdata &aseedata);
		Bool WriteMaterials(BaseDocument *doc, PolygonObject *sorc, LONG &materialNumber, ASEEdata &aseedata);
		Bool WriteScene(BaseDocument *doc);
	public:
		virtual Bool Init(GeListNode *node);
		virtual void Free(GeListNode *node);
		virtual LONG Save(PluginSceneSaver *node, const Filename &name, BaseDocument *doc, LONG filterflags);

		static NodeData *Alloc(void) { return gNew ASESaverData; }
};
//########### READ LINE DONT MODIFI!!!! ############
inline Bool ASELoaderData::ReadLine(BaseFile *bf, String *v)
{
	CHAR ch,line[1024];
	LONG i = 0, len = bf->TryReadBytes(&ch, 1);
	if (len == 0) return FALSE; // end of file
	while (len == 1 && ch != '\n' && ch != '\r') 
	{
		line[i++] = ch;
		len = bf->TryReadBytes(&ch, 1);
	}
#ifdef __PC //ONLY PC ?
	if (ch == '\r') 
	{
		len = bf->TryReadBytes(&ch, 1);
		if (len == 1 && ch != '\n') bf->Seek(-1);
	}
#endif
	v->SetCString(line, i);
	return TRUE;
}//Read Line

Bool ASESaverData::Init(GeListNode *node)
{
	BaseContainer	*data = ((PluginSceneSaver*)node)->GetDataInstance();
	data->SetBool(ASEE_SAVENORMALS,FALSE);
	data->SetBool(ASEE_REVERSENORMALS,TRUE);
	data->SetBool(ASEE_MATOBPREFIX,TRUE);
	data->SetBool(ASEE_JOINOBJECTS,FALSE);

	data->SetLong(ASEE_COMMADIGITS,6);

	mem = (CHAR*) GeAlloc(sizeof(CHAR)*2048);//2048 ???
	if (!mem) return FALSE;

	return TRUE;
}
void ASESaverData::Free(GeListNode *node)
{
	GeFree(mem);
}
//################## WriteLine ######################
//Critisch MAC AND WIN ????
inline Bool ASESaverData::WriteLine(const String &v)
{
	Bool ok;
	String s = v+"\r\n";//?????? Neu Linie
	LONG len  = s.GetCStringLen()+1; if (len>2048) return FALSE;//!!!!!
	//CHAR *mem = (CHAR*) GeAlloc(sizeof(CHAR)*len);//TOO SLOW ???
	//if (!mem)
	//{
		//file->SetError(FILEERROR_MEMORY);
		//return FALSE;
	//}
	s.GetCString(mem,len);
	ok = file->WriteBytes(mem,len-1);
	//GeFree(mem);
	return ok;
}//Write Line

//-----------------------------------------------------------------------------
//############################ Identify ############################
Bool ASELoaderData::Identify(PluginSceneLoader *node, const Filename &name, UCHAR *probe, LONG size)
{
	//ULONG *p=(ULONG*)probe,v1=p[0];	
	//lMotor(&v1); //MAC ?????
	//return v1==0x42466769;//"Begi" ???
	return TRUE;
}
//############################ Load ############################
LONG ASELoaderData::Load(PluginSceneLoader *node, const Filename &name, BaseDocument *doc, LONG filterflags, String *error, BaseThread *bt)
{
	BaseFile	*file = BaseFile::Alloc(); if (!file) return IMAGE_NOMEM; //???
	if (!file->Open(name,GE_READ,FILE_NODIALOG,GE_MOTOROLA)) return IMAGE_DISKERROR;
	BaseFile::Free(file);//???
	return 1;
}

//#########################################################
LONG ASESaverData::CountTriangles(CPolygon *vadr, LONG vcnt, LONG &quadcnt)//WORK
{	
	LONG tricnt = 0;
	LONG i;
	for (i=0; i<vcnt; i++)
	{	
		tricnt++;
		if (vadr[i].c!=vadr[i].d) {
			quadcnt++;
			tricnt++;
		}
	}
	return tricnt;
}
//#########################################################
PluginTag *GetPlugTag(BaseObject *ob,LONG id)//OK
{
	PluginTag *tag;
	LONG tn = 0;
	//tag = (PluginTag*)ob->GetFirstTag();if (tag) print(tag->GetNodeID());//TEST
	do{
		tag = (PluginTag*)ob->GetTag(Tplugin,tn);
		if (!tag) break;
		tn++;
	}while(tag->GetNodeID()!=id); //GetNodeID ???

return tag;
}
//#########################################################
LONG ASESaverData::GetMatIDCount(BaseContainer  *idtagdata, BaseDocument *doc)//WORK
{
	if (!idtagdata) return 1;
	LONG	matcnt = 1;

	for (LONG c=0; c<64; c++)
	{
		if (!idtagdata->GetLink(ASEIDT_MTLID+c,doc,Tpolygonselection)) break;
		matcnt++;
	}
	return matcnt % 64;
}
//#########################################################
Bool ASESaverData::GetSelList(BaseContainer  *idtagdata, BaseDocument *doc, BaseSelect **sel)
{
	if (!idtagdata) return FALSE;
	SelectionTag  *seltag = NULL;
	for (LONG c=0; c<64; c++)
	{
		if (idtagdata->GetLink(ASEIDT_MTLID+c,doc))
		{
			seltag = (SelectionTag*)idtagdata->GetLink(ASEIDT_MTLID+c,doc,Tpolygonselection);
			if (seltag) {
				sel[c] = seltag->GetBaseSelect();
			}
		}else{
			sel[c] = NULL;
		}
	}

	return TRUE;
}
//#########################################################
Bool ASESaverData::WriteObject(BaseDocument *doc, PolygonObject *sorc, LONG &materialNumber, ASEEdata &aseedata)
{
	if (sorc && sorc->GetType()==Opolygon)
	{
		String		opname = sorc->GetName();

		

		LONG	vc = 1;
 		LONG	nc = aseedata.commadigits;
		Matrix	mg = sorc->GetMg();//?????????	
			
		Vector		*padr	= sorc->GetPoint();	//if (!padr) return FALSE;
		LONG		pcnt	= sorc->GetPointCount();
		CPolygon   *vadr	= sorc->GetPolygon(); //if (!vadr) return FALSE;
		LONG		vcnt	= sorc->GetPolygonCount();

		if (padr && vadr){

			//ID TAG ?????
			PluginTag		*idtag = GetPlugTag(sorc,DI_IDTAG_TAG_ID);
			BaseContainer   *idtagdata = NULL; 
			LONG			matcnt = 1;
			if (idtag) {
				idtagdata = idtag->GetDataInstance();
				matcnt = GetMatIDCount(idtagdata,doc);//TEST
				if (idtagdata->GetBool(ASEIDT_COLLISION)) opname = "MCDCX_"+opname;
			}	
			BaseSelect		*sel[64];
			GetSelList(idtagdata,doc,sel);

			print("ASE Export Obj: "+opname+" ",materialNumber);

			LONG		num		= 0;
			LONG		vn		= 0;

			LONG		quadcnt = 0;
			LONG		tricnt	= CountTriangles(vadr,vcnt,quadcnt);
			Vector		pos;

			UVWStruct	uvw;
			Vector		u;
			LONG		i,c;

			LONG		matid;
			LONG		smoid   = 1;//TO DO ???
			LONG		curtime = 0;//TO DO ???

			ObjectColorProperties colorProp;//Color 
			sorc->GetColorProperties(&colorProp);//Color 
			UVWTag *uvwtag = (UVWTag*)sorc->GetTag(Tuvw);//if (!uvwtag)

			
			WriteLine("*GEOMOBJECT {");
			
			WriteLine("	*NODE_NAME \""+opname+"\"");
			WriteLine("	*NODE_TM {");

			WriteLine("		*NODE_NAME \""+opname+"\"");//jedes ding (node) hat einen namen
			//WriteLine("		*INHERIT_POS   "+RealToString(mg.off.x)+"   "+RealToString(mg.off.y)+"   "+RealToString(mg.off.z));//m.off
			//WriteLine("		*INHERIT_ROT 0 0 0"); //!!!!
			//WriteLine("		*INHERIT_SCL 0 0 0"); //!!!!
			//??? ROW0,1,2,3 ???  diese 4 zeilen geben die 3x4 tranformation matrix an 
			WriteLine("		*TM_ROW	   "+RealToString(mg.v1.x)+"   "+RealToString(mg.v1.y)+"   "+RealToString(mg.v1.z));//m.off
			WriteLine("		*TM_ROW1   "+RealToString(mg.v2.x)+"   "+RealToString(mg.v2.y)+"   "+RealToString(mg.v2.z));//m.off
			WriteLine("		*TM_ROW2   "+RealToString(mg.v3.x)+"   "+RealToString(mg.v3.y)+"   "+RealToString(mg.v3.z));//m.off
			WriteLine("		*TM_ROW3   "+RealToString(mg.off.x)+"   "+RealToString(mg.off.y)+"   "+RealToString(mg.off.z));//m.off
			
			WriteLine("		*TM_POS   "+RealToString(mg.off.x)+"   "+RealToString(mg.off.y)+"   "+RealToString(mg.off.z));//m.off //position des objekts
			
			//WriteLine("		*TM_ROTAXIS 0.000000	0.000000	0.000000"); //rotations quaternion 
			//WriteLine("		*TM_ROTANGLE 0.000000"); //rotations quaternion 
			//WriteLine("		*TM_SCALE 1.000000	1.000000	1.000000"); //vergrösserungs faktor, für jede achse separat
			//WriteLine("		*TM_SCALEAXIS 0.000000	0.000000	0.000000"); //diese zwei zeile sind mir ein bisschen unklar. wahrscheinlich kann man durch schärung eine roation wiedergeben, und die sind die nummern um wieviel geschärt wird.
			//WriteLine("		*TM_SCALEAXISANG 0.000000"); //????!!!!
			
			WriteLine("	}");

			
			WriteLine("	*MESH {");
			WriteLine("		*TIMEVALUE "+LongToString(curtime));
			WriteLine("		*MESH_NUMVERTEX "+LongToString(pcnt));
			WriteLine("		*MESH_NUMFACES "+LongToString(tricnt));//TriCnt

			
			//VERTEXES
			WriteLine("		*MESH_VERTEX_LIST {");
			for (i=0; i<pcnt; i++){
				pos = padr[i]*mg;
				WriteLine("			*MESH_VERTEX    "+LongToString(i)+
					"   "+RealToString(pos.x,vc,nc)+
					"   "+RealToString(pos.z,vc,nc)+//TEST z ???
					"   "+RealToString(pos.y,vc,nc));//TESt y ???	
			}
			WriteLine("		}");

			
			//POLYGONS
			WriteLine("		*MESH_FACE_LIST {");
			num = 0;
			for (i=0; i<vcnt; i++)
			{
				matid = 0;
				if (idtag){ 
					for (c=0; c<matcnt; c++) if (sel[c] && sel[c]->IsSelected(i)) {matid = c+1; break;}
				}

				WriteLine("			*MESH_FACE "+LongToString(num++)+
				   ":    A: "+LongToString(vadr[i].a)+
					"    B: "+LongToString(vadr[i].b)+
					"    C: "+LongToString(vadr[i].c)+
					"    AB: 1    BC: 1    CA: 0    *MESH_SMOOTHING "+LongToString(smoid)+
					"	*MESH_MTLID "+LongToString(matid));// //????!!!!
				
				if (vadr[i].c!=vadr[i].d){
					WriteLine("			*MESH_FACE "+LongToString(num++)+
					":    A: "+LongToString(vadr[i].a)+
						"    B: "+LongToString(vadr[i].c)+
						"    C: "+LongToString(vadr[i].d)+
						"    AB: 1    BC: 1    CA: 0    *MESH_SMOOTHING "+LongToString(smoid)+
						"	*MESH_MTLID "+LongToString(matid));// //????!!!!
				}
			}
			WriteLine("		}");
			
			
			//VERTEX NORMAL DONT WORK ????
			if (aseedata.savenormals){
				WriteLine("		*MESH_NORMALS {");
				num = 0;
				Vector	norm;
				
				for (i=0; i<vcnt; i++)
				{
					norm = !((padr[vadr[i].b]-padr[vadr[i].a])%(padr[vadr[i].c]-padr[vadr[i].a]));//Normal
					WriteLine("			*MESH_FACENORMAL "+LongToString(num++)+
						"   "+RealToString(norm.x,vc,nc)+
						"   "+RealToString(norm.z,vc,nc)+
						"   "+RealToString(norm.y,vc,nc));
					//WriteLine("				*MESH_VERTEXNORMAL 0 0.0 0.0 0.0");//TEST
				
					if (vadr[i].c!=vadr[i].d){
						norm = !((padr[vadr[i].c]-padr[vadr[i].a])%(padr[vadr[i].d]-padr[vadr[i].a]));//Normal
						WriteLine("			*MESH_FACENORMAL "+LongToString(num++)+
							"   "+RealToString(norm.x,vc,nc)+
							"   "+RealToString(norm.z,vc,nc)+
							"   "+RealToString(norm.y,vc,nc));
						//WriteLine("				*MESH_VERTEXNORMAL 0 0.0 0.0 0.0");//TEST
					}
				}
				WriteLine("		}");
			}


			//UVW Coordinate
			WriteLine("		*MESH_NUMTVERTEX "+LongToString(vcnt*3+quadcnt));//UVW //????!!!!
			WriteLine("		*MESH_TVERTLIST {");
			num = 0;
			for (i=0; i<vcnt; i++)
			{		
					if (uvwtag) uvw = uvwtag->Get(i);  //????!!!!
					WriteLine("			*MESH_TVERT    "+LongToString(num++)+
						"   "+RealToString(uvw.a.x,vc,nc)+
						"   "+RealToString(uvw.a.y,vc,nc)+
						"   "+RealToString(uvw.a.z,vc,nc));
					WriteLine("			*MESH_TVERT    "+LongToString(num++)+
						"   "+RealToString(uvw.b.x,vc,nc)+
						"   "+RealToString(uvw.b.y,vc,nc)+
						"   "+RealToString(uvw.b.z,vc,nc));
					WriteLine("			*MESH_TVERT    "+LongToString(num++)+
						"   "+RealToString(uvw.c.x,vc,nc)+
						"   "+RealToString(uvw.c.y,vc,nc)+
						"   "+RealToString(uvw.c.z,vc,nc));

					if (vadr[i].c!=vadr[i].d){
						WriteLine("			*MESH_TVERT    "+LongToString(num++)+
							"   "+RealToString(uvw.d.x,vc,nc)+
							"   "+RealToString(uvw.d.y,vc,nc)+
							"   "+RealToString(uvw.d.z,vc,nc));
					}
			}
			WriteLine("		}");

			//UVW Polygone
			WriteLine("		*MESH_NUMTVFACES "+LongToString(tricnt));//TriCnt
			WriteLine("		*MESH_TFACELIST {");
			num = 0;
			vn = 0;
			for (i=0; i<vcnt; i++)
			{
				WriteLine("			*MESH_TFACE    "+LongToString(num++)+
					"   "+LongToString(vn  )+
					"   "+LongToString(vn+1)+
					"   "+LongToString(vn+2));
				if (vadr[i].c!=vadr[i].d){
					WriteLine("			*MESH_TFACE    "+LongToString(num++)+
						"   "+LongToString(vn  )+
						"   "+LongToString(vn+2)+
						"   "+LongToString(vn+3));
					vn+=4;//Quads
				}else{
					vn+=3;//Triagles
				}
			}

			WriteLine("		}");


			//   "*MESH {"
			WriteLine("	}");
			WriteLine("	*PROP_MOTIONBLUR 0"); //????!!!!
			WriteLine("	*PROP_CASTSHADOW 1"); //????!!!!
			WriteLine("	*PROP_RECVSHADOW 1"); //????!!!!
			WriteLine("	*MATERIAL_REF "+LongToString(materialNumber)); //!!!!
			WriteLine("	*WIREFRAME_COLOR   "					
					+RealToString(colorProp.color.x)+"   "
					+RealToString(colorProp.color.y)+"   "
					+RealToString(colorProp.color.z));
			WriteLine("}");
			
			
		}
		materialNumber++;
	}

	//RECURSE
	for (sorc=(PolygonObject*)sorc->GetDown(); sorc; sorc=(PolygonObject*)sorc->GetNext()){
		if (!WriteObject(doc,sorc,materialNumber,aseedata)) break;
	}

	return TRUE;
}
//#########################################################
Bool ASESaverData::WriteScene(BaseDocument *doc)
{
	BaseTime	mintime = doc->GetMinTime();
	BaseTime	maxtime = doc->GetMaxTime();
	Filename 	docname	= doc->GetDocumentName();

	LONG	fps	  = doc->GetFps();
	LONG	start = mintime.GetFrame(fps);
	LONG	end   = maxtime.GetFrame(fps);

	WriteLine("*SCENE {");
	WriteLine("	*SCENE_FILENAME \""+docname.GetString()+"\"");
	WriteLine("	*SCENE_FIRSTFRAME "+LongToString(start));
	WriteLine("	*SCENE_LASTFRAME "+LongToString(end));
	WriteLine("	*SCENE_FRAMESPEED "+LongToString(fps));
	WriteLine("	*SCENE_TICKSPERFRAME 160");
	WriteLine("	*SCENE_BACKGROUND_STATIC 0.0 0.0 0.0");
	WriteLine("	*SCENE_AMBIENT_STATIC 0.0 0.0 0.0");
	WriteLine("}");

	return TRUE;
}
LONG ASESaverData::CountObjects(PolygonObject *sorc, LONG cnt)
{
	for (sorc=(PolygonObject*)sorc->GetDown(); sorc; sorc=(PolygonObject*)sorc->GetNext()){
		if (sorc->GetType()==Opolygon) cnt++; //Count only Polygon Objects
		cnt = CountObjects(sorc,cnt);
	}
	return cnt;
}

//###################################################################
Bool ASESaverData::WriteMaterialList(BaseDocument *doc, PolygonObject *sorc, ASEEdata &aseedata)
{
	LONG	materialCount = CountObjects(sorc,1); //??????
	LONG	matNumber = 0;

	WriteLine("*MATERIAL_LIST {");
	WriteLine("		*MATERIAL_COUNT "+LongToString(materialCount));

	WriteMaterials(doc,sorc,matNumber,aseedata);

	WriteLine("}"); //MATERIAL_LIST
	return TRUE;
}

//###########################################
Bool ASESaverData::WriteMaterials(BaseDocument *doc, PolygonObject *sorc, LONG &materialNumber, ASEEdata &aseedata)
{
	//print("Mat: "+sorc->GetName()+" ",materialNumber);

	if (sorc && sorc->GetType()==Opolygon){

		LONG		c;
		String		name;
		String		obname = sorc->GetName() + " ";

		//ID TAG
		PluginTag		*idtag = GetPlugTag(sorc,DI_IDTAG_TAG_ID);
		BaseContainer   *idtagdata = NULL; if (idtag) idtagdata = idtag->GetDataInstance();
		LONG			 matcnt = GetMatIDCount(idtagdata,doc);//TEST
		SelectionTag	*seltag = NULL;

		WriteLine("		*MATERIAL " + LongToString(materialNumber) + " {");
		WriteLine("			*MATERIAL_NAME \"" + LongToString(materialNumber) + " " + obname +" - Material\"");
		WriteLine("			*MATERIAL_CLASS \"Multi/Sub-Object\"");
		WriteLine("			*MATERIAL_AMBIENT 0.5 0.5 0.5");
		WriteLine("			*MATERIAL_DIFFUSE 0.5 0.5 0.5");
		WriteLine("			*MATERIAL_SPECULAR 0.9 0.9 0.9");
		WriteLine("			*MATERIAL_SHINE 0.2");
		WriteLine("			*MATERIAL_SHINESTRENGTH 0.0");
		WriteLine("			*MATERIAL_TRANSPARENCY 0.0");
		WriteLine("			*MATERIAL_WIRESIZE 1.0");
		//WriteLine("			*MATERIAL_SHADING Blinn");
		//-------------------------------------------
		WriteLine("			*NUMSUBMTLS "+LongToString(matcnt));

		for (c=0; c<matcnt; c++)
		{
			name = obname + "Default";
			if (idtagdata) seltag = (SelectionTag*)idtagdata->GetLink(ASEIDT_MTLID+c-1,doc,Tpolygonselection);
			if (aseedata.matObjecPrefix){
				if (seltag) name = obname + seltag->GetName();
			}else{
				if (seltag) name = seltag->GetName();
			}
			WriteLine("			*SUBMATERIAL "+LongToString(c)+" {");
			WriteLine("				*MATERIAL_NAME \""+name+"\"");//???
			WriteLine("				*MATERIAL_CLASS \"Standard\"");
			WriteLine("				*MAP_DIFFUSE {");
			WriteLine("					*MAP_NAME \"Map #"+LongToString(c+1)+"\"");
			WriteLine("					*MAP_CLASS \"Bitmap\"");
			WriteLine("					*MAP_SUBNO 1");
			WriteLine("					*MAP_AMOUNT 1.0");
			WriteLine("					*BITMAP \"C:\\default"+LongToString(c)+".bmp\"");
			WriteLine("					*UVW_U_OFFSET 0.0");
			WriteLine("					*UVW_V_OFFSET 0.0");
			WriteLine("					*UVW_U_TILING 1.0");
			WriteLine("					*UVW_V_TILING 1.0");
			WriteLine("				}");
			WriteLine("			}");
		}
		//-------------------------------------------
		WriteLine("		}");//MATERIAL
		materialNumber++;
	}

	//RECURSE
	for (sorc=(PolygonObject*)sorc->GetDown(); sorc; sorc=(PolygonObject*)sorc->GetNext()){
		if (!WriteMaterials(doc,sorc,materialNumber,aseedata)) break;
	}

	return TRUE;
}
//############################ Save ############################
LONG ASESaverData::Save(PluginSceneSaver *node, const Filename &name, BaseDocument *doc, LONG filterflags)
{
	if (!(filterflags&SCENEFILTER_OBJECTS)) return FILEERROR_NONE;
	
	LONG	materialNumber = 0;//???

	//TO DO TEST FREE OBJECT ????

	//Read Seting
	ASEEdata		aseedata;
	aseedata.ReadSetting(node->GetDataInstance());


	BaseObject		*urop = doc->GetActiveObject(); 
	if (!urop){ print("ASE Export: Select any Object"); return FILEERROR_NONE;}
	
	PolygonObject	*nuop = NULL;
	//Polygonize
	if (urop->GetType()!=Opolygon){
		// Get polygon object
		ModelingCommandData md1; md1.op = urop; md1.doc = doc; //Create OBJECT (md1)
		if (!SendModelingCommand(MCOMMAND_CURRENTSTATETOOBJECT, md1)){ return FILEERROR_UNKNOWN_VALUE;}

		if (aseedata.joinob){
			// Join
			ModelingCommandData md2; md2.op = md1.result_ex; //Create OBJECT (md2)
			if (!SendModelingCommand(MCOMMAND_JOIN, md2)){ return FILEERROR_UNKNOWN_VALUE;}
			BaseObject::Free(md1.result_ex);//FREE OBJECT  (md1)
			nuop = ToPoly(md2.result_ex);
		}else{
			nuop = ToPoly(md1.result_ex);
		}
	}else{
		if (aseedata.joinob){
			// Join
			ModelingCommandData md2; md2.op = urop;  //Create OBJECT (md2)
			if (!SendModelingCommand(MCOMMAND_JOIN, md2)){ return FILEERROR_UNKNOWN_VALUE;}
			nuop = ToPoly(md2.result_ex);
		}else{
			nuop = ToPoly(urop->GetClone(0,NULL));
		}
	}
	if (!nuop){ return FILEERROR_UNKNOWN_VALUE;}

	PolygonObject	*op = NULL;
		//ReverseNormals
		if (aseedata.reversenormals){
			ModelingCommandData md3; md3.op = nuop; 
			if (!SendModelingCommand(MCOMMAND_REVERSENORMALS, md3)){ return FILEERROR_UNKNOWN_VALUE;}
			op = ToPoly(md3.op);//Read Result
		}else{
			op = nuop; //Read Result
		}
	if (!op){ return FILEERROR_UNKNOWN_VALUE;}

	file = BaseFile::Alloc(); if (!file) return FILEERROR_MEMORY; //???
	if (!file->Open(name,GE_WRITE,FILE_NODIALOG,GE_INTEL)) return file->GetError();
	
	//-------------------------------------------------------------
	LONG year = 0,month = 0,day = 0,hour = 0,minute = 0,second = 0; 
	GeGetSysTime(&year,&month,&day,&hour,&minute,&second);
	
	WriteLine("*3DSMAX_ASCIIEXPORT	200");
	WriteLine("*COMMENT \"AsciiExport Version  0,31 - Remotion 2002 "+LongToString(day)+"."+LongToString(month)+"."+LongToString(year)+" \"");
	WriteScene(doc);

	WriteMaterialList(doc,op,aseedata); //?????
	WriteObject(doc,op,materialNumber,aseedata);//Triangulated Polygon Object RECURSE ???

	//ENDE -------------------------------
	PolygonObject::Free(op); //FREE OBJECT  (op,md2)
	BaseFile::Free(file);
	return FILEERROR_NONE;
}
//-----------------------------------------------------------------------------

//###################################################
Bool RegisterASEio(void)
{
	//String name=GeLoadString(IDS_STL); if (!name.Content()) return TRUE;
	//if (!RegisterSceneLoaderPlugin(ASE_LOADER_ID,"ASE Load",SCENEFILTER_OBJECTS,aseld,"ase")) return FALSE; 
	if (!RegisterSceneSaverPlugin(ASE_SAVER_ID,"ASE [Remotion]",SCENEFILTER_OBJECTS,ASESaverData::Alloc,"aseexport","ase")) return FALSE;

	return TRUE;
}
