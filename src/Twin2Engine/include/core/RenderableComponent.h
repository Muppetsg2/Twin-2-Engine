#pragma once

#include <core/Component.h>
#include <graphic/Window.h>

namespace Twin2Engine::Core {

	class RenderableComponent : public Component {
	private:			
		bool _isTransparent = false;
	protected:
		RenderableComponent(); // Powoduje ze klasa jest jakby abstrakcyjna no chyba ze bedzie dziedziczona
	public:
		virtual ~RenderableComponent();

		virtual void Render();
		virtual YAML::Node Serialize() const override;

		bool IsTransparent() const;
		void SetIsTransparent(bool value);

		static std::vector<RenderableComponent*> _components;
	};
}