#pragma once

#include <core/Component.h>
#include <core/Window.h>

namespace Twin2Engine::Core {
	class RenderableComponent : public Component {
	private:
		bool _isTransparent = false;
	protected:
		RenderableComponent() = default; // Powoduje �e klasa jest jakby abstrakcyjna no chyba �e b�dzie dziedziczona
	public:
		virtual ~RenderableComponent() = default;

		virtual void Render(const Window* window);

		bool IsTransparent() const;
		void SetIsTransparent(bool value);
	};
}