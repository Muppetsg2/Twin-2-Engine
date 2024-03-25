#pragma once

#include "Component.h"
#include <vector>

namespace Twin2EngineCore {
	class RenderableComponent;

	//std::vector<RenderableComponent*> renderableComponents = std::vector<RenderableComponent*>();

	class RenderableComponent : public Component {
	protected:
		RenderableComponent(); // Powoduje �e klasa jest jakby abstrakcyjna no chyba �e b�dzie dziedziczona
	public:
		virtual ~RenderableComponent();

		virtual void Render();
	};
}