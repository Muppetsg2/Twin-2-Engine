#pragma once

#include <core/Component.h>
#include <graphic/Window.h>

namespace Twin2Engine::Core {

	class RenderableComponent : public Component {
		/*CloneFunctionStart(RenderableComponent, Component)
			CloneField(_isTransparent)
		CloneFunctionEnd()*/		

	protected:
		CloneBaseFunc(RenderableComponent, Component, _isTransparent)

		bool _isTransparent = false;

		RenderableComponent(); // Powoduje ze klasa jest jakby abstrakcyjna no chyba ze bedzie dziedziczona
	public:
		virtual ~RenderableComponent();

		virtual void Render();
		virtual YAML::Node Serialize() const override;
		virtual bool Deserialize(const YAML::Node& node) override;
		virtual void DrawEditor() override;

		bool IsTransparent() const;
		void SetIsTransparent(bool value);

		static std::vector<RenderableComponent*> _components;
	};
}