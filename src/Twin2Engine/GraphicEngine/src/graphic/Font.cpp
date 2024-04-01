#include <graphic/Font.h>

using namespace Twin2Engine::GraphicEngine;
using namespace std;

Font::Font(map<char, Character*> glyphs) : _glyphs(glyphs) {}

Font::~Font() {
	for (auto& g : _glyphs) {
		glDeleteTextures(1, &g.second->TextureID);
	}
	_glyphs.clear();
}

Character* Font::GetCharacter(char character) {
	if (_glyphs.find(character) != _glyphs.end()) {
		return _glyphs[character];
	}
	return nullptr;
}

vector<Character*> Font::GetText(const string& text) {
	vector<Character*> res = vector<Character*>();
	for (auto& c : text) {
		res.push_back(GetCharacter(c));
	}
	return res;
}