#pragma once

#include <Component.h>

namespace Twin2Engine {
	namespace Core {
		class CameraComponent : public Component {
		protected:
			CameraComponent();
		public:
			virtual ~CameraComponent();
		};
	}
}