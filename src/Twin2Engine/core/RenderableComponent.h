#pragma once

#include <core/Component.h>
#include <vector>

namespace Twin2Engine {
	namespace Core {
		class RenderableComponent : public Component {
		private:
			bool _isTransparent = false;
		protected:
			RenderableComponent(); // Powoduje �e klasa jest jakby abstrakcyjna no chyba �e b�dzie dziedziczona
		public:
			virtual ~RenderableComponent();

			virtual void Render();

			bool IsTransparent() const;
			void SetIsTransparent(bool value);

			static std::vector<RenderableComponent*> renderableComponents;
		};
	}
}