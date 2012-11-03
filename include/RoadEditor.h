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
#include "VisualHelper.h"
#include "Event.h"
#include "RoadRenderListener.h"

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
    class PluginExport CRoadEditor : public CNodeEditor
    {
        friend class CRoadEditorFactory;
    public:
       
        /** @copydoc CBaseEditor::getHandle() */
        inline virtual void			*getHandle() {return static_cast<void*>(mRoadHandle);};
		/** @copydoc CBaseEditor::getPropertyMap(OgitorsPropertyValueMap &) */
		virtual void				getPropertyMap(OgitorsPropertyValueMap& map);
		/** @copydoc CBaseEditor::getObjectContextMenu(UTFStringVector &) */
        virtual bool                getObjectContextMenu(UTFStringVector &menuitems);
        /** @copydoc CBaseEditor::onObjectContextMenu(int) */
        virtual void                onObjectContextMenu(int menuresult);
		/** @copydoc CBaseEditor::onDropMaterial(Ogre::Ray ray, Ogre::Vector3, const Ogre::String&) */
        virtual void				onDropMaterial(Ogre::Ray ray, Ogre::Vector3 position, const Ogre::String& materialname);
		/** @copydoc CBaseEditor::postSceneUpdate(Ogre::SceneManager *, Ogre::Camera *, Ogre::RenderWindow *) */
		// virtual bool				postSceneUpdate(Ogre::SceneManager *SceneMngr, Ogre::Camera *Camera, Ogre::RenderWindow *RenderWindow);
		/** @copydoc CBaseEditor::load(bool) */
		virtual bool				load(bool async);
		/** @copydoc CBaseEditor::unLoad() */
		virtual bool				unLoad();

		virtual void				update();
		virtual void				deformTerrain();

    protected:
		virtual void				onUndoManagerNotification(Ogitors::IEvent* evt);
		virtual void				onTerrainEditorChange(Ogitors::IEvent* evt);
		
		OgitorsProperty<bool>		*mDeformTerrain;
		OgitorsProperty<Ogre::Real>	*mRoadTesselation;
		OgitorsProperty<Ogre::Real>	*mRoadWidth;

		CRoad								*mRoadHandle;			// Road curve handle

		void						_addPoint();

		bool						_setDeformation(OgitorsPropertyBase* property, const bool& deform);
		bool						_setTesselation(OgitorsPropertyBase* property, const Ogre::Real& width);
		bool						_setWidth(OgitorsPropertyBase* property, const Ogre::Real& width);

        /**
        * Constructor
        * @param factory Handle to terrain editor factory
        */
        CRoadEditor(CBaseEditorFactory *factory);
        /**
        * Destructor
        */
        virtual     ~CRoadEditor();

	private:
		RoadRenderListener *mRoadRenderListener;
    };

    //! Particle editor factory class
    /*!  
        A class that is responsible for instantiating particle editor class(es)
    */
    class PluginExport CRoadEditorFactory: public CNodeEditorFactory
    {
    public:
        /** @copydoc CBaseEditorFactory::CBaseEditorFactory() */
        CRoadEditorFactory(OgitorsView *view = 0);
        /** @copydoc CBaseEditorFactory::~CBaseEditorFactory() */
        virtual ~CRoadEditorFactory() {};
        /** @copydoc CBaseEditorFactory::duplicate(OgitorsView *view) */
        virtual CBaseEditorFactory*	duplicate(OgitorsView *view);
        /** @copydoc CBaseEditorFactory::CreateObject(CBaseEditor **, OgitorsPropertyValueMap &) */
        virtual CBaseEditor			*CreateObject(CBaseEditor **parent, OgitorsPropertyValueMap &params);
		/** @copydoc CBaseEditorFactory::DestroyObject(CBaseEditor *) */
		virtual void DestroyObject(CBaseEditor *object);

        /** @copydoc CBaseEditorFactory::GetPlaceHolderName() */
        virtual Ogre::String		GetPlaceHolderName() {return "scbRoadPoint.mesh";};
    };
}
