//
//  BallObject.cpp
//  PixFu
//
//  Created by rodo on 06/03/2020.
//  Copyright © 2020 rodo. All rights reserved.
//

#include "BallObject.hpp"
#include "Utils.hpp"

namespace Pix {

	BallObject::BallObject(BallWorld *world, ObjectProperties_t &meta, ObjectLocation_t location)
			: Ball(world->CONFIG, meta, location) {

		if (meta.trajectory.splineId != NO_SPLINE) {
			pSpline = &world->map()->vecSplines[meta.trajectory.splineId];
		}

	}

	void BallObject::followSpline(float fElapsedTime) {

		constexpr float followspeed = 80;

		if (fPosition > pSpline->fTotalSplineLength) fPosition = 0;

		fPosition += fElapsedTime * followspeed * fPenalty;

		float offset = pSpline->GetNormalisedOffset(fPosition);

		Pix::sPoint2D splPoint = pSpline->GetSplinePoint(offset);
		glm::vec3 &posi = pos();

		float sqdist = (posi.x - splPoint.x) * (posi.x - splPoint.x) + (posi.z - splPoint.y) * (posi.z - splPoint.y);

		constexpr float THR_VERYFAR = 300 * 300;
		constexpr float THR_OUTRANGE = 1000 * 1000;
		constexpr float NEAR = 0.001;

		if (sqdist > THR_OUTRANGE) {
			posi.x = splPoint.x;
			posi.z = splPoint.y;
			LogV(TAG, "PLaced back ...");
			mLastPoint = splPoint;
		} else if (sqdist < THR_VERYFAR) {
			mLastPoint = splPoint;
		} else {
			if (fabs(posi.x - mLastPoint.x) < NEAR && fabs(posi.x - mLastPoint.x) < NEAR) {
				mLastPoint = splPoint;
			} else {
				fPosition -= fElapsedTime * followspeed * fPenalty;
				// wait for object to eventually arrive
				LogV(TAG, "Waiting ...");
			}
		}

		posi.x += (mLastPoint.x - posi.x) * CONFIG.trajectory.lerp * fElapsedTime;
		posi.z += (mLastPoint.y - posi.z) * CONFIG.trajectory.lerp * fElapsedTime;

		glm::vec3 &rota = rot();

		float arot = -pSpline->GetSplineAngle(offset);
		rota.y = arot;

		//		rota.y = (arot - rota.y) * LERPSPLINE * fElapsedTime;

	}

	void BallObject::process(World *world, float fTime) {

		Ball::process(world, fTime);

		if (fTime == NOTIME) {
			fTime = fSimTimeRemaining;
		}

		if (pSpline != nullptr) followSpline(fTime);
	}

}
