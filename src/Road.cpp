#include "Road.h"
#include <Ogre.h>
#include <cassert>
#include <cmath>

using namespace Ogre;

enum {
	POSITION_BINDING,
	TEXCOORD_BINDING
};

CRoad::CRoad(Ogre::SceneNode *pOwnerNode, Ogre::String Name, OperationType opType)
{
	initialize(opType, false);
	setMaterial("BaseWhiteNoLighting");
	mDirty = true;
	
	mTesselation = 1;
	mWidth = 5;

	mSceneManager = pOwnerNode->getCreator();
	mRoadMesh = mSceneManager->createManualObject(Name.append(" internal class"));
	pOwnerNode->attachObject(mRoadMesh);

}

CRoad::~CRoad()
{
	mSceneManager->destroyManualObject(mRoadMesh);
	mRoadMesh = NULL;
}

void CRoad::setOperationType(OperationType opType)
{
	mRenderOp.operationType = opType;
}

RenderOperation::OperationType CRoad::getOperationType() const
{
	return mRenderOp.operationType;
}

void CRoad::addPoint(const Vector3 &p)
{
	mPoints.addPoint(p);
	mDirty = true;
}
void CRoad::addPoint(Real x, Real y, Real z)
{
	mPoints.addPoint(Vector3(x,y,z));
	mDirty = true;
}
const Vector3& CRoad::getPoint(unsigned short index) const
{
	assert(index < mPoints.getNumPoints() && "Point index is out of bounds!!");
	return mPoints.getPoint(index);
}
unsigned short CRoad::getNumPoints(void) const
{
	return (unsigned short)mPoints.getNumPoints();
}
void CRoad::setPoint(unsigned short index, const Vector3 &value)
{
	assert(index < mPoints.getNumPoints() && "Point index is out of bounds!!");

	mPoints.updatePoint(index, value);
	mDirty = true;
}
void CRoad::clear()
{
	mPoints.clear();
	mDirty = true;
}

void CRoad::update(HeightFunction *pHeightFunction)
{
	mHeightFunction = pHeightFunction;
	if (mDirty) 
	{
		tesselate();
		fillHardwareBuffers();
	}
}

void CRoad::tesselate()
{
	mCurve.clear();

	if(mPoints.getNumPoints() > 2)
	{
		for (int iPointIndex = 0; iPointIndex < mPoints.getNumPoints() - 1; iPointIndex++)
		{
			Ogre::Real fDistance = (mPoints.getPoint(iPointIndex + 1) - mPoints.getPoint(iPointIndex)).length();

			int iDivisions = (int)(fDistance / mTesselation);

			for (int iTess = 0; iTess < iDivisions; iTess++)
			{
				Real Time = ((Ogre::Real)iTess) * (((Ogre::Real)1.0f) / (Ogre::Real)(iDivisions));
				Vector3 TesselatedPoint = mPoints.interpolate(iPointIndex, Time);

				// Get height at the XZ location and change node' Y value (example of camera walk - walk height is 0 to be exactly on the terrain surface)
				Real CurveY = 0 + (*mHeightFunction)(getParentNode()->_getDerivedPosition().x + TesselatedPoint.x, getParentNode()->_getDerivedPosition().z + TesselatedPoint.z, 0);

				Vector3 CurvePosition = Vector3(TesselatedPoint.x, CurveY - getParentNode()->_getDerivedPosition().y, TesselatedPoint.z);
				mCurve.push_back(CurvePosition);
			}
		}
	}
	else
	{
		for (int iIndex = 0; iIndex < mPoints.getNumPoints(); iIndex++)
			mCurve.push_back(mPoints.getPoint(iIndex));
	}
}

void CRoad::createVertexDeclaration()
{
	VertexDeclaration *decl = mRenderOp.vertexData->vertexDeclaration;
	decl->addElement(POSITION_BINDING, 0, VET_FLOAT3, VES_POSITION);
}

void CRoad::fillHardwareBuffers()
{
	int size = mCurve.size();

	prepareHardwareBuffers(size,0);

	if (!size) { 
		mBox.setExtents(Vector3::ZERO,Vector3::ZERO);
		mDirty=false;
		return;
	}

	Vector3 vaabMin = mCurve[0];
	Vector3 vaabMax = mCurve[0];

	HardwareVertexBufferSharedPtr vbuf =
		mRenderOp.vertexData->vertexBufferBinding->getBuffer(0);
	mRoadMesh->clear();

	Real *prPos = static_cast<Real*>(vbuf->lock(HardwareBuffer::HBL_DISCARD));
	{
		mRoadMesh->begin("Road/RoadMaterial", RenderOperation::OT_LINE_LIST);

		for(int i = 0; i < size; i++)
		{
			*prPos++ = mCurve[i].x;
			*prPos++ = mCurve[i].y;
			*prPos++ = mCurve[i].z;

			if(mCurve[i].x < vaabMin.x)
				vaabMin.x = mCurve[i].x;
			if(mCurve[i].y < vaabMin.y)
				vaabMin.y = mCurve[i].y;
			if(mCurve[i].z < vaabMin.z)
				vaabMin.z = mCurve[i].z;

			if(mCurve[i].x > vaabMax.x)
				vaabMax.x = mCurve[i].x;
			if(mCurve[i].y > vaabMax.y)
				vaabMax.y = mCurve[i].y;
			if(mCurve[i].z > vaabMax.z)
				vaabMax.z = mCurve[i].z;

			if(i < (size - 1))
			{
				Vector3 Line = mCurve[i + 1] - mCurve[i];
				Line.normalise();

				// Terrain is XZ plane, so we're basically dealing with 2D vectors
				Vector3 TopLeft = mCurve[i] + Ogre::Vector3(-Line.z, Line.y, Line.x) * mWidth;
				Vector3 BottomLeft = mCurve[i] + Ogre::Vector3(Line.z, Line.y, -Line.x) * mWidth;

				Vector3 TopRight = mCurve[i + 1] + Ogre::Vector3(-Line.x, Line.y, Line.z) * mWidth;
				Vector3 BottomRight = mCurve[i + 1] + Ogre::Vector3(Line.x, Line.y, -Line.z) * mWidth;
						
				Real PointY = 0 + (*mHeightFunction)(getParentNode()->_getDerivedPosition().x + TopLeft.x, getParentNode()->_getDerivedPosition().z + TopLeft.z, 0);
				TopLeft.y = PointY - getParentNode()->_getDerivedPosition().y;

				PointY = 0 + (*mHeightFunction)(getParentNode()->_getDerivedPosition().x + BottomLeft.x, getParentNode()->_getDerivedPosition().z + BottomLeft.z, 0);
				BottomLeft.y = PointY - getParentNode()->_getDerivedPosition().y;

				PointY = 0 + (*mHeightFunction)(getParentNode()->_getDerivedPosition().x + TopRight.x, getParentNode()->_getDerivedPosition().z + TopRight.z, 0);
				TopRight.y = PointY - getParentNode()->_getDerivedPosition().y;

				PointY = 0 + (*mHeightFunction)(getParentNode()->_getDerivedPosition().x + BottomRight.x, getParentNode()->_getDerivedPosition().z + BottomRight.z, 0);
				BottomRight.y = PointY - getParentNode()->_getDerivedPosition().y;

				Vector3 Up = (TopLeft - mCurve[i]).normalisedCopy(), Down = (BottomLeft - mCurve[i]).normalisedCopy();

				for (int iWidthTesselation = 0; iWidthTesselation < 10; iWidthTesselation++)
				{
					Real fLength = Ogre::Real(iWidthTesselation) * mWidth / 5.0f;
					Vector3 Position = mCurve[i] + Up * fLength;

					Real PointY =(*mHeightFunction)(getParentNode()->_getDerivedPosition().x + Position.x, getParentNode()->_getDerivedPosition().z + Position.z, 0);
					Position.y = PointY - getParentNode()->_getDerivedPosition().y;

					mRoadMesh->position(Position);
				}

				for (int iWidthTesselation = 0; iWidthTesselation < 10; iWidthTesselation++)
				{
					Real fLength = Ogre::Real(iWidthTesselation) * mWidth / 5.0f;
					Vector3 Position = mCurve[i] + Down * fLength;

					Real PointY =(*mHeightFunction)(getParentNode()->_getDerivedPosition().x + Position.x, getParentNode()->_getDerivedPosition().z + Position.z, 0);
					Position.y = PointY - getParentNode()->_getDerivedPosition().y;

					mRoadMesh->position(Position);
				}
			}
		}

		mRoadMesh->end();
	}
	vbuf->unlock();

	mBox.setExtents(vaabMin, vaabMax);

	mDirty = false;
}
