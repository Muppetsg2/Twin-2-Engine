#pragma once

#include <core/Component.h>
#include <vector>

namespace Twin2Engine {
	namespace Core {
		class RenderableComponent;

		//std::vector<RenderableComponent*> renderableComponents = std::vector<RenderableComponent*>();

		class RenderableComponent : public Component {
		private:
			bool _isTransparent = false;
		protected:
			RenderableComponent(); // Powoduje ¿e klasa jest jakby abstrakcyjna no chyba ¿e bêdzie dziedziczona
		public:
			virtual ~RenderableComponent();

			virtual void Render();

			bool IsTransparent() const;
			void SetIsTransparent(bool value);
		};
	}
}