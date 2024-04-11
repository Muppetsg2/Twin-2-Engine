#pragma once

#include <core/Component.h>
#include <graphic/Window.h>

namespace Twin2Engine::Core {
	class RenderableComponent : public Component {
	private:			

		CloneFunctionStart(RenderableComponent, Component)
			CloneField(_isTransparent)
		CloneFunctionEnd()

		bool _isTransparent = false;
	protected:
		RenderableComponent(); // Powoduje �e klasa jest jakby abstrakcyjna no chyba �e b�dzie dziedziczona
	public:
		virtual ~RenderableComponent();

		virtual void Render();

		bool IsTransparent() const;
		void SetIsTransparent(bool value);

		static std::vector<RenderableComponent*> _components;
	};
}