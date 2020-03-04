//
//  GameObjects.h
//  LoneKart
//
//  Created by rodo on 19/01/2020.
//  Copyright © 2020 rodo. All rights reserved.
//

#pragma once

#define NOHEIGHT -1
#define NOMASS -1

#include <map>

namespace Pix {

// The Static Database of objects, defined in the Meta/ dir
// Provides methods to get Ground Sprites

	class GameObjectsDb {

		static const std::map<int, ObjectFeatures_t> MapGroundSprites;
		static const std::map<int, int> MapCircuitSprites;                    // 1000kg mass of the player car

	public:

//		static ObjectFeatures_t  const *getCircuitGroundSprite(int circuitObjectOid);

		static ObjectFeatures_t const *getGroundSprite(int groundObjectOid);

	};


const std::map<int, ObjectInfo_t> GameObjectsDb::MapGroundSprites = {
	// Name			oid    sprInfo    anlen  ansp, mass   ang  accl  height selfScale
	{TortugaVerde,	{1000, {1, 0,}, 	  8, 10.0, 500.0, 	0, 0.08, NOHEIGHT}},
	{TortugaRoja, 	{1001, {1, 10},		  8, 10.0, 1000, 	0, 0.12, NOHEIGHT}},
	{TortugaPincha, {1002, {1, 20}, 	  8, 10.0, 2000, 	0, 0.14, NOHEIGHT}},
	{Banana, 		{1003, {1, 30}, 	  1, 10.0, NOMASS}},
	{Ball,			{1004, {1, 33}, 	  1, 10.0, 500}},
	{Stone,			{1005, {1, 34}, 	  1, 10.0, 1000}},
	{QuestionBlock, {1006, {1, 56}, 	  4, 10.0, NOMASS,  0, 0, NOHEIGHT, 2.0}},
	{Tree1, 		{500,  {3, 8, 2, 2},  1, 10.0, 900000}},
	{Tree2, 		{501,  {3, 28, 1, 2}, 1, 10.0, 900000}},
	{Tree3, 		{502,  {3, 29, 1, 2}, 1, 10.0, 900000}}
};

}
