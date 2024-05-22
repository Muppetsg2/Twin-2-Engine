#include <Clouds/Cloud.h>

using namespace Twin2Engine::Core;

void Cloud::Initialize() {

}

void Cloud::Update() {

}


YAML::Node Cloud::Serialize() const {
	YAML::Node node = Component::Serialize();
	return node;
}

bool Cloud::Deserialize(const YAML::Node& node) {
	if (!Component::Deserialize(node)) 
		return false;

	return true;
}

void Cloud::DrawEditor() {

}