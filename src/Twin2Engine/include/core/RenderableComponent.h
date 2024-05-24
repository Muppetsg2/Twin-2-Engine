#pragma once

#include <core/Component.h>
#include <graphic/Window.h>

namespace Twin2Engine::Core {

	class RenderableComponent : public Component {	

	protected:
		CloneBaseFunc(RenderableComponent, Component, _isTransparent)

		bool _isTransparent = false;

		RenderableComponent(); // Powoduje ze klasa jest jakby abstrakcyjna no chyba ze bedzie dziedziczona
	public:
		virtual ~RenderableComponent();

		bool IsTransparent() const;
		void SetIsTransparent(bool value);

		virtual void Render();

		virtual YAML::Node Serialize() const override;
		virtual bool Deserialize(const YAML::Node& node) override;
		
#if _DEBUG
		virtual void DrawEditor() override;
#endif

		static std::vector<RenderableComponent*> _components;
	};
}