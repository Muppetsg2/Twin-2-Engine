#include <PlaneGenerator.h>
#include <graphic/Vertex.h>
#include <core/MathExtensions.h>
#include <graphic/manager/ModelsManager.h>
#include <manager/SceneManager.h>
#include <graphic/manager/MaterialsManager.h>

using namespace Twin2Engine::Graphic;
using namespace Twin2Engine::Manager;

void PlaneGenerator::Generate()
{
    std::vector<Vertex> vertices = std::vector<Vertex>();
    std::vector<unsigned int> indices = std::vector<unsigned int>();

    float diffX = 1.f / (_columns - 1);
    float diffZ = 1.f / (_rows - 1);

    std::vector<unsigned int> trisNum;

    for (int row = 0; row < _rows; ++row) {

        float z = -.5f + row * diffZ;

        for (int col = 0; col < _columns; ++col) {
            float x = -.5f + col * diffX;

            vertices.push_back({ { x, 0.f, z }, { fmapf(x, -.5f, .5f, 0.f, 1.f), fmapf(z, -.5f, .5f, 0.f, 1.f) }, { 0.f, 1.f, 0.f }, glm::vec3(0.f), glm::vec3(0.f) });

            if (row * _columns + col == 0 || row * _columns + col == _rows * _columns - 1) {
                trisNum.push_back(1);
            }
            else if ((row == 0 && col + 1 == _columns) || (row + 1 == _rows && col == 0)) {
                trisNum.push_back(2);
            }
            else if (row == 0 || row + 1 == _rows || col == 0 || col + 1 == _columns) {
                trisNum.push_back(3);
            }
            else {
                trisNum.push_back(6);
            }
        }
    }

    for (unsigned int i = 0; i < vertices.size(); ++i) {

        if ((i + 1) % _columns == 0) {
            continue;
        }

        if (i + _columns >= vertices.size()) {
            break;
        }

        // First Triangle
        indices.push_back(i + _columns);
        indices.push_back(i + 1);
        indices.push_back(i);

        std::pair<glm::vec3, glm::vec3> TB = CalcTangentBitangent(vertices, i + _columns, i + 1, i);

        vertices[i + _columns].Tangent += TB.first;
        vertices[i + _columns].Bitangent += TB.second;

        vertices[i + 1].Tangent += TB.first;
        vertices[i + 1].Bitangent += TB.second;

        vertices[i].Tangent += TB.first;
        vertices[i].Bitangent += TB.second;

        // Second Triangle
        indices.push_back(i + _columns);
        indices.push_back(i + _columns + 1);
        indices.push_back(i + 1);

        TB = CalcTangentBitangent(vertices, i + _columns, i + _columns + 1, i + 1);

        vertices[i + _columns].Tangent += TB.first;
        vertices[i + _columns].Bitangent += TB.second;

        vertices[i + _columns + 1].Tangent += TB.first;
        vertices[i + _columns + 1].Bitangent += TB.second;

        vertices[i + 1].Tangent += TB.first;
        vertices[i + 1].Bitangent += TB.second;
    }

    for (unsigned int i = 0; i < vertices.size(); ++i) {
        vertices[i].Tangent /= (float)trisNum[i];
        vertices[i].Tangent = glm::normalize(vertices[i].Tangent);

        vertices[i].Bitangent /= (float)trisNum[i];
        vertices[i].Bitangent = glm::normalize(vertices[i].Bitangent);
    }

    trisNum.clear();

    if (ModelsManager::IsModelLoaded(_modelName)) {
        MeshRenderer::OnModelDataDestroyed();
        ModelsManager::UnloadModel(_modelName);
    }

    SetModel(ModelsManager::CreateModel(_modelName, std::move(vertices), std::move(indices)));

    _dirty = false;
}

void PlaneGenerator::Initialize()
{
    _modelName = std::vformat(std::string_view(_modelName), std::make_format_args("{", this->GetId(), "}"));
    Generate();
	MeshRenderer::Initialize();
}

void PlaneGenerator::Update()
{
    MeshRenderer::Update();
    if (_dirty) Generate();
}

YAML::Node PlaneGenerator::Serialize() const 
{
    YAML::Node node = RenderableComponent::Serialize();
    node["type"] = "PlaneGenerator";
    node["rows"] = _rows;
    node["columns"] = _columns;
	node["materials"] = vector<size_t>();
	for (const auto& mat : _materials) {
		node["materials"].push_back(SceneManager::GetMaterialSaveIdx(mat.GetId()));
	}
    return node;
}

bool PlaneGenerator::Deserialize(const YAML::Node& node)
{
    if (!node["materials"] || 
        !node["rows"] ||
        !node["columns"] ||
        !RenderableComponent::Deserialize(node)) return false;

    SetGridValues(node["rows"].as<unsigned int>(), node["columns"].as<unsigned int>());

    _materials = vector<Material>();
    for (const auto& mat : node["materials"]) {
        _materials.push_back(MaterialsManager::GetMaterial(SceneManager::GetMaterial(mat.as<size_t>())));
    }

    return true;
}

#if _DEBUG
void PlaneGenerator::DrawEditor() 
{
    std::string id = std::string(std::to_string(this->GetId()));
    std::string name = std::string("Plane Generator##Component").append(id);
    if (ImGui::CollapsingHeader(name.c_str())) {
        Component::DrawInheritedFields();
        
        ImGui::Text("Model: ");
        ImGui::SameLine();
        ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[1]);
        ImGui::Text(_modelName.c_str());
        ImGui::PopFont();

        ImGui::Text("Material: ");
        ImGui::SameLine();
        ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[1]);
        ImGui::Text(MaterialsManager::GetMaterialName(_materials[0].GetId()).c_str());
        ImGui::PopFont();

        unsigned int r = _rows;
        ImGui::DragUInt(string("Rows##").append(id).c_str(), &r, 1.f, 2);

        unsigned int c = _columns;
        ImGui::DragUInt(string("Columns##").append(id).c_str(), &c, 1.f, 2);

        if (r != _rows || c != _columns) {
            SetGridValues(r, c);
        }

        // TODO: Dodac opcje zmiany Material
    }
}
#endif

unsigned int PlaneGenerator::GetRowsCount()
{
    return _rows;
}

unsigned int PlaneGenerator::GetColumnsCount() 
{
    return _columns;
}

ivec2 PlaneGenerator::GetGridValues() 
{
    return ivec2(_rows, _columns);
}

void PlaneGenerator::SetRowsCount(unsigned int count) 
{
    if (_rows == count) return;
    
    _rows = count < 2 ? 2 : count;
    _dirty = true;
}

void PlaneGenerator::SetColumnsCount(unsigned int count)
{
    if (_columns == count) return;

    _columns = count < 2 ? 2 : count;
    _dirty = true;
}

void PlaneGenerator::SetGridValues(unsigned int rows, unsigned int columns)
{
    if (_rows == rows && _columns == columns) return;

    if (_rows != rows) _rows = rows < 2 ? 2 : rows;
    if (_columns != columns) _columns = columns < 2 ? 2 : columns;

    _dirty = true;
}

void PlaneGenerator::SetGridValues(const ivec2& values)
{
    SetGridValues(values.x, values.y);
}
