#pragma once

enum class Layer : uint8_t {
	DEFAULT = 0,
	IGNORE_RAYCAST = 1,
	IGNORE_COLLISION = 2,
	UI = 4,
	LAYER_1 = 8,
	LAYER_2 = 16,
	LAYER_3 = 32,
	LAYER_4 = 64
};

//system nigdy nie bêdzie sprawdza³ kolizji z obiektami w warstwi UI, nawet jeœli filtry wskazywaæ bêd¹ inaczej (chyba, ¿e podczas raycast?)
enum class CollisionMode : uint8_t {
	IGNORING = 0,
	NEUTRAL = 1,
	ACTIVE = 2
};

struct LayerCollisionFilter {
	CollisionMode DEFAULT : 2;
	CollisionMode IGNORE_RAYCAST : 2;
	CollisionMode IGNORE_COLLISION : 2;
	CollisionMode UI : 2;
	CollisionMode LAYER_1 : 2;
	CollisionMode LAYER_2 : 2;
	CollisionMode LAYER_3 : 2;
	CollisionMode LAYER_4 : 2;
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
static LayerCollisionFilter DEFAULT_LAYERS_FILTER{ CollisionMode::ACTIVE, CollisionMode::ACTIVE, CollisionMode::NEUTRAL, CollisionMode::IGNORING, CollisionMode::ACTIVE, CollisionMode::ACTIVE, CollisionMode::ACTIVE, CollisionMode::ACTIVE };

static LayerCollisionFilter GLOBAL_LAYERS_FILTERS[] = {
													{ CollisionMode::ACTIVE,	CollisionMode::ACTIVE,		CollisionMode::NEUTRAL,		CollisionMode::IGNORING, CollisionMode::ACTIVE,		CollisionMode::ACTIVE,		CollisionMode::ACTIVE,		CollisionMode::ACTIVE	},
													{ CollisionMode::ACTIVE,	CollisionMode::ACTIVE,		CollisionMode::NEUTRAL,		CollisionMode::IGNORING, CollisionMode::ACTIVE,		CollisionMode::ACTIVE,		CollisionMode::ACTIVE,		CollisionMode::ACTIVE	},
													{ CollisionMode::ACTIVE,	CollisionMode::ACTIVE,		CollisionMode::NEUTRAL,		CollisionMode::IGNORING, CollisionMode::ACTIVE,		CollisionMode::ACTIVE,		CollisionMode::ACTIVE,		CollisionMode::ACTIVE	},
													{ CollisionMode::IGNORING,	CollisionMode::IGNORING,	CollisionMode::IGNORING,	CollisionMode::IGNORING, CollisionMode::IGNORING,	CollisionMode::IGNORING,	CollisionMode::IGNORING,	CollisionMode::IGNORING },
													{ CollisionMode::ACTIVE,	CollisionMode::ACTIVE,		CollisionMode::NEUTRAL,		CollisionMode::IGNORING, CollisionMode::ACTIVE,		CollisionMode::ACTIVE,		CollisionMode::ACTIVE,		CollisionMode::ACTIVE	},
													{ CollisionMode::ACTIVE,	CollisionMode::ACTIVE,		CollisionMode::NEUTRAL,		CollisionMode::IGNORING, CollisionMode::ACTIVE,		CollisionMode::ACTIVE,		CollisionMode::ACTIVE,		CollisionMode::ACTIVE	},
													{ CollisionMode::ACTIVE,	CollisionMode::ACTIVE,		CollisionMode::NEUTRAL,		CollisionMode::IGNORING, CollisionMode::ACTIVE,		CollisionMode::ACTIVE,		CollisionMode::ACTIVE,		CollisionMode::ACTIVE	},
													{ CollisionMode::ACTIVE,	CollisionMode::ACTIVE,		CollisionMode::NEUTRAL,		CollisionMode::IGNORING, CollisionMode::ACTIVE,		CollisionMode::ACTIVE,		CollisionMode::ACTIVE,		CollisionMode::ACTIVE	}
};

namespace YAML {
	template<> struct convert<CollisionMode> {
		static Node encode(const CollisionMode& rhs) {
			Node node;
			node = (size_t)rhs;
			return node;
		}

		static bool decode(const Node& node, CollisionMode& rhs) {
			if (!node.IsScalar()) return false;
			rhs = (CollisionMode)node.as<size_t>();
			return true;
		}
	};

	template<> struct convert<LayerCollisionFilter> {
		static Node encode(const LayerCollisionFilter& rhs) {
			Node node;
			node.push_back(rhs.DEFAULT);
			node.push_back(rhs.IGNORE_RAYCAST);
			node.push_back(rhs.IGNORE_COLLISION);
			node.push_back(rhs.UI);
			node.push_back(rhs.LAYER_1);
			node.push_back(rhs.LAYER_2);
			node.push_back(rhs.LAYER_3);
			node.push_back(rhs.LAYER_4);
			return node;
		}

		static bool decode(const Node& node, LayerCollisionFilter& rhs) {
			if (!node.IsSequence()) return false;

			rhs.DEFAULT = node[0].as<CollisionMode>();
			rhs.IGNORE_RAYCAST = node[1].as<CollisionMode>();
			rhs.IGNORE_COLLISION = node[2].as<CollisionMode>();
			rhs.UI = node[3].as<CollisionMode>();
			rhs.LAYER_1 = node[4].as<CollisionMode>();
			rhs.LAYER_2 = node[5].as<CollisionMode>();
			rhs.LAYER_3 = node[6].as<CollisionMode>();
			rhs.LAYER_4 = node[7].as<CollisionMode>();
			return true;
		}
	};

	template<> struct convert<Layer> {
		static Node encode(const Layer& rhs) {
			Node node;
			node = (size_t)rhs;
			return node;
		}

		static bool decode(const Node& node, Layer& rhs) {
			if (!node.IsScalar()) return false;
			rhs = (Layer)node.as<size_t>();
			return true;
		}
	};
}