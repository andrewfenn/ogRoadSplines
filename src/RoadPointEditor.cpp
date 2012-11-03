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
#include "RoadPointEditor.h"
#include "tinyxml.h"

using namespace Ogitors;
#define DEFAULT_SPACING Vector3(5.0f, 5.0f, 0.0f)

// Use void CTerrainGroupEditor::_deform(CTerrainPageEditor *handle, Ogre::Vector3 &editpos, float timePassed)
// as an example of manual terrain deformation
//-------------------------------------------------------------------------------
CRoadPointEditor::CRoadPointEditor(CBaseEditorFactory *factory) : CNodeEditor(factory)
{
	mUsesGizmos = true;
	mUsesHelper = false;

	mEntityHandle = NULL;
}
//-----------------------------------------------------------------------------------------
bool CRoadPointEditor::load(bool async)
{
	if(mLoaded->get())
        return true;
    
    mHandle = getParent()->getNode()->createChildSceneNode(mName->get(), mPosition->get(), mOrientation->get());

	// Once road has been created, position it ON the terrain
	// Get terrain editor
	OgitorsRoot *mOgitorsRoot = OgitorsRoot::getSingletonPtr();

	try
	{
		mEntityHandle = mOgitorsRoot->GetSceneManager()->createEntity(mName->get(), "scbRoadPoint.mesh");
	}
	catch(...)
	{
		mEntityHandle = mOgitorsRoot->GetSceneManager()->createEntity(mName->get(), "missing_mesh.mesh");
		mEntityHandle->setMaterialName("MAT_GIZMO_X_L");
	}

	mHandle->attachObject(mEntityHandle);
	mEntityHandle->setQueryFlags(QUERYFLAG_MOVABLE);

	if(mOgitorsRoot->GetTerrainEditor() != NULL)
	{
		CONNECT_PROPERTY_MEMFN(this, "position", CRoadPointEditor, OnRoadPointPositionChange, mPointConnection);

		Ogitors::PGHeightFunction *func = mOgitorsRoot->GetTerrainEditor()->getHeightFunction();

		// Get height at the XZ location and change node' Y value (example of camera walk - walk height is 0 to be exactly on the terrain surface)
		Real TerrainY = 0 + (*func)(getDerivedPosition().x, getDerivedPosition().z, 0);
		// Compose vector that is positioned on the terrain (Y value)
		Vector3 TerrainPosition = Vector3(mHandle->getPosition().x, (TerrainY - getDerivedPosition().y) + mHandle->getPosition().y, mHandle->getPosition().z);

		// Invoke update to re-tesselate Road with all points
		OnRoadPointPositionChange(mPosition, Ogre::Any(TerrainPosition));
	}

	mLoaded->set(true);
    return true;
}

bool CRoadPointEditor::unLoad()
{
    if(!mLoaded->get())
        return true;

	if(CNodeEditor::unLoad())
	{
		if(mEntityHandle)
		{
			mEntityHandle->detachFromParent();
			mEntityHandle->_getManager()->destroyEntity(mEntityHandle);
			mEntityHandle = 0;
		}
	}
	return CNodeEditor::unLoad();
}
//-------------------------------------------------------------------------------
//--------------CPARTICLEEDITORFACTORY-------------------------------------------
//-------------------------------------------------------------------------------
CRoadPointEditorFactory::CRoadPointEditorFactory(OgitorsView *view) : CNodeEditorFactory(view)
{
	mTypeName = "Road Control Point";
	mEditorType = ETYPE_MOVABLE;
	mAddToObjectList = false;
	mRequirePlacement = false;
	mIcon = "Icons/road.svg";
	mCapabilities = CAN_MOVE | CAN_FOCUS | CAN_DRAG | CAN_UNDO | CAN_CLONE;

	OgitorsPropertyDef *definition;
	definition = AddPropertyDefinition("road control point","Road Control Point","The terrain road control point",PROP_STRING);
	//definition->setOptions(OgitorsRoot::GetParticleTemplateNames());
	AddPropertyDefinition("castshadows","Cast Shadows","Does the object cast shadows?",PROP_BOOL);

	OgitorsPropertyDefMap::iterator it = mPropertyDefs.find("layer");
	it->second.setAccess(true, true);
}
//-----------------------------------------------------------------------------------------
CBaseEditorFactory *CRoadPointEditorFactory::duplicate(OgitorsView *view)
{
	CBaseEditorFactory *ret = OGRE_NEW CRoadPointEditorFactory(view);
	ret->mTypeID = mTypeID;

	return ret;
}
//-----------------------------------------------------------------------------------------
CBaseEditor *CRoadPointEditorFactory::CreateObject(CBaseEditor **parent, OgitorsPropertyValueMap &params)
{
	CRoadPointEditor *object = OGRE_NEW CRoadPointEditor(this);

	OgitorsPropertyValueMap::iterator ni;

	if ((ni = params.find("init")) != params.end())
	{
		OgitorsPropertyValue value = EMPTY_PROPERTY_VALUE;
		// value.val = Ogre::Any((*parent)->getName().append(CreateUniqueID(" Control Point ")));
		// params["name"] = value;
		params.erase(ni);
	}

	object->createProperties(params);
	object->mParentEditor->init(*parent);

	mInstanceCount++;
	return object;
}

//-----------------------------------------------------------------------------------------
CBaseEditor *CRoadPointEditorFactory::CloneObject(CBaseEditor **parent, CBaseEditor *object)
{
    OgitorsPropertyValueMap params;
    object->getPropertyMap(params);

	CRoad *pRoad = (CRoad *)(*parent)->getHandle();

	stringstream strID;
	strID << pRoad->getNumPoints();

	params["name"] = OgitorsPropertyValue(PROP_STRING, Ogre::Any((*parent)->getName().append(Ogre::String(" Point ").append(strID.str()))));
    params["init"] = EMPTY_PROPERTY_VALUE;

	return CreateObject(parent, params);
}
//-------------------------------------------------------------------------------
