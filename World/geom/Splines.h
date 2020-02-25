
//
//  Splines.h
//  (c) javid onelonecoder.com//
//

#pragma once

#include <iostream>
#include <string>
#include <cmath>
#include <fstream>

#include "PixFu.hpp"
#include "Common.h"
#include "Canvas2D.hpp"
#include "Utils.hpp"


struct sPoint3D
{
	float x;
	float y;
	float z;
};

struct sPoint2D
{
	float x;
	float y;
	float length;				// spline segment length
	
	inline float module() {
		return sqrt(x*x+y*y);
	}

	inline float distance(sPoint2D p) {
		return sqrt((x-p.x)*(x-p.x)+(y-p.y)*(y-p.y));
	}

	inline void translate (sPoint2D where)  {
		x += where.x;
		y += where.y;
	}

	inline void rotate (float angle, sPoint2D orig={0,0})  {
		float c = cosf(angle), s = sinf(angle);
		translate({-orig.x, -orig.y});
		x = x * c - y * s;
		y = x * s + y * c;
		translate({orig.x, orig.y});
	}
	
	inline sPoint2D rotated (float angle, sPoint2D orig={0,0})  {
		sPoint2D p = translated({-orig.x, -orig.y});
		float c = cosf(angle), s = sinf(angle);
		p= {p.x * c - p.y * s, p.x * s + p.y * c};
		p.translate(orig);
		return p;
	}

	inline sPoint2D translated(sPoint2D where) {
		return {x+where.x, y+where.y};
	}

	inline sPoint2D scaled(float s) {
		return {x*s, y*s};
	}
};

struct sLineSegment
{
	float sx, sy;
	float ex, ey;
	float radius;
	
	inline float angle() { return atan2(ey-sy, ex-sx);}

	inline sLineSegment scaled(float s) { return { sx*s,sy*s,ex*s,ey*s, radius}; }
	inline sLineSegment translated(float x,float y) { return { sx+x,sy+y,ex+x,ey+y, radius}; }
	inline sLineSegment rotated(float rads) {
		// rotation of points about the orgin using http://en.wikipedia.org/wiki/Transformation_matrix#Rotation
		float c = cosf(rads), s = sinf(rads);
		return { sx*c-sy*s, sy*c+sx*s, ex*c- ey*s, ey*c + ex*s };
	}

	inline void scale(float s) { sx*=s; sy*=s; ex*=s; ey*=s; }
	inline void translate(float x,float y) { sx+=x; sy+=y; ex+=x; ey+=y; }
	inline void rotate(float rads) {
		// rotation of points about the orgin using http://en.wikipedia.org/wiki/Transformation_matrix#Rotation
		float c = cosf(rads), s = sinf(rads);
		sx = sx*c - sy*s; sy = sy*c + sx*s;
		ex = ex*c - ey*s; ey = ey*c + ex*s;
	}
};

typedef struct sNearCheckpoints {
	bool near = false;
	int wrongway = 0;
	float mindistance = 0;
	int point;
	sPoint2D rawPoint;
} NearCheckpoints_t;

class WalkableLine
{
	sPoint2D start;
	sPoint2D end;
	
	float cosAngle, sinAngle, angle, length;
	
private:
	void calculate() {
		angle = atan2(end.y-start.y, end.x-start.x);
		sinAngle = sinf(angle);
		cosAngle = cosf(angle);
		length = sqrt((start.x-end.x)*(start.x-end.x)+(start.y-end.y)*(start.y-end.y));
	}
	
public:
	WalkableLine(sPoint2D s, sPoint2D e) {
		start=s;
		end=e;
		calculate();

	}
	
	bool isInTrack(float position) {
		return position>0 && position < length;
	}
	
	bool reset(sPoint2D startPoint) {
		if (start.x != startPoint.x && start.y != startPoint.y) {
			start = startPoint;
			calculate();
			return true;
		} else return false;
	}
	float getAngle() {
		return angle;
	}
	
	float getLength() {
		return length;
	}
	
	sPoint2D getPoint(float lenstep) {
		return { lenstep*cosAngle + start.x, lenstep*sinAngle+start.y };
	}
	void DrawSelf(rgl::Canvas2D *gfx, rgl::Pixel col = 0x000F) {
		gfx->drawLine(start.x, start.y, end.x, end.y, col);
	}
	
	
};

struct sSpline
{
	std::vector<sPoint2D> points;
	float fTotalSplineLength = 0.0f;
	bool bIsLooped = true;
	
	bool IsInTrack(float t) {
		int p0, p1, p2, p3;
		if (!bIsLooped) {
			p1 = (int)t + 1;
			p0 = p1 - 1;
			p2 = p1 + 1;
			p3 = p2 + 1;
			return p1<points.size();
		}
		else return true;
		
	}
	sPoint2D GetControlPoint(int index) {
		return points.at(index);
	}
	
	float GetPointPosition(int index) {
		float acum=0;
		for (int i = 0; i<index; i++) acum+=points.at(index).length;
		return acum;
	}

	int GetTotalControlPoints() {
		return (int)points.size();
	}

	void NearControlPoint(int index, sPoint2D target, float range, NearCheckpoints_t *nearInfo) {
		
		//from, sPoint2D input, float range) {
		auto distance = [] (sPoint2D orig, sPoint2D to) {
			return sqrt((orig.x-to.x)*(orig.x-to.x)+(orig.y-to.y)*(orig.y-to.y));
		};
		
		auto near = [distance] (sPoint2D p1, sPoint2D p2, float range) {
			float d =distance(p1, p2);
			return d < range;
		};
		
		sPoint2D targetPoint = points.at(index);
		float d =distance(targetPoint, target);

		if (nearInfo->mindistance > d || nearInfo->point != index || nearInfo->mindistance == 0) {
			// correct way
			nearInfo->wrongway--;
			if (nearInfo->wrongway<0) nearInfo->wrongway = 0;
			nearInfo->mindistance = d;
			nearInfo->point = index;
//			std::cout << "mind " <<d <<std::endl;
		} else {

//			std::cout << "wway " <<nearInfo->wrongway <<std::endl;
			nearInfo->wrongway++;
			if (nearInfo->wrongway>100) nearInfo->wrongway = 100;
		}
		nearInfo->near = near(points.at(index), target, range);
		nearInfo->rawPoint = targetPoint;

//		if (nearInfo->near)
//			if (rgl::DBG) rgl::LogV(TAG, rgl::SF("player near cp %d", index));
	}
	
	sPoint2D GetSplinePoint(float t)
	{
		int p0, p1, p2, p3;
		if (!bIsLooped)
		{
			p1 = (int)t + 1;
			p2 = p1 + 1;
			p3 = p2 + 1;
			p0 = p1 - 1;
			
			if (p2 >= points.size()) p2=(int)points.size()-1;
			if (p3 >= points.size()) p3=(int)points.size()-1;
			if (p1 >= points.size()) p1=(int)points.size()-1;
			if (p0 == -1) p0 = 0;
			
		}
		else
		{
			p1 = ((int)t) % points.size();
			p2 = (p1 + 1) % points.size();
			p3 = (p2 + 1) % points.size();
			p0 = p1 >= 1 ? p1 - 1 : (int)points.size() - 1;
		}
		
		t = t - (int)t;
		
		float tt = t * t;
		float ttt = tt * t;
		
		float q1 = -ttt + 2.0f*tt - t;
		float q2 = 3.0f*ttt - 5.0f*tt + 2.0f;
		float q3 = -3.0f*ttt + 4.0f*tt + t;
		float q4 = ttt - tt;
		
		float tx = 0.5f * (points[p0].x * q1 + points[p1].x * q2 + points[p2].x * q3 + points[p3].x * q4);
		float ty = 0.5f * (points[p0].y * q1 + points[p1].y * q2 + points[p2].y * q3 + points[p3].y * q4);
		
		return{ tx, ty };
	}
	
	float GetSplineAngle(float t) {
		sPoint2D p = GetSplineGradient(t);
		return atan2(p.y, p.x);
	}
	
	sPoint2D GetSplineGradient(float t)
	{
		int p0, p1, p2, p3;
		if (!bIsLooped)
		{
			p1 = (int)t + 1;
			p2 = p1 + 1;
			p3 = p2 + 1;
			p0 = p1 - 1;
		}
		else
		{
			p1 = ((int)t) % points.size();
			p2 = (p1 + 1) % points.size();
			p3 = (p2 + 1) % points.size();
			p0 = p1 >= 1 ? p1 - 1 : (int)points.size() - 1;
		}
		
		t = t - (int)t;
		
		float tt = t * t;
		float ttt = tt * t;
		
		float q1 = -3.0f * tt + 4.0f*t - 1.0f;
		float q2 = 9.0f*tt - 10.0f*t;
		float q3 = -9.0f*tt + 8.0f*t + 1.0f;
		float q4 = 3.0f*tt - 2.0f*t;
		
		float tx = 0.5f * (points[p0].x * q1 + points[p1].x * q2 + points[p2].x * q3 + points[p3].x * q4);
		float ty = 0.5f * (points[p0].y * q1 + points[p1].y * q2 + points[p2].y * q3 + points[p3].y * q4);
		
		return{ tx, ty };
	}
	
	float CalculateSegmentLength(int node)
	{
		float fLength = 0.0f;
		float fStepSize = 0.1;
		
		sPoint2D old_point, new_point;
		old_point = GetSplinePoint((float)node);
		
		for (float t = 0; t < 1.0f; t += fStepSize)
		{
			new_point = GetSplinePoint((float)node + t);
			fLength += sqrtf((new_point.x - old_point.x)*(new_point.x - old_point.x)
							 + (new_point.y - old_point.y)*(new_point.y - old_point.y));
			old_point = new_point;
		}
		
		return fLength;
	}
	
	float GetNormalisedOffset(float p)
	{
		// Which node is the base?
		int i = 0;
		while (p > points[i].length)
		{
			p -= points[i].length;
			i++;
		}
		
		// The fractional is the offset
		return (float)i + (p / points[i].length);
	}
	
	bool insertPointInterpolating(int previousPoint, bool update = false) {
		if (previousPoint >= 0 && previousPoint<points.size()) {
			sPoint2D before = points.at(previousPoint);
			sPoint2D after=points.at((previousPoint+1) % points.size());
			sPoint2D mid = { 0.5f*(after.x+before.x), 0.5f*(after.y + before.y)};
			return insertPoint(previousPoint, mid, update) != nullptr;
		}
		return false;
	}

	sPoint2D const *insertPoint(int previousPoint, sPoint2D newPoint, bool update = false) {
		if (previousPoint >= 0 && previousPoint<points.size()) {
			points.insert(points.begin() + previousPoint + 1, newPoint);
			if (update) UpdateSplineProperties();
			return &points.at(previousPoint + 1);
		}
		return nullptr;
	}
	
	void deletePoint(int previousPoint, bool update = true) {
		if (previousPoint >= 0 && previousPoint<points.size()) {
			points.erase(points.begin() + previousPoint);
			if (update) UpdateSplineProperties();
		}
	}

	void UpdateSplineProperties()
	{
		// Use to cache local spline lengths and overall spline length
		fTotalSplineLength = 0.0f;
		
		if (bIsLooped)
		{
			// Each node has a succeeding length
			for (int i = 0; i < points.size(); i++)
			{
				points[i].length = CalculateSegmentLength(i);
				fTotalSplineLength += points[i].length;
			}
		}
		else
		{
			auto size = points.size();
			if (size < 5) {
				for (long i=size; i<5; i++) {
					points.insert(points.begin()+(i-size+1),{(points[i].x+points[i+1].x)/2,(points[i].y+points[i+1].y)/2});
				}
			}
			
			//			for (int i = 1; i < points.size() - 2; i++)
			for (int i = 0; i < points.size() - 1; i++)
			{
				points[i].length = CalculateSegmentLength(i);
				fTotalSplineLength += points[i].length;
			}
		}
	}
	
	void DrawSelf(rgl::Canvas2D *gfx, float ox, float oy, rgl::Pixel col = 0x000F, float scale = 1.0f, bool drawPoints = false, int currentPoint = -1, rgl::Pixel colProgress = 0, float fTime = 0)
	{
		if (colProgress.n==0) colProgress = col;
		
		if (bIsLooped)
		{
			for (float t = 0; t < (float)points.size() - 0; t += 0.005f)
			{
				sPoint2D pos = GetSplinePoint(t);
				
				gfx->setPixel(ox+pos.x * scale, oy+pos.y * scale, t<currentPoint-1?colProgress:col);
			}
		}
		else // Not Looped
		{
			for (float t = 0; t < (float)points.size() - 0; t += 0.005f)
			{
				sPoint2D pos = GetSplinePoint(t);
				gfx->setPixel(ox+pos.x * scale, oy+pos.y * scale, col);
			}
		}
		
		if (drawPoints) DrawPoints(gfx, ox, oy, currentPoint, colProgress, scale, fTime);
	}
	
	void DrawPoints(rgl::Canvas2D *gfx, float ox, float oy, int currentPoint = -1, rgl::Pixel colProgress = 0, float scale = 1.0f, float fTime = 0, bool noPast = false) {
		for (int  l =0; l<points.size();l++) {
			if (noPast && l<currentPoint - 1) continue;
			sPoint2D i = points.at(l);
			rgl::Pixel curr = fTime<0.5?rgl::Colors::YELLOW:rgl::Colors::BLACK;
			gfx->fillRect(ox+i.x * scale - 2, oy+i.y*scale - 2, 4, 4, l<currentPoint-1?rgl::Colors::GREEN.scale(0.5) : l==currentPoint-1 ? curr : rgl::Colors::RED.scale(0.5));
		}
	}
	
	void DrawSprite(rgl::Drawable *gfx, float ox, float oy, rgl::Pixel col = 0x000F)
	{
		if (bIsLooped)
		{
			for (float t = 0; t < (float)points.size() - 0; t += 0.005f)
			{
				sPoint2D pos = GetSplinePoint(t);
				gfx->setPixel(pos.x, pos.y, col);
				gfx->setPixel(pos.x-1, pos.y, col);
				gfx->setPixel(pos.x, pos.y-1, col);
				gfx->setPixel(pos.x-1, pos.y-1, col);
			}
		}
		else // Not Looped
		{
			for (float t = 0; t < (float)points.size() - 3; t += 0.005f)
			{
				sPoint2D pos = GetSplinePoint(t);
				gfx->setPixel(pos.x-1, pos.y, col);
				gfx->setPixel(pos.x, pos.y-1, col);
				gfx->setPixel(pos.x-1, pos.y-1, col);
			}
		}
	}
	bool SaveTp(std::string sFilename, int scaleFactor = 0)
	{
		std::ofstream *file = new std::ofstream(sFilename, std::ios::out | std::ios::binary);
		if (!file->is_open()) return false;
		return Save(file, scaleFactor);
	}
	
	bool Save(std::ofstream *file, int scaleFactor = 0)
	{
		if (!file->is_open()) return false;
		long s = points.size();
		file->write((char*)&s,sizeof(long));
		
		for (int x = 0; x < s; x++)
		{
			sPoint2D point = points.at(x);
			if (scaleFactor != 0) {
				point = { point.x*(1+scaleFactor), point.y*(1+scaleFactor) };
			}
			file->write( (char*)&point, sizeof(sPoint2D));
		}
		
		return true;
	}
	
	bool LoadFrom(std::string sFilename, int scaleFactor = 0)
	{
		std::ifstream *file = new std::ifstream(sFilename, std::ios::in | std::ios::binary);
		if (!file->is_open()) return false;
		return Load(file, scaleFactor);
	}
	
	void scale(float rawscale) {
		int k=0;
		for (sPoint2D point:points)
		{
			points[k++] = {point.x*rawscale,  point.y*rawscale, point.length*rawscale};
		}
		UpdateSplineProperties();
	}

	bool Load(std::ifstream *file, int scaleFactor = 0)
	{
		
		long n;
		file->read((char*)&n, sizeof(long));
		
		points.clear();
		float scale = 1/(1+scaleFactor);
		for (int x = 0; x < n; x++)
		{
			sPoint2D point;
			file->read( (char*)&point, sizeof(sPoint2D));
			points.push_back({point.x*scale, point.y*scale});
		}
		
		UpdateSplineProperties();
		
		return true;
	}
};
