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

#pragma once

#include <OgreRoot.h>
#include <OgreStringConverter.h>
#include <OgreRenderSystem.h>
#include <OgreRenderObjectListener.h>
#include "Road.h"

#define RENDER_QUEUE_ROAD_STENCIL	RENDER_QUEUE_MAIN + 1
#define RENDER_QUEUE_ROAD			RENDER_QUEUE_MAIN + 2

class RoadRenderListener : public Ogre::RenderObjectListener 
{
public: 
	RoadRenderListener(Renderable *pRoad)
	{
		mRoad = pRoad;
	};

	virtual void notifyRenderSingleObject(Renderable* rend, const Pass* pass, const AutoParamDataSource* source, 
			const LightList* pLightList, bool suppressRenderStateChanges)
	{
		if(mRoad != NULL && mRoad == rend)
		{
			Ogre::Matrix4 Transform, ReverseTransform, ModelvewProjection;
			getTransformMatrices(ModelvewProjection, &Transform, &ReverseTransform);

			Ogre::RenderSystem *pSystem = Ogre::Root::getSingletonPtr()->getRenderSystem();

			if(pass->getIndex() == 0)
			{
				pSystem->_setCullingMode(Ogre::CullingMode::CULL_NONE);
				pSystem->_setDepthBufferParams(true, false, Ogre::CompareFunction::CMPF_LESS_EQUAL);
				pSystem->setStencilCheckEnabled(true);

				pSystem->setStencilBufferParams(Ogre::CompareFunction::CMPF_ALWAYS_PASS, 0xFF, 0xFF, Ogre::StencilOperation::SOP_KEEP, Ogre::StencilOperation::SOP_INCREMENT,  Ogre::StencilOperation::SOP_KEEP, true);

				pass->getVertexProgram()->setParameter("transform", Ogre::StringConverter::toString(Transform));
			}
			else if(pass->getIndex() == 1)
			{
				pSystem->_setCullingMode(Ogre::CullingMode::CULL_ANTICLOCKWISE);
				pSystem->_setDepthBufferParams(false, false, Ogre::CompareFunction::CMPF_LESS_EQUAL);
				pSystem->setStencilCheckEnabled(true);
				
				pSystem->_setSceneBlending(Ogre::SceneBlendFactor::SBF_DEST_ALPHA, Ogre::SceneBlendFactor::SBF_ONE_MINUS_SOURCE_ALPHA);
				pSystem->setStencilBufferParams(Ogre::CompareFunction::CMPF_NOT_EQUAL, 0xFF, 0xFF, Ogre::StencilOperation::SOP_KEEP, Ogre::StencilOperation::SOP_KEEP,  Ogre::StencilOperation::SOP_ZERO, false);

				pass->getVertexProgram()->setParameter("transform", Ogre::StringConverter::toString(Transform));
				pass->getVertexProgram()->setParameter("reverse_transform", Ogre::StringConverter::toString(ReverseTransform));
			}
		}
	};

protected:
	void getTransformMatrices(Matrix4 ModelviewProjection, Matrix4 *pTransform, Matrix4 *pReverseTransform, Vector3 v0, Vector3 v1)
	{
		Real len = v0.distance(v1);
		Vector3 mid = 0.5f * (v0 + v1);

		Vector3 dir_x = (v1 - v0).normalisedCopy();
		Vector3 dir_z = dir_x.crossProduct(Ogre::Vector3::UNIT_Y).normalisedCopy();
		Vector3 dir_y = dir_z.crossProduct(dir_x).normalisedCopy();

		const float size = 30.0f; // Relatively large number, ideally should be computed to be only as big as necessary for a road segment
		const float down_factor = 0.3f; // Move the box down a little to better use the box volume in normal cases for this demo

		Matrix4 rot(
			dir_x.x, dir_y.x, dir_z.x, mid.x,
			dir_x.y, dir_y.y, dir_z.y, mid.y - down_factor * size,
			dir_x.z, dir_y.z, dir_z.z, mid.z,
			0, 0, 0, 1
		);

		Matrix4 trans = rot * Matrix4::getScale(Vector3(len * 0.5f, size, 2));
		Matrix4 transform = ModelviewProjection * trans;

		Matrix4 reverse_transform = Matrix4::makeTrans(Vector3(0.5f, 0.0f, 0.5f)) * Matrix4::getScale(Vector3(0.125f * len, 0.0f, -0.5f)) * transform.inverse() * (Matrix4::makeTrans(Vector3(-1.0f, 1.0f, 0.0f)) * Matrix4::getScale(Vector3(2.0f / width, -2.0f / height, 1.0f)));

	};

private:
	Renderable *mRoad;
};

