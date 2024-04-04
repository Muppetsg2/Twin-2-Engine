#ifndef _LIGHTCOMPONENT_H_
#define _LIGHTCOMPONENT_H_

#include <LightingController.h>
#include <core/Component.h>
#include <ui/EventHandler.h>

namespace Twin2Engine {
	namespace Core {
		class LightComponent : public Component {
			protected:
				LightComponent() : Component() {};
		};
	}
}

#endif // !_LIGHTCOMPONENT_H_
