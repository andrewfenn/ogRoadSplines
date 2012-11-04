/*/////////////////////////////////////////////////////////////////////////////////
/// Copyright (c) 2010 Andrew Fenn <andrewfenn@gmail.com> and the Ogitor Team
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

#include "RoadRenderListener.h"

#include <OgreStringConverter.h>
#include <OgreRenderSystem.h>
#include <OgreCommon.h>

void RoadRenderListener::notifyRenderSingleObject(Renderable* rend, 
                                                           const Pass* pass,
                                          const AutoParamDataSource* source, 
                                    			const LightList* pLightList,
                                            bool suppressRenderStateChanges)
{
	if(mRoad != NULL && mRoad == rend)
	{
		Ogre::Matrix4 Transform, ReverseTransform, ModelvewProjection;
		getTransformMatrices(ModelvewProjection, &Transform, &ReverseTransform, Ogre::Vector3::ZERO, Ogre::Vector3::ZERO);
		Ogre::RenderSystem *pSystem = Ogre::Root::getSingletonPtr()->getRenderSystem();

		if(pass->getIndex() == 0)
		{
			pSystem->_setCullingMode(CULL_NONE);
			pSystem->_setDepthBufferParams(true, false, CMPF_LESS_EQUAL);
			pSystem->setStencilCheckEnabled(true);

			pSystem->setStencilBufferParams(CMPF_ALWAYS_PASS, 0xFF, 0xFF, SOP_KEEP, SOP_INCREMENT,  SOP_KEEP, true);

            return;
			pass->getVertexProgram()->setParameter("transform", Ogre::StringConverter::toString(Transform));
		}
		else if(pass->getIndex() == 1)
		{
			pSystem->_setCullingMode(CULL_ANTICLOCKWISE);
			pSystem->_setDepthBufferParams(false, false, CMPF_LESS_EQUAL);
			pSystem->setStencilCheckEnabled(true);
			
			pSystem->_setSceneBlending(SBF_DEST_ALPHA, SBF_ONE_MINUS_SOURCE_ALPHA);
			pSystem->setStencilBufferParams(CMPF_NOT_EQUAL, 0xFF, 0xFF, SOP_KEEP, SOP_KEEP, SOP_ZERO, false);

            if (pass->getVertexProgram().isNull())
                return;

			pass->getVertexProgram()->setParameter("transform", Ogre::StringConverter::toString(Transform));
			pass->getVertexProgram()->setParameter("reverse_transform", Ogre::StringConverter::toString(ReverseTransform));
		}
	}
}

void RoadRenderListener::getTransformMatrices(Matrix4 ModelviewProjection, Matrix4 *pTransform, Matrix4 *pReverseTransform, Vector3 v0, Vector3 v1)
{
	Real len = v0.distance(v1);
	Vector3 mid = 0.5f * (v0 + v1);

	Vector3 dir_x = (v1 - v0).normalisedCopy();
	Vector3 dir_z = dir_x.crossProduct(Ogre::Vector3::UNIT_Y).normalisedCopy();
	Vector3 dir_y = dir_z.crossProduct(dir_x).normalisedCopy();

	const float size = 30.0f; // Relatively large number, ideally should be computed to be only as big as necessary for a road segment
	const float down_factor = 0.3f; // Move the box down a little to better use the box volume in normal cases for this demo

    const int width = 10;
    const int height = 10;

	Matrix4 rot(
		dir_x.x, dir_y.x, dir_z.x, mid.x,
		dir_x.y, dir_y.y, dir_z.y, mid.y - down_factor * size,
		dir_x.z, dir_y.z, dir_z.z, mid.z,
		0, 0, 0, 1
	);

	Matrix4 trans = rot * Matrix4::getScale(Vector3(len * 0.5f, size, 2));
	Matrix4 transform = ModelviewProjection * trans;

	Matrix4 reverse_transform, temp;
    reverse_transform.makeTrans(Ogre::Vector3(0.5f, 0.0f, 0.5f));
    temp.makeTrans(Vector3(-1.0f, 1.0f, 0.0f));
    reverse_transform = reverse_transform * Matrix4::getScale(Vector3(0.125f * len, 0.0f, -0.5f)) * transform.inverse() * (temp * Matrix4::getScale(Vector3(2.0f / width, -2.0f / height, 1.0f)));

}
