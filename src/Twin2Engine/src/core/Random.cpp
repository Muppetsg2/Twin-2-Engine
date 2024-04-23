#include <core/Random.h>

using namespace Twin2Engine::Core;


std::random_device Random::_randomDevice;
std::mt19937 Random::_generator{ _randomDevice() };