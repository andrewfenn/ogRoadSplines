#ifndef ROAD_H
#define ROAD_H

#include "DynamicRenderable.h"
#include <OgreTerrainGroup.h>
#include <OgreSimpleSpline.h>
#include <vector>

using namespace Ogre;

#pragma once
class CRoad : public DynamicRenderable
{
	typedef Ogre::Real (HeightFunction)(Ogre::Real x, Ogre::Real z, void *userData);
	typedef Ogre::Vector3 Vector3;
	typedef Ogre::Quaternion Quaternion;
	typedef Ogre::Camera Camera;
	typedef Ogre::Real Real;
	typedef Ogre::RenderOperation::OperationType OperationType;

public:
	/// Constructor - see setOperationType() for description of argument.
	CRoad(Ogre::SceneNode *pOwnerNode, Ogre::String Name, OperationType opType=Ogre::RenderOperation::OT_LINE_STRIP);
	virtual ~CRoad();

	/// Add a point to the point list
	void addPoint(const Ogre::Vector3 &point);
	/// Add a point to the point list
	void addPoint(Real rX, Real rY, Real rZ);

	/// Change the location of an existing point in the point list
	void setPoint(unsigned short usIndex, const Vector3 &value);

	/// Return the location of an existing point in the point list
	const Vector3& getPoint(unsigned short index) const;

	/// Return the total number of points in the point list
	unsigned short getNumPoints() const;

	/// Remove all points from the point list
	void clear();

	/// Call this to update the hardware buffer after making changes.  
	void update(HeightFunction *pHeightFunction);

	/** Set the type of operation to draw with.
	* @param opType Can be one of 
	*    - RenderOperation::OT_LINE_STRIP
	*    - RenderOperation::OT_LINE_LIST
	*    - RenderOperation::OT_POINT_LIST
	*    - RenderOperation::OT_TRIANGLE_LIST
	*    - RenderOperation::OT_TRIANGLE_STRIP
	*    - RenderOperation::OT_TRIANGLE_FAN
	*    The default is OT_LINE_STRIP.
	*/
	void setOperationType(OperationType opType);
	OperationType getOperationType() const;

	void setTesselation(Ogre::Real fTesselation) {mTesselation = fTesselation; };
	const Ogre::Real& getTesselation() const { return mTesselation; };

	std::vector<Vector3> getCurve() { return mCurve; };

	void setWidth(Ogre::Real width) { mWidth = width; };
	const Ogre::Real& getWidth() { return mWidth; };
protected:
	void tesselate();
	/// Implementation DynamicRenderable, creates a simple vertex-only decl
	virtual void createVertexDeclaration();
	/// Implementation DynamicRenderable, pushes point list out to hardware memory
	virtual void fillHardwareBuffers();

private:
	HeightFunction	*mHeightFunction;
	Ogre::SimpleSpline mPoints;
	std::vector<Vector3> mCurve;
	Ogre::ManualObject *mRoadMesh;
	Ogre::Real mTesselation;
	Ogre::Real mWidth;
	Ogre::SceneManager *mSceneManager;
	
	bool mDirty;
};

#endif
