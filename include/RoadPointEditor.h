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

#include "Road.h"
#include "EntityEditor.h"
#include "RoadEditor.h"

#pragma once

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
   #ifdef PLUGIN_EXPORT
     #define PluginExport __declspec (dllexport)
   #else
     #define PluginExport __declspec (dllimport)
   #endif
#else
   #define PluginExport
#endif

namespace Ogitors
{
    //! Particle editor class
    /*!  
        A class that handles particle system(s) editing
    */
    class PluginExport CRoadPointEditor : public CNodeEditor
    {
        friend class CRoadPointEditorFactory;
    public:
		/** @copydoc CBaseEditor::isSerializable() */
		// virtual bool isSerializable() { return false; };
        /** @copydoc CBaseEditor::load(bool) */
        virtual bool load(bool async = true);
		/** @copydoc CBaseEditor::unLoad() */
        virtual bool     unLoad();
        /** @copydoc CBaseEditor::getHandle() */
        inline virtual void     *getHandle() {return static_cast<void*>(mHandle);};
		inline virtual void *getEntity() { return static_cast<void*>(mEntityHandle); };
    protected:
        /**
        * Constructor
        * @param factory Handle to terrain editor factory
        */
        CRoadPointEditor(CBaseEditorFactory *factory);
        /**
        * Destructor
        */
        virtual     ~CRoadPointEditor() {};

	private:
		Ogre::Entity						*mEntityHandle;
		OgitorsScopedConnection             mPointConnection;

		/**
        * Delegate function that is called when viewport camera position is changed
        * @param property position property handle
        * @param value new viewport camera position
        */
        void OnRoadPointPositionChange(const OgitorsPropertyBase* property, Ogre::Any value)
        {
            Vector3 oldVal = Ogre::any_cast<Ogre::Vector3>(property->getOldValue());
			Vector3 newVal = Ogre::any_cast<Ogre::Vector3>(value);

			Ogitors::PGHeightFunction *func = mOgitorsRoot->GetTerrainEditor()->getHeightFunction();

			// Get height at the XZ location and change node' Y value (example of camera walk - walk height is 0 to be exactly on the terrain surface)
			Real TerrainY = 0 + (*func)(getDerivedPosition().x, getDerivedPosition().z, 0);

			Vector3 TerrainPosition = Vector3(newVal.x, -(getDerivedPosition().y - TerrainY) + newVal.y, newVal.z);

			// Tell parent road editor to move associated curve point
			CRoadEditor *pParent = (CRoadEditor *)getParent();
			if(pParent != NULL)
			{
				mHandle->setPosition(TerrainPosition);
				pParent->update();
			}
        }
    };

    //! Particle editor factory class
    /*!  
        A class that is responsible for instantiating particle editor class(es)
    */
    class PluginExport CRoadPointEditorFactory: public CNodeEditorFactory
    {
    public:
        /** @copydoc CBaseEditorFactory::CBaseEditorFactory() */
        CRoadPointEditorFactory(OgitorsView *view = 0);
        /** @copydoc CBaseEditorFactory::~CBaseEditorFactory() */
        virtual ~CRoadPointEditorFactory() {};
        /** @copydoc CBaseEditorFactory::duplicate(OgitorsView *view) */
        virtual CBaseEditorFactory* duplicate(OgitorsView *view);
		/** @copydoc CBaseEditorFactory::CloneObject(CBaseEditor **, OgitorsPropertyValueMap &) */
        virtual CBaseEditor *CloneObject(CBaseEditor **parent, CBaseEditor *object);
        /** @copydoc CBaseEditorFactory::CreateObject(CBaseEditor **, OgitorsPropertyValueMap &) */
        virtual CBaseEditor *CreateObject(CBaseEditor **parent, OgitorsPropertyValueMap &params);
        /** @copydoc CBaseEditorFactory::GetPlaceHolderName() */
        virtual Ogre::String GetPlaceHolderName() {return "scbRoadPoint.mesh";};
    };
}
