#pragma once

#include <manager/ScriptableObjectManager.h>

/*
namespace Twin2Engine::Manager
{
	class ScriptableObjectManager;
}
*/

namespace Twin2Engine::Core
{
	class ScriptableObject;

	template<class T>
	class ScriptableObjectRegister;

	class ScriptableObject
	{
		friend class Twin2Engine::Manager::ScriptableObjectManager;

		friend class ScriptableObjectRegister<ScriptableObject>;
		static void Register();
		size_t _id = 0;
	protected:
		static std::hash<std::string> hasher;
		struct ScriptableObjectData
		{
			std::string scriptableObjectName;
			std::function<ScriptableObject* ()> createSpecificScriptableObject;
		};

		static std::unordered_map<size_t, ScriptableObjectData> scriptableObjects;
		static std::unordered_map<size_t, ScriptableObjectData>* scriptableObjectsTemp;

		static void Register(size_t hash, const ScriptableObjectData& soData);

#if _DEBUG
		void DrawInheritedFields() {

			static bool err = false;
			static bool saved = false;

			if (ImGui::Button(std::string("Save##SO").append(std::to_string(GetId())).c_str(), ImVec2(ImGui::GetContentRegionAvail().x, 0.f))) {
				bool res = Twin2Engine::Manager::ScriptableObjectManager::Save(Twin2Engine::Manager::ScriptableObjectManager::GetPath(GetId()), this);

				if (res) {
					saved = true;
					err = false;
					ImGui::OpenPopup(std::string("SavedPopUp##SO").append(std::to_string(GetId())).c_str(), ImGuiPopupFlags_NoReopen);
				}
				else {
					saved = true;
					err = false;
					ImGui::OpenPopup(std::string("SavedPopUp##SO").append(std::to_string(GetId())).c_str(), ImGuiPopupFlags_NoReopen);
				}
			}

			if (ImGui::BeginPopup(std::string("SavedPopUp##SO").append(std::to_string(GetId())).c_str(), ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoDocking)) {
				if (saved) ImGui::TextColored(ImVec4(0.f, 1.f, 0.f, 1.f), "File Saved!");
				if (err) ImGui::TextColored(ImVec4(1.f, 0.f, 0.f, 1.f), "Error occured when saving file!");
				ImGui::EndPopup();
			}
			else {
				saved = false;
				err = false;
			}
		}
#endif

	public:
		size_t GetId() const;

		virtual void Serialize(YAML::Node& node) const;
		virtual bool Deserialize(const YAML::Node& node);

#if _DEBUG
		virtual void DrawEditor() { }
#endif

		static ScriptableObject* Create();
		static void Init();
	};
#define PRINT_CLASS(Class) \
	printf("Class name: %s", #Class)

	template<class T>
	class ScriptableObjectRegister
	{
	public:
		ScriptableObjectRegister();
	};
	template<class T>
	ScriptableObjectRegister<T>::ScriptableObjectRegister()
	{
		static bool _canBeRegistered = true;
		if (_canBeRegistered)
		{
			_canBeRegistered = false;
			T::Register();
		}
	}
}

//std::unordered_map<size_t, Twin2Engine::Core::ScriptableObject::ScriptableObjectData> Twin2Engine::Core::ScriptableObject::scriptableObjects;

#define SCRIPTABLE_OBJECT_BODY(ScriptableObjectClass) \
		friend class Twin2Engine::Core::ScriptableObjectRegister<ScriptableObjectClass>; \
		static std::string _registeredName; \
		static Twin2Engine::Core::ScriptableObject* Create(); \
		static void Register();
		//static Twin2Engine::Core::ScriptableObjectRegister<ScriptableObjectClass> registererInstance##ScriptableObjectClass; \

#define SCRIPTABLE_OBJECT_SOURCE_CODE(ScriptableObjectClass, ScriptableObjectNamespace, RegisteredName) \
namespace ScriptableObjectNamespace \
{ \
	std::string ScriptableObjectClass::_registeredName = RegisteredName; \
	Twin2Engine::Core::ScriptableObject* ScriptableObjectClass::Create() \
	{ \
		return new ScriptableObjectClass(); \
	} \
	  \
	void ScriptableObjectClass::Register() \
	{ \
		Twin2Engine::Core::ScriptableObject::Register(Twin2Engine::Core::ScriptableObject::hasher(RegisteredName), ScriptableObjectData { .scriptableObjectName = RegisteredName, .createSpecificScriptableObject = ScriptableObjectClass::Create }); \
	} \
}

#define SCRIPTABLE_OBJECT_SOURCE_CODE_NN(ScriptableObjectClass, RegisteredName) \
std::string ScriptableObjectClass::_registeredName = RegisteredName; \
Twin2Engine::Core::ScriptableObject* ScriptableObjectClass::Create() \
{ \
	return new ScriptableObjectClass(); \
} \
  \
void ScriptableObjectClass::Register() \
{ \
	Twin2Engine::Core::ScriptableObject::Register(Twin2Engine::Core::ScriptableObject::hasher(RegisteredName), ScriptableObjectData { .scriptableObjectName = RegisteredName, .createSpecificScriptableObject = ScriptableObjectClass::Create }); \
} 


//scriptableObjects[hasher(_registeredName)] = ScriptableObjectData { .scriptableObjectName = _registeredName, .createSpecificScriptableObject = ScriptableObjectClass::Create }; \
//template<class T>
//struct ScriplableObjectYAMLConverter : public YAML::convert
//{
//	static Node encode(const T& rhs) {
//		Node node;
//		rhs.Serialize(node);
//		return node;
//	}
//
//	static bool decode(const Node& node, T& rhs)
//	{
//		return rhs.Deserialize(node);
//	}
//};
//template<class T>
//struct ScriplableObjectPtrYAMLConverter : public YAML::convert<T*>
//{
//	static Node encode(const T*& rhs) {
//		Node node;
//		rhs->Serialize(node);
//		return node;
//	}
//
//	static bool decode(const Node& node, T*& rhs)
//	{
//		return rhs->Deserialize(node);
//	}
//};
//using ScriplableObjectYAMLConverter_##ScriptableObjectClass = ScriplableObjectYAMLConverter<ScriptableObjectClass>; \
//using ScriplableObjectPtrYAMLConverter_##ScriptableObjectClass = ScriplableObjectPtrYAMLConverter<ScriptableObjectClass>; \

//*
#define SERIALIZABLE_SCRIPTABLE_OBJECT_NN(ScriptableObjectClass) \
	static Twin2Engine::Core::ScriptableObjectRegister<ScriptableObjectClass> registererInstance##ScriptableObjectClass;

#define SERIALIZABLE_SCRIPTABLE_OBJECT(ScriptableObjectClass, ScriptableObjectNamespace) \
	static Twin2Engine::Core::ScriptableObjectRegister<ScriptableObjectNamespace::ScriptableObjectClass> registererInstance##ScriptableObjectClass;
//template<> struct YAML::convert<ScriptableObjectClass> { \
//	static Node encode(const ScriptableObjectClass& rhs) { \
//		Node node; \
//		rhs.Serialize(node); \
//		return node; \
//	} \
// \
//	static bool decode(const Node& node, ScriptableObjectClass& rhs) \
//	{ \
//		rhs.Deserialize(node); \
//	} \
//}; \
//template<> struct YAML::convert<ScriptableObjectClass*> { \
//	static Node encode(const ScriptableObjectClass*& rhs) { \
//		Node node; \
//		rhs->Serialize(node); \
//		return node; \
//	} \
// \
//	static bool decode(const Node& node, ScriptableObjectClass*& rhs) \
//	{ \
//		rhs->Deserialize(node); \
//	} \
//}; \



#define SO_SERIALIZE() \
virtual void Serialize(YAML::Node& node) const override;

#define SO_SERIALIZATION_BEGIN(ScriptableObjectClass, ScriptableObjectBaseClass) \
void ScriptableObjectClass::Serialize(YAML::Node& node) const \
{ \
	ScriptableObjectBaseClass::Serialize(node); \
	node["__SO_RegisteredName__"] = _registeredName;

#define SO_SERIALIZATION_END() \
}

#define SO_SERIALIZE_FIELD(field) \
	node[#field] = field;
#define SO_SERIALIZE_FIELD_F(field, serializer) \
	node[#field] = serializer(field);
#define SO_SERIALIZE_FIELD_R(name, field) \
	node[name] = field;
#define SO_SERIALIZE_FIELD_F_R(name, field, serializer) \
	node[name] = serializer(field);


#define SO_DESERIALIZE() \
virtual bool Deserialize(const YAML::Node& node) override;

#define SO_DESERIALIZATION_BEGIN(ScriptableObjectClass, ScriptableObjectBaseClass) \
bool ScriptableObjectClass::Deserialize(const YAML::Node& node) \
{ \
	bool baseReturned = ScriptableObjectBaseClass::Deserialize(node);

#define SO_DESERIALIZATION_END() \
	return baseReturned; \
}

#define SO_DESERIALIZE_FIELD(field) \
	field = node[#field].as<decltype(field)>();
#define SO_DESERIALIZE_FIELD_F(field, deserializer) \
	field = deserializer(node[#field].as<decltype(field)>());
#define SO_DESERIALIZE_FIELD_F_T(field, deserializer, type) \
	field = deserializer(node[#field].as<type>());
#define SO_DESERIALIZE_FIELD_R(name, field) \
	field = node[name].as<decltype(field)>();
#define SO_DESERIALIZE_FIELD_F_R(name, field, deserializer) \
	field = deserializer(node[name]);
#define SO_DESERIALIZE_FIELD_F_T_R(name, field, deserializer, type) \
	field = deserializer(node[name].as<type>());
//#define SO_DESERIALIZE_FIELD(field, type) \
//	field = node[#field].as<type>();
//#define SO_DESERIALIZE_FIELD_R(name, field, type) \
//	field = node[name].as<type>();
//#define SO_DESERIALIZE_FIELD_F(field, deserializer) \
//	field = deserializer(node[#field]);
//#define SO_DESERIALIZE_FIELD_F_R(name, field, deserializer) \
//	field = deserializer(node[name]);