#include <core/DistanceFieldGenerator.h>
#include <core/MathExtensions.h>
#include <graphic/manager/ShaderManager.h>
#include <graphic/Window.h>
#include <core/GameObject.h>

#include <GraphicEnigine.h>

using namespace Twin2Engine::Core;
using namespace Twin2Engine::Graphic;
using namespace Twin2Engine::Manager;
using namespace Twin2Engine::Tools;

// UBO DATA
GLuint DistanceFieldGenerator::_uboDistanceFieldData = 0;
STD140Struct DistanceFieldGenerator::_uboDistanceFieldDataStruct {
	STD140Variable<glm::mat4>("dFProjection"),
	STD140Variable<glm::mat4>("dFView"),
	STD140Variable<glm::vec3>("dFPos"),
};

// PLANE
bool DistanceFieldGenerator::_planeCreated = false;
GLuint DistanceFieldGenerator::_planeVAO = 0;
GLuint DistanceFieldGenerator::_planeVBO = 0;
GLuint DistanceFieldGenerator::_planeEBO = 0;
float DistanceFieldGenerator::_planeVerts[8] = {
	-1.f, 1.f,  //0.f, 0.f,
	1.f, 1.f,   //1.f, 0.f,
	-1.f, -1.f, //0.f, 1.f,
	1.f, -1.f   //1.f, 1.f
};
GLuint DistanceFieldGenerator::_planeIndicies[6] = {
	2, 1, 0,
	2, 3, 1
};

// GAUSSIAN BLUR
size_t DistanceFieldGenerator::_blurMSize = 40;
float DistanceFieldGenerator::_blurKernel[40] = {};

// SHADERS
Shader* DistanceFieldGenerator::_depthShader;
Shader* DistanceFieldGenerator::_blurShader;

void DistanceFieldGenerator::CreatePlane()
{
	if (_planeCreated) return;

	glGenVertexArrays(1, &_planeVAO);
	glGenBuffers(1, &_planeVBO);
	glGenBuffers(1, &_planeEBO);

	glBindVertexArray(_planeVAO);
	glBindBuffer(GL_ARRAY_BUFFER, _planeVBO);

	glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(float), _planeVerts, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _planeEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(GLuint), _planeIndicies, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	_planeCreated = true;
}

void DistanceFieldGenerator::GenerateBlurKernel()
{
	if (_blurMSize == 0) return;

	float sigma = 7.0;
	int kSize = (_blurMSize - 1) / 2;
	for (int j = 0; j <= kSize; ++j)
	{
		_blurKernel[kSize + j] = _blurKernel[kSize - j] = normpdf(float(j), sigma);
	}
}

void DistanceFieldGenerator::GenerateTexture()
{
	if (!IsEnable() || !GetGameObject()->GetActive()) {
		return;
	}

	GraphicEngine::UpdateBeforeRendering();

	glBindFramebuffer(GL_FRAMEBUFFER, _depthFBO);
	glClearColor(0.f, 0.f, 0.f, 1.f);
	glClear(GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, _width, _height);
	glEnable(GL_DEPTH_TEST);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	_depthShader->Use();
	vec3 pos = GetTransform()->GetGlobalPosition();
	mat4 mat = glm::lookAt(pos, pos + _front, _up);

	_depthShader->SetMat4("view", mat);

	//mat = glm::perspective(glm::radians(_fov), (_height != 0) ? ((float)_width / (float)_height) : 0, _near, _far);
	mat = glm::ortho(-_orthoSize, _orthoSize, -_orthoSize, _orthoSize, -100.f, 100.f);
	_depthShader->SetMat4("projection", mat);
	GraphicEngine::PreRender();
	
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, _blurFBO);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, _width, _height);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	_blurShader->Use();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, _depthMap);
	_blurShader->SetBool("isDepth", true);
	_blurShader->SetInt("Texture", 0);
	//_blurShader->SetFloat("nearPlane", _near);
	//_blurShader->SetFloat("farPlane", _far);

	_blurShader->SetFloatArray("blurKernel", _blurKernel, 40);
	_blurShader->SetInt("blurMSize", (int)_blurMSize);

	glBindVertexArray(_planeVAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, &_planeIndicies);
	glBindVertexArray(0);

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glEnable(GL_CULL_FACE);

	UpdateUBOData();
}

void DistanceFieldGenerator::UpdateUBOData()
{
	glBindBuffer(GL_UNIFORM_BUFFER, _uboDistanceFieldData);

	vec3 pos = GetTransform()->GetGlobalPosition();
	_uboDistanceFieldDataStruct.Set("dFProjection", glm::ortho(-_orthoSize, _orthoSize, -_orthoSize, _orthoSize, -100.f, 100.f));
	//_uboDistanceFieldDataStruct.Set("dFProjection", glm::perspective(glm::radians(_fov), (_height != 0) ? ((float)_width / (float)_height) : 0, _near, _far));
	_uboDistanceFieldDataStruct.Set("dFView", glm::lookAt(pos, pos + _front, _up));
	_uboDistanceFieldDataStruct.Set("dFPos", pos);

	//Jesli wiecej kamer i kazda ma ze swojego kata dawac obraz
	glBufferSubData(GL_UNIFORM_BUFFER, 0, _uboDistanceFieldDataStruct.GetSize(), _uboDistanceFieldDataStruct.GetData().data());
}

void DistanceFieldGenerator::Initialize()
{
	_depthShader = ShaderManager::GetShaderProgram("origin/DistanceFieldDepthShader");
	_blurShader = ShaderManager::GetShaderProgram("origin/BlurShader");

	CreatePlane();
	GenerateBlurKernel();

	_blurShader->Use();
	_blurShader->SetFloatArray("blurKernel", _blurKernel, 40);
	_blurShader->SetUInt("blurMSize", _blurMSize);
	
	SetLookingDir(_dir);

	vec3 pos = GetTransform()->GetGlobalPosition();
	mat4 mat = glm::lookAt(pos, pos + _front, _up);

	_depthShader->Use();
	_depthShader->SetMat4("view", mat);

	//mat = glm::perspective(glm::radians(_fov), (_height != 0) ? ((float)_width / (float)_height) : 0, _near, _far);
	mat = glm::ortho(-_orthoSize, _orthoSize, -_orthoSize, _orthoSize, -100.f, 100.f);
	_depthShader->SetMat4("projection", mat);

	// UBO MATRICIES
	glGenBuffers(1, &_uboDistanceFieldData);

	glBindBuffer(GL_UNIFORM_BUFFER, _uboDistanceFieldData);
	glBindBufferBase(GL_UNIFORM_BUFFER, 6, _uboDistanceFieldData);

	UpdateUBOData();

	glGenFramebuffers(1, &_depthFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, _depthFBO);

	glGenTextures(1, &_depthMap);
	glBindTexture(GL_TEXTURE_2D, _depthMap);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, _width, _height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_NONE);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, _depthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glGenFramebuffers(1, &_blurFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, _blurFBO);

	glGenTextures(1, &_texture);
	glBindTexture(GL_TEXTURE_2D, _texture);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, _width, _height, 0, GL_RGB, GL_FLOAT, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _texture, 0);
	glReadBuffer(GL_NONE);

	glBindTexture(GL_TEXTURE_2D, 0);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void DistanceFieldGenerator::Update()
{
	if (!_generated) GenerateTexture();
	//GenerateTexture();

	UseTexture(25);
}

YAML::Node DistanceFieldGenerator::Serialize() const
{
	YAML::Node node = Component::Serialize();
	node["type"] = "DistanceFieldGenerator";
	node["dir"] = (size_t)(uint8_t)_dir;
	node["width"] = _width;
	node["height"] = _height;
	node["orthoSize"] = _orthoSize;
	//node["fov"] = _fov;
	//node["nearPlane"] = _near;
	//node["farPlane"] = _far;
	node["worldUp"] = _worldUp;
	return node;
}

bool DistanceFieldGenerator::Deserialize(const YAML::Node& node)
{
	if (!node["dir"] || !node["width"] || !node["height"] || 
		!node["orthoSize"] ||
		//!node["fov"] || !node["nearPlane"] || !node["farPlane"] || 
		!node["worldUp"] || !Component::Deserialize(node)) return false;

	_dir = (LookingDir)(uint8_t)node["dir"].as<size_t>();
	_width = node["width"].as<unsigned int>();
	_height = node["height"].as<unsigned int>();
	_orthoSize = node["orthoSize"].as<float>();
	//_fov = node["fov"].as<float>();
	//_near = node["nearPlane"].as<float>();
	//_far = node["farPlane"].as<float>();
	_worldUp = node["worldUp"].as<vec3>();

	return true;
}

void DistanceFieldGenerator::DrawEditor()
{
	string id = string(std::to_string(this->GetId()));
	string name = string("Distance Field Generator##Component").append(id);
	if (ImGui::CollapsingHeader(name.c_str())) {

		if (Component::DrawInheritedFields()) return;

		// Image
		ImTextureID im_id = (ImTextureID)_texture;

		ImGuiStyle& style = ImGui::GetStyle();
		float alignment = 0.5f;

		float siz = 100 + style.FramePadding.x * 2.0f;
		float avail = ImGui::GetContentRegionAvail().x;

		float off = (avail - siz) * alignment;
		if (off > 0.0f)
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + off);

		if (ImGui::BeginChild(std::string("Preview##DistanceFieldGenerator").append(id).c_str(), ImVec2(100, 100))) {

			ImGui::Image(im_id, ImGui::GetContentRegionAvail(), ImVec2(0.f, 0.f), ImVec2(1.f, 1.f));

			ImGui::EndChild();
		}

		// Width
		unsigned int w = _width;
		ImGui::DragUInt(std::string("Width##").append(id).c_str(), &w, 1, 0, UINT_MAX);

		// Height
		unsigned int h = _height;
		ImGui::DragUInt(std::string("Height##").append(id).c_str(), &h, 1, 0, UINT_MAX);

		if (w != _width || h != _height) {
			SetTextureDimmensions(glm::uvec2(w, h));
		}

		// View
		float os = _orthoSize;
		ImGui::DragFloat(std::string("OrthoSize##").append(id).c_str(), &os, 0.1f, 0.f, FLT_MAX);

		if (os != _orthoSize) _orthoSize = os;

		// Dir
		LookingDir d = _dir;
		bool clicked = false;
		if (ImGui::BeginCombo(string("Direction##").append(id).c_str(), to_string(d).c_str())) {

			for (size_t i = 0; i < size<LookingDir>(); ++i)
			{
				LookingDir dc = (LookingDir)(uint8_t)(i);
				if (ImGui::Selectable(to_string(dc).append("##").append(id).c_str(), d == dc))
				{
					if (clicked) {
						continue;
					}
					d = dc;
					clicked = true;
				}
			}
			ImGui::EndCombo();
		}

		if (clicked && _dir != d) {
			this->SetLookingDir(d);
		}

		// Generate Button
		if (ImGui::Button(std::string("Generate##").append(id).c_str(), ImVec2(ImGui::GetContentRegionAvail().x, 0.f))) {
			GenerateTexture();
		}
	}
}

void DistanceFieldGenerator::UseTexture(unsigned int i)
{
	glActiveTexture(GL_TEXTURE0 + i);
	glBindTexture(GL_TEXTURE_2D, _texture);
}

DistanceFieldGenerator::LookingDir DistanceFieldGenerator::GetLookingDir()
{
	return _dir;
}

unsigned int DistanceFieldGenerator::GetTextureId()
{
	return _texture;
}

unsigned int DistanceFieldGenerator::GetTextureWidth()
{
	return _width;
}

unsigned int DistanceFieldGenerator::GetTextureHeight()
{
	return _height;
}

glm::uvec2 DistanceFieldGenerator::GetTextureDimmensions()
{
	return glm::uvec2(_width, _height);
}

void DistanceFieldGenerator::SetLookingDir(LookingDir dir)
{
	glm::vec3 front;

	switch (dir) {
		case LookingDir::FRONT: {
			front = glm::vec3(0.f, 0.f, -1.f);
			break;
		}
		case LookingDir::BACK: {
			front = glm::vec3(0.f, 0.f, 1.f);
			break;
		}
		case LookingDir::LEFT: {
			front = glm::vec3(-1.f, 0.f, 0.f);
			break;
		}
		case LookingDir::RIGHT: {
			front = glm::vec3(1.f, 0.f, 0.f);
			break;
		}
		case LookingDir::UP: {
			front = glm::vec3(0.f, 0.99985f, 0.01745f);
			break;
		}
		case LookingDir::DOWN: {
			front = glm::vec3(0.f, -0.99985f, -0.01745f);
			break;
		}
	}

	_dir = dir;
	_front = glm::normalize(front);
	_right = normalize(cross(_front, _worldUp));
	_up = normalize(cross(_right, _front));
}

void DistanceFieldGenerator::SetTextureWidth(unsigned int width)
{
	if (_width != width) SetTextureDimmensions({ width, _height });
}

void DistanceFieldGenerator::SetTextureHeight(unsigned int height)
{
	if (_height != height) SetTextureDimmensions({ _width, height });
}

void DistanceFieldGenerator::SetTextureDimmensions(glm::uvec2 dim)
{
	if (dim.x != _width || dim.y != _height) {
		_width = dim.x;
		_height = dim.y;

		glBindTexture(GL_TEXTURE_2D, _depthMap);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, _width, _height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

		glBindTexture(GL_TEXTURE_2D, _texture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, _width, _height, 0, GL_RGB, GL_FLOAT, NULL);
	}
}