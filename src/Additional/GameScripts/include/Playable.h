#pragma once

#include <core/Transform.h>
#include <core/GameObject.h>
#include <core/Component.h>

using namespace Twin2Engine::Core;

class PatronData;

class Playable : public Component {
public:
	PatronData* patron;
};
