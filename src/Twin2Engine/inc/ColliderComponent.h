#pragma once

#include "Component.h"
#include <vector>

namespace Twin2EngineCore {
	class ColliderComponent;
	std::vector<ColliderComponent*> colliderComponents = std::vector<ColliderComponent*>();

	class ColliderComponent : public Component {
	protected:
		ColliderComponent(); // Powoduje �e klasa jest jakby abstrakcyjna no chyba �e b�dzie dziedziczona
	public:
		virtual ~ColliderComponent();
	};
}