#include <graphic/Texture2D.h>
#include <graphic/manager/TextureManager.h>

using namespace Twin2Engine::Graphic;
using namespace glm;

Texture2D::Texture2D(size_t managerId, unsigned int id, unsigned int width, unsigned int height, unsigned int channelsNum, 
	const TextureFormat& format, const TextureWrapMode& sWrapMode, const TextureWrapMode& tWrapMode, 
	const TextureFilterMode& minFilterMode, const TextureFilterMode& magFilterMode)
	: _managerId(managerId), _id(id), _size(width, height), _channelsNum(channelsNum),
	_format(format), _sWrapMode(sWrapMode), _tWrapMode(tWrapMode),
	_minFilterMode(minFilterMode), _magFilterMode(magFilterMode) {}

Texture2D::Texture2D(size_t managerId, unsigned int id, uvec2 size, unsigned int channelsNum,
	const TextureFormat& format, const TextureWrapMode& sWrapMode, const TextureWrapMode& tWrapMode,
	const TextureFilterMode& minFilterMode, const TextureFilterMode& magFilterMode)
	: _managerId(managerId), _id(id), _size(size), _channelsNum(channelsNum),
	_format(format), _sWrapMode(sWrapMode), _tWrapMode(tWrapMode),
	_minFilterMode(minFilterMode), _magFilterMode(magFilterMode) {}

Texture2D::~Texture2D()
{
	glDeleteTextures(1, &_id);
}

void Texture2D::SetWrapModeS(const TextureWrapMode& mode)
{
	glBindTexture(GL_TEXTURE_2D, _id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, (GLint)mode);
	_sWrapMode = mode;
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture2D::SetWrapModeT(const TextureWrapMode& mode)
{
	glBindTexture(GL_TEXTURE_2D, _id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, (GLint)mode);
	_tWrapMode = mode;
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture2D::SetMinFilterMode(const TextureFilterMode& mode)
{
	glBindTexture(GL_TEXTURE_2D, _id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, (GLint)mode);
	_minFilterMode = mode;
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture2D::SetMagFilterMode(const TextureFilterMode& mode)
{
	glBindTexture(GL_TEXTURE_2D, _id);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, (GLint)mode);
	_magFilterMode = mode;
	glBindTexture(GL_TEXTURE_2D, 0);
}

size_t Texture2D::GetManagerId() const
{
	return _managerId;
}

unsigned int Texture2D::GetId() const
{
	return _id;
}

uvec2 Texture2D::GetSize() const {
	return _size;
}

unsigned int Texture2D::GetWidth() const
{
	return _size.x;
}

unsigned int Texture2D::GetHeight() const
{
	return _size.y;
}

unsigned int Texture2D::GetChannelsNum() const
{
	return _channelsNum;
}

TextureFormat Texture2D::GetFormat() const
{
	return _format;
}

TextureWrapMode Texture2D::GetWrapModeS() const
{
	return _sWrapMode;
}

TextureWrapMode Texture2D::GetWrapModeT() const
{
	return _tWrapMode;
}

TextureFilterMode Texture2D::GetMinFilterMode() const
{
	return _minFilterMode;
}

TextureFilterMode Texture2D::GetMagFilterMode() const
{
	return _magFilterMode;
}

void Texture2D::Use(unsigned int samplerId) const
{
	glActiveTexture(GL_TEXTURE0 + samplerId);
	glBindTexture(GL_TEXTURE_2D, _id);
}

void Texture2D::DrawEditor()
{
	const std::vector<TextureWrapMode> wrapModes = {
		TextureWrapMode::REPEAT,
		TextureWrapMode::CLAMP_TO_BORDER,
		TextureWrapMode::CLAMP_TO_EDGE,
		TextureWrapMode::MIRRORED_REPEAT,
		TextureWrapMode::MIRROR_CLAMP_TO_EDGE
	};

	const std::vector<TextureFilterMode> filterModes = {
		TextureFilterMode::NEAREST,
		TextureFilterMode::LINEAR,
		TextureFilterMode::NEAREST_MIPMAP_NEAREST,
		TextureFilterMode::LINEAR_MIPMAP_NEAREST,
		TextureFilterMode::NEAREST_MIPMAP_LINEAR,
		TextureFilterMode::LINEAR_MIPMAP_LINEAR
	};

	std::string id = std::to_string(_managerId);

	ImGui::Text("Name: ");
	ImGui::SameLine();
	ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[1]);
	ImGui::Text(Manager::TextureManager::GetTexture2DName(_managerId).c_str());
	ImGui::PopFont();
	ImGui::Text("Id: ");
	ImGui::SameLine();
	ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[1]);
	ImGui::Text(id.c_str());
	ImGui::PopFont();

	TextureWrapMode wm = _sWrapMode;
	bool clicked = false;
	if (ImGui::BeginCombo(std::string("Wrap Mode S##").append(id).c_str(), to_string(wm).c_str())) {

		for (auto item : wrapModes)
		{
			if (ImGui::Selectable(to_string(item).append("##WrapModeS").append(id).c_str(), wm == item))
			{
				if (clicked) {
					continue;
				}
				wm = item;
				clicked = true;
			}
		}
		ImGui::EndCombo();
	}

	if (clicked) {
		this->SetWrapModeS(wm);
	}

	wm = _tWrapMode;
	clicked = false;
	if (ImGui::BeginCombo(std::string("Wrap Mode T##").append(id).c_str(), to_string(wm).c_str())) {

		for (auto item : wrapModes)
		{
			if (ImGui::Selectable(to_string(item).append("##WrapModeT").append(id).c_str(), wm == item))
			{
				if (clicked) {
					continue;
				}
				wm = item;
				clicked = true;
			}
		}
		ImGui::EndCombo();
	}

	if (clicked) {
		this->SetWrapModeT(wm);
	}

	TextureFilterMode fm = _minFilterMode;
	clicked = false;
	if (ImGui::BeginCombo(std::string("Min Filter Mode##").append(id).c_str(), to_string(fm).c_str())) {

		for (auto item : filterModes)
		{
			if (ImGui::Selectable(to_string(item).append("##MinFilterMode").append(id).c_str(), fm == item))
			{
				if (clicked) {
					continue;
				}
				fm = item;
				clicked = true;
			}
		}
		ImGui::EndCombo();
	}

	if (clicked) {
		this->SetMinFilterMode(fm);
	}

	fm = _magFilterMode;
	clicked = false;
	if (ImGui::BeginCombo(std::string("Mag Filter Mode##").append(id).c_str(), to_string(fm).c_str())) {

		for (auto item : filterModes)
		{
			if (ImGui::Selectable(to_string(item).append("##MagFilterMode").append(id).c_str(), fm == item))
			{
				if (clicked) {
					continue;
				}
				fm = item;
				clicked = true;
			}
		}
		ImGui::EndCombo();
	}

	if (clicked) {
		this->SetMagFilterMode(fm);
	}
}