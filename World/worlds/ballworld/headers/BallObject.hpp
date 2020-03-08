//
// Created by rodo on 2020-03-06.
//

#ifndef PIXFU_ANDROID_LIB_BALLOBJECT_HPP
#define PIXFU_ANDROID_LIB_BALLOBJECT_HPP

#include "../../../geom/Splines.hpp"
#include "Ball.hpp"
#include "../../../core/headers/WorldMeta.hpp"

namespace Pix {

	class BallObject:public Ball {

		Spline_t *pSpline;
		float fPos;

	public:
		BallObject(const WorldConfig_t &planetConfig, ObjectProperties_t &meta, const ObjectLocation_t &location, int overrideId)
		:Ball(planetConfig, meta, location, overrideId) {}

	private:
		void driveSpline(float fElapsedTime);
	};

	void BallObject::driveSpline(float fElapsedTime) {

		if (fPos > pSpline->fTotalSplineLength) fPos = 0;

		fPos += fElapsedTime;

		Pix::sPoint2D splPoint = pSpline->GetSplinePoint(fPos);

		glm::vec3 &pos = pHumanPlayer->pos();
		pos.x = splPoint.x; //1000;
		pos.z = splPoint.y; //1000;

		pHumanPlayer->rot().y = -spl.GetSplineAngle(fPos);

	}
BallObject::BallObject(const WorldConfig_t & planetConfig, ObjectProperties_t & meta, const ObjectLocation_t & location, int overrideId):Ball(planetConfig,meta,location,overrideId){}

}
