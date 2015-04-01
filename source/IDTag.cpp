/////////////////////////////////////////////////////////////
// Tag Plugin  IDTag
/////////////////////////////////////////////////////////////
// Date: 08.02.2003 Time: 18:35:36
/////////////////////////////////////////////////////////////
// (c) 2002 REMOTION, all rights reserved 
/////////////////////////////////////////////////////////////

#include "c4d.h"
#include "c4d_symbols.h"
#include "TIDTag.h"

#define DI_IDTAG_TAG_ID 1011248

//#################################################################
class IDTagTagData : public TagData
{
	INSTANCEOF(IDTagTagData,TagData)
	public:
		virtual Bool Init(GeListNode *node);
		//virtual Bool Draw(PluginTag *tag, BaseObject *op, BaseDraw *bd, BaseDrawHelp *bh);
		virtual LONG Execute(PluginTag *tag, BaseDocument *doc, BaseObject *op, BaseThread *bt, LONG priority, LONG flags);
		//virtual Bool AddToExecution(PluginTag *tag, PriorityList *list);
		virtual Bool GetDDescription(GeListNode *node, Description *description,LONG &flags);
		//
		static NodeData *Alloc(void) { return gNew IDTagTagData; }
};
//#################################################################
Bool IDTagTagData::Init(GeListNode *node)
{
	BaseTag		  *tag  = (BaseTag*)node; if (!tag) return FALSE;
	BaseContainer *data = tag->GetDataInstance();

	data->SetBool(ASEIDT_COLLISION,FALSE);

	return TRUE;
}
//#################################################################
Bool IDTagTagData::GetDDescription(GeListNode *node, Description *description,LONG &flags)
{
	if (!description->LoadDescription(node->GetType())) return FALSE;

	BaseDocument  *doc  = node->GetDocument();
	BaseTag		  *tag  = (BaseTag*)node;if (!tag) return FALSE;
	BaseContainer *data = tag->GetDataInstance();
	/*
	BaseList2D		*link0= data->GetLink(ASEIDT_MTLID+0,doc);
	//TO DO
		BaseContainer bc2 = GetCustomDataTypeDefault(DTYPE_BASELISTLINK);
		bc2.SetString(DESC_NAME,"PolySel2");
		bc2.SetString(DESC_SHORT_NAME,"PolySel2");
		bc2.SetLong(DESC_ANIMATE,DESC_ANIMATE_OFF);
		bc2.SetBool(DESC_REMOVEABLE,FALSE);
		if (!description->SetParameter(DescLevel(ASEIDT_MTLID+1,DTYPE_BASELISTLINK,0),bc2,DescLevel(ID_TAGPROPERTIES))) return FALSE;
	*/
	BaseContainer abc; abc.SetString(Tpolygonselection,"Tpolygonselection");
	for (LONG c=0; c<63; c++)
	{
		if (!data->GetLink(ASEIDT_MTLID+c,doc)) break;

		BaseContainer bc2 = GetCustomDataTypeDefault(DTYPE_BASELISTLINK);
		bc2.SetString(DESC_NAME,"ID "+LongToString(c+2));
		bc2.SetString(DESC_SHORT_NAME,"ID "+LongToString(c+2));
		bc2.SetContainer(DESC_ACCEPT,abc);
		bc2.SetLong(DESC_ANIMATE,DESC_ANIMATE_OFF);
		bc2.SetBool(DESC_REMOVEABLE,FALSE);
		if (!description->SetParameter(DescLevel(ASEIDT_MTLID+c+1,DTYPE_BASELISTLINK,0),bc2,DescLevel(ID_TAGPROPERTIES))) return FALSE;
	}

	flags |= DESCFLAGS_DESC_LOADED;
	return SUPER::GetDDescription(node,description,flags);//Necessary
}

//#################################################################
LONG IDTagTagData::Execute(PluginTag *tag, BaseDocument *doc, BaseObject *op, BaseThread *bt, LONG priority, LONG flags)
{
	return EXECUTION_RESULT_OK;
}

//#################################################################
Bool RegisterIDTagTag(void)
{
	String name=GeLoadString(IDS_IDTAG); if (!name.Content()) return TRUE;
	return RegisterTagPlugin(DI_IDTAG_TAG_ID,name,TAG_VISIBLE,IDTagTagData::Alloc,"TIDTag","IDTag.tif",0);
}
