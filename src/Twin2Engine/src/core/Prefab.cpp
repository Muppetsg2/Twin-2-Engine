#include <core/Prefab.h>

using namespace Twin2Engine::Core;

void Prefab::SetRootObject(const YAML::Node& rootObjectNode)
{
	_rootObject = rootObjectNode;
}