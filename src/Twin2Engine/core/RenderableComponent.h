#pragma once

#include "Component.h"
#include <vector>

namespace Twin2Engine {
	namespace Core {
		class RenderableComponent;

		static std::vector<RenderableComponent*> renderableComponents = std::vector<RenderableComponent*>();

		class RenderableComponent : public Component {
		protected:
			RenderableComponent(); // Powoduje ¿e klasa jest jakby abstrakcyjna no chyba ¿e bêdzie dziedziczona
		public:
			virtual ~RenderableComponent();

			virtual void Render();
		};
	}
}