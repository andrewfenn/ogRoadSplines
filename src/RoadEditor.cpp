/*/////////////////////////////////////////////////////////////////////////////////
/// An
///    ___   ____ ___ _____ ___  ____
///   / _ \ / ___|_ _|_   _/ _ \|  _ \
///  | | | | |  _ | |  | || | | | |_) |
///  | |_| | |_| || |  | || |_| |  _ <
///   \___/ \____|___| |_| \___/|_| \_\
///                              File
///
/// Copyright (c) 2008-2010 Ismail TARIM <ismail@royalspor.com> and the Ogitor Team
//
/// The MIT License
///
/// Permission is hereby granted, free of charge, to any person obtaining a copy
/// of this software and associated documentation files (the "Software"), to deal
/// in the Software without restriction, including without limitation the rights
/// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
/// copies of the Software, and to permit persons to whom the Software is
/// furnished to do so, subject to the following conditions:
///
/// The above copyright notice and this permission notice shall be included in
/// all copies or substantial portions of the Software.
///
/// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
/// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
/// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
/// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
/// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
/// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
/// THE SOFTWARE.
////////////////////////////////////////////////////////////////////////////////*/

#include "OgitorsPrerequisites.h"
#include "BaseEditor.h"
#include "OgitorsRoot.h"
#include "OgitorsSystem.h"
#include "NodeEditor.h"
#include "TerrainEditor.h"
#include "TerrainPageEditor.h"
#include "EntityEditor.h"
#include "RoadEditor.h"
#include "RoadPointEditor.h"
#include "EventManager.h"
#include "tinyxml.h"

using namespace Ogitors;
#define DEFAULT_SPACING Vector3(5.0f, 5.0f, 0.0f)

//-------------------------------------------------------------------------------
CRoadEditor::CRoadEditor(CBaseEditorFactory *factory) : CNodeEditor(factory)
{
	mUsesGizmos = false;
	mUsesHelper = false;

	mRoadHandle = NULL;

	PROPERTY_PTR(mDeformTerrain, "deformation"   ,bool   ,false       ,0,SETTER(bool, CRoadEditor, _setDeformation));
	PROPERTY_PTR(mRoadTesselation, "tesselation"   ,Ogre::Real   ,0.0f       ,0,SETTER(Ogre::Real, CRoadEditor, _setTesselation));
	PROPERTY_PTR(mRoadWidth       , "width"   ,Ogre::Real   ,0.0f       ,0,SETTER(Ogre::Real, CRoadEditor, _setWidth));

	EventManager::Instance()->connectEvent("undomanager_notification", this, true, 0, true, 0, EVENT_CALLBACK(CRoadEditor, onUndoManagerNotification));
	EventManager::Instance()->connectEvent("terrain_editor_change", this, true, 0, true, 0, EVENT_CALLBACK(CRoadEditor, onTerrainEditorChange));
}  

CRoadEditor::~CRoadEditor()
{
	EventManager::Instance()->disconnectEvent("undomanager_notification", this);
	EventManager::Instance()->disconnectEvent("terrain_editor_change", this);
}

void CRoadEditor::onUndoManagerNotification(Ogitors::IEvent* evt)
{
	update();
}

void CRoadEditor::onTerrainEditorChange(Ogitors::IEvent* evt)
{
	update();
}

void CRoadEditor::onDropMaterial(Ogre::Ray ray, Ogre::Vector3 position, const Ogre::String& materialname)
{
	if(mRoadHandle)
	{
		// mRoadMaterial->set(materialname);
		update();
	}
}

void CRoadEditor::deformTerrain()
{
	for (int iIndex = 0; iIndex < mRoadHandle->getNumPoints(); iIndex++)
	{
		Vector3 Location = mRoadHandle->getPoint(iIndex);
		CTerrainPageEditor *pTerrainPageEditor = static_cast<CTerrainPageEditor *>(OgitorsRoot::getSingletonPtr()->FindObject("Terrain Page Object"));
		if(pTerrainPageEditor != NULL)
		{
			Ogre::Terrain *pTerrain = static_cast<Ogre::Terrain*>(pTerrainPageEditor->getHandle());
			if(pTerrain != NULL)
			{
				float *mHeightData = pTerrain->getHeightData();
			}
		}
	}
}

bool CRoadEditor::_setDeformation(OgitorsPropertyBase* property, const bool& deformation)
{
	if(mRoadHandle)
	{
		if(deformation)
		{
			if(OgitorsSystem::getSingletonPtr()->DisplayMessageDialog(OTR("Terrain deformation is recommended when road path is completed.\nDo you want to continue?"),DLGTYPE_YESNO) == DLGRET_YES)
				deformTerrain();
			else
				return false;
		}
		return true;
	}
	
	return false;
}

bool CRoadEditor::_setTesselation(OgitorsPropertyBase* property, const Ogre::Real& width)
{
	if(mRoadHandle)
	{
		mRoadHandle->setTesselation(width);
		mRoadHandle->update(OgitorsRoot::getSingletonPtr()->GetTerrainEditor()->getHeightFunction());

		return true;
	}
	return false;
}

bool CRoadEditor::_setWidth(OgitorsPropertyBase* property, const Ogre::Real& width)
{
	if(mRoadHandle)
	{
		mRoadHandle->setWidth(width);
		mRoadHandle->update(OgitorsRoot::getSingletonPtr()->GetTerrainEditor()->getHeightFunction());

		return true;
	}
	return false;
}

bool CRoadEditor::load(bool async)
{
	if(!mLoaded->get() && CNodeEditor::load(async))
	{
		mRoadHandle = new CRoad(mHandle, getName());
		mHandle->attachObject(mRoadHandle);

		mRoadWidth->set(mRoadHandle->getWidth());
		mRoadTesselation->set(mRoadHandle->getTesselation());

		// Create render queue road listener
		mRoadRenderListener = new RoadRenderListener(mRoadHandle);
		OgitorsRoot::getSingletonPtr()->GetSceneManager()->addRenderObjectListener(mRoadRenderListener);

		mLoaded->set(true);
		return true;
	}

	return mLoaded->get();
}

bool CRoadEditor::unLoad()
{
	OGRE_DELETE mRoadHandle;
	mRoadHandle = NULL;

	return CNodeEditor::unLoad();
}
/*
bool CRoadEditor::postSceneUpdate(Ogre::SceneManager *SceneMngr, Ogre::Camera *Camera, Ogre::RenderWindow *RenderWindow)
{
	mHandle = getParent()->getNode()->createChildSceneNode(mName->get(), mPosition->get(), mOrientation->get());

	mRoadHandle = new CRoad(mHandle, getName());
	// _addPoint();

	mHandle->attachObject(mRoadHandle);

	mRoadWidth->set(mRoadHandle->getWidth());
	mRoadTesselation->set(mRoadHandle->getTesselation());

	return true;
}
*/
//-----------------------------------------------------------------------------------------
bool CRoadEditor::getObjectContextMenu(UTFStringVector &menuitems)
{
	menuitems.clear();
	menuitems.push_back(OTR("Add point") + ";:/icons/additional.svg");

	return true;
}
//-------------------------------------------------------------------------------
void CRoadEditor::onObjectContextMenu(int menuresult)
{
	if(menuresult == 0)	// 1 control point
		_addPoint();
}

void CRoadEditor::getPropertyMap(OgitorsPropertyValueMap& map)
{
	CBaseEditor::getPropertyMap(map);

    map = mProperties.getValueMap();

    getPropertyMapImpl(map);
}

void CRoadEditor::_addPoint()
{
	if(mRoadHandle == NULL)
		return;

	stringstream strID;
	strID << mRoadHandle->getNumPoints();

	OgitorsPropertyValueMap params;
	params["init"] = EMPTY_PROPERTY_VALUE;
	params["name"] = OgitorsPropertyValue(PROP_STRING, Ogre::Any(getName().append(Ogre::String(" Point ").append(strID.str()))));

	CBaseEditor *pPointEditor = Ogitors::OgitorsRoot::getSingletonPtr()->CreateEditorObject(this, "Road Control Point", params, true, true);
}

void CRoadEditor::update()
{
	if(mRoadHandle == NULL)
		return;

	int iPointIndex = 0;
	mRoadHandle->clear();

	NameObjectPairList& children = getChildren();
    NameObjectPairList::iterator i, iend;

    iend = children.end();

    for (i = children.begin(); i != iend; ++i)
	{
		CBaseEditor *pEditor = i->second;
		if(pEditor != NULL)
		{
			Ogre::SceneNode *pNode = (Ogre::SceneNode *)pEditor->getHandle();
			if(pNode != NULL)
			{
				mRoadHandle->addPoint(pNode->getPosition());
			}
		}
	}

	mRoadHandle->update(OgitorsRoot::getSingletonPtr()->GetTerrainEditor()->getHeightFunction());

}
//-----------------------------------------------------------------------------------------
//--------------CROADEDITORFACTORY-------------------------------------------
//-------------------------------------------------------------------------------
CRoadEditorFactory::CRoadEditorFactory(OgitorsView *view) : CNodeEditorFactory(view)
{
	mTypeName = "Road Object";
	mEditorType = ETYPE_CUSTOM_MANAGER;
	mAddToObjectList = true;
	mRequirePlacement = true;
	mIcon = "Icons/road.svg";
	mCapabilities = CAN_DELETE | CAN_UNDO | CAN_MOVE;

	OgitorsPropertyDef *definition;
	definition = AddPropertyDefinition("road","Road","The terrain road.",PROP_STRING);
	AddPropertyDefinition("width","Width","The width of the road",PROP_REAL);
	AddPropertyDefinition("tesselation","Quad size","The length of individual road curve blocks",PROP_REAL);
	AddPropertyDefinition("deformation","Deform Terrain","Deforms the terrain along road curve",PROP_BOOL);
	AddPropertyDefinition("splatroadtexture","Splat Texture","Splats road texture along the curve",PROP_BOOL);

	definition = AddPropertyDefinition("material","Material","Road material",PROP_STRING);
	definition->setOptions(OgitorsRoot::GetMaterialNames());
	AddPropertyDefinition("castshadows","Cast Shadows","Does the object cast shadows?",PROP_BOOL);

	OgitorsPropertyDefMap::iterator it = mPropertyDefs.find("layer");
	it->second.setAccess(true, true);
}
//-----------------------------------------------------------------------------------------
CBaseEditorFactory *CRoadEditorFactory::duplicate(OgitorsView *view)
{
	CBaseEditorFactory *ret = OGRE_NEW CRoadEditorFactory(view);
	ret->mTypeID = mTypeID;

	return ret;
}

void CRoadEditorFactory::DestroyObject(CBaseEditor *editor)
{
	CBaseEditorFactory::DestroyObject(editor);
}

//-----------------------------------------------------------------------------------------
CBaseEditor *CRoadEditorFactory::CreateObject(CBaseEditor **parent, OgitorsPropertyValueMap &params)
{
	CRoadEditor *object = OGRE_NEW CRoadEditor(this);

	OgitorsPropertyValueMap::iterator ni;

	if ((ni = params.find("init")) != params.end())
	{
		OgitorsPropertyValue value = EMPTY_PROPERTY_VALUE;
		value.val = Ogre::Any(CreateUniqueID("Road"));
		params["name"] = value;
		params.erase(ni);
	}

	object->createProperties(params);
	object->mParentEditor->init(*parent);
	object->registerForPostSceneUpdates();

	mInstanceCount++;
	return object;
}
//-------------------------------------------------------------------------------
