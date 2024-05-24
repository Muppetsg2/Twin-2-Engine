#pragma once 

#include <Playable.h>

using namespace Twin2Engine::Core;

class Player : public Playable {
public:

	virtual YAML::Node Serialize() const override;
	virtual bool Deserialize(const YAML::Node& node) override;

#if _DEBUG
	virtual void DrawEditor() override;
#endif
};