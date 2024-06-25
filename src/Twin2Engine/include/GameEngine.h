#pragma once
// HID
#include <core/Input.h>

// TIME
#include <core/Time.h>

// GRAPHIC_ENGINE
#include <GraphicEnigine.h>

// LOGGER
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

// MANAGERS
#include <manager/AudioManager.h>
#include <manager/SceneManager.h>
#include <manager/PrefabManager.h>
#include <physic/CollisionManager.h>

// GAME OBJECT
#include <core/GameObject.h>

// COMPONENTS
#include <ui/Image.h>
#include <ui/Text.h>
#include <ui/Button.h>
#include <ui/InputField.h>
#include <ui/Canvas.h>
#include <ui/GIFPlayer.h>
#include <core/MeshRenderer.h>
#include <core/AudioComponent.h>
#include <core/CameraComponent.h>

#include <core/BoxColliderComponent.h>
#include <core/CapsuleColliderComponent.h>
#include <core/SphereColliderComponent.h>
#include <core/HexagonalColliderComponent.h>

#include <core/PointLightComponent.h>
#include <core/SpotLightComponent.h>
#include <core/DirectionalLightComponent.h>
#include <Clouds/Cloud.h>
#include<core/DistanceFieldGenerator.h>

// COMPONENTS MAP
#include <core/ComponentsMap.h>

// SCENE
#include <core/Scene.h>

// PREFAB
#include <core/Prefab.h>

// SCRIPTABLE OBJECT
#include <core/ScriptableObject.h>

// PROCESSED
#include <processes/Process.h>
#include <processes/ProcessManager.h>
#include <processes/SynchronizedProcess.h>
#include <processes/ThreadProcess.h>
#include <processes/TimerProcess.h>

// PLAYER PREFS
#include <core/PlayerPrefs.h>


namespace Twin2Engine {
	class GameEngine {
	private:
		static bool updateShadowLightingMap;

		static void Deserializers();

		static void Update();
		static void Render();
		static void EndFrame();

		static void Loop();
		static void End();

	public:
		static Tools::MethodEventHandler OnInput;
		static Tools::MethodEventHandler EarlyUpdate;
		static Tools::MethodEventHandler LateUpdate;
		static Tools::MethodEventHandler EarlyRender;
		static Tools::MethodEventHandler LateRender;
		static Tools::MethodEventHandler EarlyEnd;

		static bool Init(const std::string& window_name, int32_t window_width, int32_t window_height, bool fullscreen, int32_t gl_version_major, int32_t gl_version_minor);
		static void Start();
	};
}