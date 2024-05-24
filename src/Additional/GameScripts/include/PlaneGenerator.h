#pragma once

#include <glm/glm.hpp>
#include <core/MeshRenderer.h>

class PlaneGenerator : public Twin2Engine::Core::MeshRenderer {
private:

	string _modelName = "{}PLANE_GENERATED_{}{}";

	unsigned int _rows = 2;
	unsigned int _columns = 2;

	bool _dirty = false;

	void Generate();

public:
	virtual void Initialize() override;
	virtual void Update() override;

	virtual YAML::Node Serialize() const override;
	virtual bool Deserialize(const YAML::Node& node) override;
#if _DEBUG
	virtual void DrawEditor() override;
#endif

	unsigned int GetRowsCount();
	unsigned int GetColumnsCount();

	ivec2 GetGridValues();

	void SetRowsCount(unsigned int count);
	void SetColumnsCount(unsigned int count);

	void SetGridValues(unsigned int rows, unsigned int columns);

	// x - rows
	// y - columns
	void SetGridValues(const ivec2& values);
};