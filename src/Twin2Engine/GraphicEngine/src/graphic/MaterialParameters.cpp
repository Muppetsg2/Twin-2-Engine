#include <graphic/MaterialParameters.h>

#if _DEBUG
#include <graphic/manager/TextureManager.h>
#include <regex>
#endif

#include <spdlog/spdlog.h>

using namespace Twin2Engine::Graphic;
using namespace Twin2Engine::Tools;
using namespace glm;
using namespace std;

 std::hash<std::string> MaterialParameters::hasher;

MaterialParameters::MaterialParameters() {}

MaterialParameters::~MaterialParameters()
{
	_textureMappings.clear();
	_textures.clear();
	glDeleteBuffers(1, &_materialParametersDataUBO);
}

MaterialParameters::MaterialParameters(const STD140Struct& parameters, const map<size_t, char>& textureMappings, const vector<GLuint>& textures)
{
	_parameters = parameters;
	// Tworzenie UBO materialInput
	glGenBuffers(1, &_materialParametersDataUBO);
	glBindBuffer(GL_UNIFORM_BUFFER, _materialParametersDataUBO);
	// Initialization of buffer
	glBufferData(GL_UNIFORM_BUFFER, _parameters.GetSize(), _parameters.GetData().data(), GL_DYNAMIC_DRAW);
	//glBindBufferBase(GL_UNIFORM_BUFFER, BINDING_POINT_MATERIAL_INPUT, _materialParametersDataUBO);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	_textureMappings = textureMappings;
	_textures = textures;
}

#if _DEBUG
YAML::Node MaterialParameters::Serialize() const {
	YAML::Node node;
	std::vector<YAML::Node> params = std::vector<YAML::Node>();
	std::vector<std::string> paramsNames = _parameters.GetNames();

	for (std::string param : paramsNames) {
		
		YAML::Node p;
		p["name"] = param;

		const Tools::ValueType* v = _parameters.GetType(param);

		if (dynamic_cast<const ScalarType*>(v) != nullptr) {
			const ScalarType* t = dynamic_cast<const ScalarType*>(v);

			switch (t->GetType())
			{
				case VALUE_TYPE::BOOL:
				{
					p["type"] = "bool";
					p["value"] = Get<bool>(param);
					break;
				}
				case VALUE_TYPE::INT:
				{
					p["type"] = "int";
					p["value"] = Get<int>(param);
					break;
				}
				case VALUE_TYPE::UINT:
				{
					p["type"] = "uint";
					p["value"] = Get<unsigned int>(param);
					break;
				}
				case VALUE_TYPE::FLOAT:
				{
					p["type"] = "float";
					p["value"] = Get<float>(param);
					break;
				}
				case VALUE_TYPE::DOUBLE:
				{
					p["type"] = "double";
					p["value"] = Get<double>(param);
					break;
				}
				case VALUE_TYPE::OTHER:
				default:
				{
					break;
				}
			}
		}
		else if (dynamic_cast<const VecType*>(v) != nullptr) {
			const VecType* t = dynamic_cast<const VecType*>(v);

			switch (t->GetLength()) {
				case 1:
				{
					switch (t->GetType())
					{
						case VALUE_TYPE::BOOL:
						{
							p["type"] = "bvec1";
							p["value"] = Get<glm::vec<1, bool>>(param);
							break;
						}
						case VALUE_TYPE::INT:
						{
							p["type"] = "ivec1";
							p["value"] = Get<glm::vec<1, int>>(param);
							break;
						}
						case VALUE_TYPE::UINT:
						{
							p["type"] = "uvec1";
							p["value"] = Get<glm::vec<1, unsigned int>>(param);
							break;
						}
						case VALUE_TYPE::FLOAT:
						{
							p["type"] = "vec1";
							p["value"] = Get<glm::vec<1, float>>(param);
							break;
						}
						case VALUE_TYPE::DOUBLE:
						{
							p["type"] = "dvec1";
							p["value"] = Get<glm::vec<1, double>>(param);
							break;
						}
						case VALUE_TYPE::OTHER:
						default:
						{
							break;
						}
					}
					break;
				}
				case 2:
				{
					switch (t->GetType())
					{
						case VALUE_TYPE::BOOL:
						{
							p["type"] = "bvec2";
							p["value"] = Get<glm::vec<2, bool>>(param);
							break;
						}
						case VALUE_TYPE::INT:
						{
							p["type"] = "ivec2";
							p["value"] = Get<glm::vec<2, int>>(param);
							break;
						}
						case VALUE_TYPE::UINT:
						{
							p["type"] = "uvec2";
							p["value"] = Get<glm::vec<2, unsigned int>>(param);
							break;
						}
						case VALUE_TYPE::FLOAT:
						{
							p["type"] = "vec2";
							p["value"] = Get<glm::vec<2, float>>(param);
							break;
						}
						case VALUE_TYPE::DOUBLE:
						{
							p["type"] = "dvec2";
							p["value"] = Get<glm::vec<2, double>>(param);
							break;
						}
						case VALUE_TYPE::OTHER:
						default:
						{
							break;
						}
					}
					break;
				}
				case 3:
				{
					switch (t->GetType())
					{
						case VALUE_TYPE::BOOL:
						{
							p["type"] = "bvec3";
							p["value"] = Get<glm::vec<3, bool>>(param);
							break;
						}
						case VALUE_TYPE::INT:
						{
							p["type"] = "ivec3";
							p["value"] = Get<glm::vec<3, int>>(param);
							break;
						}
						case VALUE_TYPE::UINT:
						{
							p["type"] = "uvec3";
							p["value"] = Get<glm::vec<3, unsigned int>>(param);
							break;
						}
						case VALUE_TYPE::FLOAT:
						{
							p["type"] = "vec3";
							p["value"] = Get<glm::vec<3, float>>(param);
							break;
						}
						case VALUE_TYPE::DOUBLE:
						{
							p["type"] = "dvec3";
							p["value"] = Get<glm::vec<3, double>>(param);
							break;
						}
						case VALUE_TYPE::OTHER:
						default:
						{
							break;
						}
					}
					break;
				}
				case 4:
				{
					switch (t->GetType())
					{
						case VALUE_TYPE::BOOL:
						{
							p["type"] = "bvec4";
							p["value"] = Get<glm::vec<4, bool>>(param);
							break;
						}
						case VALUE_TYPE::INT:
						{
							p["type"] = "ivec4";
							p["value"] = Get<glm::vec<4, int>>(param);
							break;
						}
						case VALUE_TYPE::UINT:
						{
							p["type"] = "uvec4";
							p["value"] = Get<glm::vec<4, unsigned int>>(param);
							break;
						}
						case VALUE_TYPE::FLOAT:
						{
							p["type"] = "vec4";
							p["value"] = Get<glm::vec<4, float>>(param);
							break;
						}
						case VALUE_TYPE::DOUBLE:
						{
							p["type"] = "dvec4";
							p["value"] = Get<glm::vec<4, double>>(param);
							break;
						}
						case VALUE_TYPE::OTHER:
						default:
						{
							break;
						}
					}
					break;
				}
				default:
				{
					break;
				}
			}
		}

		if (p["type"] && p["value"]) params.push_back(p);
	}

	for (std::pair<size_t, char> map : _textureMappings) {
		YAML::Node p;
		p["name"] = _textureNames.at(map.first);
		p["type"] = "texture2D";

		Texture2D* tex = Manager::TextureManager::FindTextureWithProgramID(_textures[map.second]);

		if (tex != nullptr) {
			p["value"] = Manager::TextureManager::GetTexture2DPath(tex->GetManagerId());
		}

		if (p["value"]) params.push_back(p);
	}

	node["parameters"] = params;

	return node;
}

void MaterialParameters::DrawEditor(size_t id) {

	std::string id_s = std::to_string(id);

	std::vector<std::string> paramsNames = _parameters.GetNames();
	for (std::string param : paramsNames) {
		
		const Tools::ValueType* v = _parameters.GetType(param);

		if (dynamic_cast<const ScalarType*>(v) != nullptr) {
			const ScalarType* t = dynamic_cast<const ScalarType*>(v);

			switch(t->GetType())
			{
				case VALUE_TYPE::BOOL:
				{
					bool b = Get<bool>(param);

					ImGui::Checkbox(std::string(param).append("##MaterialParameters").append(id_s).c_str(), &b);

					if (b != Get<bool>(param)) Set(param, b);
					break;
				}
				case VALUE_TYPE::INT:
				{
					int b = Get<int>(param);

					ImGui::DragInt(std::string(param).append("##MaterialParameters").append(id_s).c_str(), &b);

					if (b != Get<int>(param)) Set(param, b);
					break;
				}
				case VALUE_TYPE::UINT:
				{
					unsigned int b = Get<unsigned int>(param);

					ImGui::DragUInt(std::string(param).append("##MaterialParameters").append(id_s).c_str(), &b, 1.0f, 0, UINT_MAX);

					if (b != Get<unsigned int>(param)) Set(param, b);
					break;
				}
				case VALUE_TYPE::FLOAT:
				{
					float b = Get<float>(param);

					ImGui::DragFloat(std::string(param).append("##MaterialParameters").append(id_s).c_str(), &b);

					if (b != Get<float>(param)) Set(param, b);
					break;
				}
				case VALUE_TYPE::DOUBLE:
				{
					double b = Get<double>(param);

					ImGui::DragDouble(std::string(param).append("##MaterialParameters").append(id_s).c_str(), &b);

					if (b != Get<double>(param)) Set(param, b);
					break;
				}
				case VALUE_TYPE::OTHER:
				default:
				{
					break;
				}
			}
		}
		else if (dynamic_cast<const VecType*>(v) != nullptr) {
			const VecType* t = dynamic_cast<const VecType*>(v);

			switch (t->GetLength()) {
				case 1:
				{
					switch (t->GetType())
					{
						case VALUE_TYPE::BOOL:
						{
							glm::bvec1 b = Get<glm::vec<1, bool>>(param);

							ImGui::Checkbox(std::string(param).append("##MaterialParameters").append(id_s).c_str(), &b.x);

							if (b != Get<glm::vec<1, bool>>(param)) Set(param, b);
							break;
						}
						case VALUE_TYPE::INT:
						{
							glm::ivec1 b = Get<glm::vec<1, int>>(param);

							ImGui::DragInt(std::string(param).append("##MaterialParameters").append(id_s).c_str(), &b.x);

							if (b != Get<glm::vec<1, int>>(param)) Set(param, b);
							break;
						}
						case VALUE_TYPE::UINT:
						{
							glm::uvec1 b = Get<glm::vec<1, unsigned int>>(param);

							ImGui::DragUInt(std::string(param).append("##MaterialParameters").append(id_s).c_str(), &b.x, 1.f, 0, UINT_MAX);

							if (b != Get<glm::vec<1, unsigned int>>(param)) Set(param, b);
							break;
						}
						case VALUE_TYPE::FLOAT:
						{
							glm::vec1 b = Get<glm::vec<1, float>>(param);

							ImGui::DragFloat(std::string(param).append("##MaterialParameters").append(id_s).c_str(), &b.x);

							if (b != Get<glm::vec<1, float>>(param)) Set(param, b);
							break;
						}
						case VALUE_TYPE::DOUBLE:
						{
							glm::dvec1 b = Get<glm::vec<1, double>>(param);

							ImGui::DragDouble(std::string(param).append("##MaterialParameters").append(id_s).c_str(), &b.x);

							if (b != Get<glm::vec<1, double>>(param)) Set(param, b);
							break;
						}
						case VALUE_TYPE::OTHER:
						default:
						{
							break;
						}
					}
					break;
				}
				case 2:
				{
					switch (t->GetType())
					{
						case VALUE_TYPE::BOOL:
						{
							glm::bvec2 b = Get<glm::vec<2, bool>>(param);

							ImGui::Checkbox(std::string("##").append(param).append("MaterialParameters1").append(id_s).c_str(), &b.x);
							ImGui::SameLine();
							ImGui::Checkbox(std::string(param).append("##MaterialParameters2").append(id_s).c_str(), &b.y);

							if (b != Get<glm::vec<2, bool>>(param)) Set(param, b);
							break;
						}
						case VALUE_TYPE::INT:
						{
							glm::ivec2 b = Get<glm::vec<2, int>>(param);

							ImGui::DragInt2(std::string(param).append("##MaterialParameters").append(id_s).c_str(), glm::value_ptr(b));

							if (b != Get<glm::vec<2, int>>(param)) Set(param, b);
							break;
						}
						case VALUE_TYPE::UINT:
						{
							glm::uvec2 b = Get<glm::vec<2, unsigned int>>(param);

							ImGui::DragUInt2(std::string(param).append("##MaterialParameters").append(id_s).c_str(), glm::value_ptr(b), 1.f, 0, UINT_MAX);

							if (b != Get<glm::vec<2, unsigned int>>(param)) Set(param, b);
							break;
						}
						case VALUE_TYPE::FLOAT:
						{
							glm::vec2 b = Get<glm::vec<2, float>>(param);

							ImGui::DragFloat2(std::string(param).append("##MaterialParameters").append(id_s).c_str(), glm::value_ptr(b));

							if (b != Get<glm::vec<2, float>>(param)) Set(param, b);
							break;
						}
						case VALUE_TYPE::DOUBLE:
						{
							glm::dvec2 b = Get<glm::vec<2, double>>(param);

							ImGui::DragDouble2(std::string(param).append("##MaterialParameters").append(id_s).c_str(), glm::value_ptr(b));

							if (b != Get<glm::vec<2, double>>(param)) Set(param, b);
							break;
						}
						case VALUE_TYPE::OTHER:
						default:
						{
							break;
						}
					}
					break;
				}
				case 3:
				{
					switch (t->GetType())
					{
						case VALUE_TYPE::BOOL:
						{
							glm::bvec3 b = Get<glm::vec<3, bool>>(param);

							ImGui::Checkbox(std::string("##").append(param).append("MaterialParameters1").append(id_s).c_str(), &b.x);
							ImGui::SameLine();
							ImGui::Checkbox(std::string("##").append(param).append("MaterialParameters2").append(id_s).c_str(), &b.y);
							ImGui::SameLine();
							ImGui::Checkbox(std::string(param).append("##MaterialParameters3").append(id_s).c_str(), &b.z);

							if (b != Get<glm::vec<3, bool>>(param)) Set(param, b);
							break;
						}
						case VALUE_TYPE::INT:
						{
							glm::ivec3 b = Get<glm::vec<3, int>>(param);

							ImGui::DragInt3(std::string(param).append("##MaterialParameters").append(id_s).c_str(), glm::value_ptr(b));

							if (b != Get<glm::vec<3, int>>(param)) Set(param, b);
							break;
						}
						case VALUE_TYPE::UINT:
						{
							glm::uvec3 b = Get<glm::vec<3, unsigned int>>(param);

							ImGui::DragUInt3(std::string(param).append("##MaterialParameters").append(id_s).c_str(), glm::value_ptr(b), 1.f, 0, UINT_MAX);

							if (b != Get<glm::vec<3, unsigned int>>(param)) Set(param, b);
							break;
						}
						case VALUE_TYPE::FLOAT:
						{
							glm::vec3 b = Get<glm::vec<3, float>>(param);

							if (std::regex_search(param, std::regex("(?:[cC][oO][lL][oO][rR])|(?:[cC][oO][lL][oO][uU][rR])"))) {
								ImGui::ColorEdit3(std::string(param).append("##MaterialParameters").append(id_s).c_str(), glm::value_ptr(b));
							}
							else {
								ImGui::DragFloat3(std::string(param).append("##MaterialParameters").append(id_s).c_str(), glm::value_ptr(b));
							}

							if (b != Get<glm::vec<3, float>>(param)) Set(param, b);
							break;
						}
						case VALUE_TYPE::DOUBLE:
						{
							glm::dvec3 b = Get<glm::vec<3, double>>(param);

							ImGui::DragDouble3(std::string(param).append("##MaterialParameters").append(id_s).c_str(), glm::value_ptr(b));

							if (b != Get<glm::vec<3, double>>(param)) Set(param, b);
							break;
						}
						case VALUE_TYPE::OTHER:
						default:
						{
							break;
						}
					}
					break;
				}
				case 4:
				{
					switch (t->GetType())
					{
						case VALUE_TYPE::BOOL:
						{
							glm::bvec4 b = Get<glm::vec<4, bool>>(param);

							ImGui::Checkbox(std::string("##").append(param).append("MaterialParameters1").append(id_s).c_str(), &b.x);
							ImGui::SameLine();
							ImGui::Checkbox(std::string("##").append(param).append("MaterialParameters2").append(id_s).c_str(), &b.y);
							ImGui::SameLine();
							ImGui::Checkbox(std::string("##").append(param).append("MaterialParameters3").append(id_s).c_str(), &b.z);
							ImGui::SameLine();
							ImGui::Checkbox(std::string(param).append("##MaterialParameters4").append(id_s).c_str(), &b.w);

							if (b != Get<glm::vec<4, bool>>(param)) Set(param, b);
							break;
						}
						case VALUE_TYPE::INT:
						{
							glm::ivec4 b = Get<glm::vec<4, int>>(param);

							ImGui::DragInt4(std::string(param).append("##MaterialParameters").append(id_s).c_str(), glm::value_ptr(b));

							if (b != Get<glm::vec<4, int>>(param)) Set(param, b);
							break;
						}
						case VALUE_TYPE::UINT:
						{
							glm::uvec4 b = Get<glm::vec<4, unsigned int>>(param);

							ImGui::DragUInt4(std::string(param).append("##MaterialParameters").append(id_s).c_str(), glm::value_ptr(b), 1.f, 0, UINT_MAX);

							if (b != Get<glm::vec<4, unsigned int>>(param)) Set(param, b);
							break;
						}
						case VALUE_TYPE::FLOAT:
						{
							glm::vec4 b = Get<glm::vec<4, float>>(param);

							if (std::regex_search(param, std::regex("(?:[cC][oO][lL][oO][rR])|(?:[cC][oO][lL][oO][uU][rR])"))) {
								ImGui::ColorEdit4(std::string(param).append("##MaterialParameters").append(id_s).c_str(), glm::value_ptr(b));
							}
							else {
								ImGui::DragFloat4(std::string(param).append("##MaterialParameters").append(id_s).c_str(), glm::value_ptr(b));
							}

							if (b != Get<glm::vec<4, float>>(param)) Set(param, b);
							break;
						}
						case VALUE_TYPE::DOUBLE:
						{
							glm::dvec4 b = Get<glm::vec<4, double>>(param);

							ImGui::DragDouble4(std::string(param).append("##MaterialParameters").append(id_s).c_str(), glm::value_ptr(b));

							if (b != Get<glm::vec<4, double>>(param)) Set(param, b);
							break;
						}
						case VALUE_TYPE::OTHER:
						default:
						{
							break;
						}
					}
					break;
				}
				default:
				{
					break;
				}
			}
		}
	}

	ImGuiTreeNodeFlags node_flag = ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
	bool node_open = ImGui::TreeNodeEx(string("Textures##").append(id_s).c_str(), node_flag);

	if (node_open) {
		for (auto& map : _textureMappings) {
			GLuint tex_id = _textures[map.second];
			string n = std::string(_textureNames[map.first]).append("##TEXTURES MATERIAL PARAMETERS").append(id_s);
			std::map<size_t, std::string> types = Manager::TextureManager::GetAllTexture2DNames();

			Texture2D* tex = Manager::TextureManager::FindTextureWithProgramID(tex_id);

			size_t choosed = tex != nullptr ? tex->GetManagerId() : 0;

			if (ImGui::BeginCombo(n.c_str(), choosed == 0 ? "None" : types[choosed].c_str())) {

				bool click = false;
				size_t i = 0;
				for (auto& item : types) {

					if (ImGui::Selectable(std::string(item.second).append("##").append(_textureNames[map.first]).append(id_s).append(std::to_string(i)).c_str(), item.first == choosed)) {

						if (click) continue;

						choosed = item.first;
						click = true;
					}

					++i;
				}

				if (click) {
					if (choosed != 0) {
						_textures[map.second] = Manager::TextureManager::GetTexture2D(choosed)->GetId();
					}
				}

				ImGui::EndCombo();
			}
		}
		ImGui::TreePop();
	}
}
#endif

void MaterialParameters::SetTexture2D(const std::string& textureName, unsigned int textureId)
{
	size_t hashed = hasher(textureName);

	if (_textureMappings.contains(hashed))
	{
		_textures[_textureMappings[hashed]] = textureId;
	}
}

void MaterialParameters::UploadTextures2D(unsigned int programId, int& beginLocation, int& textureBinded)
{
	for (int i = 0; i < _textures.size(); i++)
	{
		glActiveTexture(GL_TEXTURE0 + textureBinded);
		glBindTexture(GL_TEXTURE_2D, _textures[i]);

		glUniform1i(beginLocation, textureBinded);

		textureBinded++;
		beginLocation++;
	}
}


GLuint MaterialParameters::GetDataUBO() const
{
	return _materialParametersDataUBO;
}

const char* MaterialParameters::GetData() const
{
	return _parameters.GetData().data();
}
size_t MaterialParameters::GetSize() const
{
	return _parameters.GetSize();
}