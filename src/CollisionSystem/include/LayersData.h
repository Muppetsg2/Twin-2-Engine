#ifndef _LAYERSDATA_H_
#define _LAYERSDATA_H_

#include <cstdint>

enum Layer : uint8_t { DEFAULT, IGNORE_RAYCAST, IGNORE_COLLISION, UI, LAYER_1, LAYER_2, LAYER_3, LAYER_4 };


//system nigdy nie bêdzie sprawdza³ kolizji z obiektami w warstwi UI, nawet jeœli filtry wskazywaæ bêd¹ inaczej (chyba, ¿e podczas raycast?)
enum CollisionMode : uint8_t { IGNORING = 0, NEUTRAL = 1, ACTIVE = 3 };
struct LayerCollisionFilter {
	CollisionMode DEFAULT			: 2;
	CollisionMode IGNORE_RAYCAST	: 2;
	CollisionMode IGNORE_COLLISION	: 2;
	CollisionMode UI				: 2;
	CollisionMode LAYER_1			: 2;
	CollisionMode LAYER_2			: 2;
	CollisionMode LAYER_3			: 2;
	CollisionMode LAYER_4			: 2;
};
/*struct LayersFilter {
	bool DEFAULT : 1;
	bool IGNORE_RAYCAST : 1;
	bool IGNORE_COLLISION : 1;
	bool UI : 1;
	bool LAYER_1 : 1;
	bool LAYER_2 : 1;
	bool LAYER_3 : 1;
	bool LAYER_4 : 1;
};/**/

//const LayersFilter DEFAULT_LAYERS_FILTER { 1, 1, 0, 0, 1, 1, 1, 1 };
static LayerCollisionFilter DEFAULT_LAYERS_FILTER			{ ACTIVE, ACTIVE, NEUTRAL, IGNORING, ACTIVE, ACTIVE, ACTIVE, ACTIVE };

static LayerCollisionFilter GLOBAL_LAYERS_FILTERS[] = { 
													{ ACTIVE,	ACTIVE,		NEUTRAL,	IGNORING,		ACTIVE,		ACTIVE,		ACTIVE,		ACTIVE },
													{ ACTIVE,	ACTIVE,		NEUTRAL,	IGNORING,		ACTIVE,		ACTIVE,		ACTIVE,		ACTIVE },
													{ ACTIVE,	ACTIVE,		NEUTRAL,	IGNORING,		ACTIVE,		ACTIVE,		ACTIVE,		ACTIVE },
													{ IGNORING,	IGNORING,	IGNORING,	IGNORING,		IGNORING,	IGNORING,	IGNORING,	IGNORING },
													{ ACTIVE,	ACTIVE,		NEUTRAL,	IGNORING,		ACTIVE,		ACTIVE,		ACTIVE,		ACTIVE },
													{ ACTIVE,	ACTIVE,		NEUTRAL,	IGNORING,		ACTIVE,		ACTIVE,		ACTIVE,		ACTIVE },
													{ ACTIVE,	ACTIVE,		NEUTRAL,	IGNORING,		ACTIVE,		ACTIVE,		ACTIVE,		ACTIVE },
													{ ACTIVE,	ACTIVE,		NEUTRAL,	IGNORING,		ACTIVE,		ACTIVE,		ACTIVE,		ACTIVE }
												};

#endif // !_LAYERSDATA_H_
