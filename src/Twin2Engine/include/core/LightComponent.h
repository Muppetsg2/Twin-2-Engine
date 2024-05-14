#ifndef _LIGHTCOMPONENT_H_
#define _LIGHTCOMPONENT_H_

#include <graphic/LightingController.h>
#include <core/Component.h>
#include <tools/EventHandler.h>

namespace Twin2Engine {
	namespace Core {

		class LightComponent : public Component {
			protected:
				virtual YAML::Node Serialize() const override {
					return Component::Serialize();
				}
		};
	}
}

#endif // !_LIGHTCOMPONENT_H_
