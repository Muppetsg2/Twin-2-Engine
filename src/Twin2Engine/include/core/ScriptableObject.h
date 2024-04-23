#ifndef _SCRIPTABLE_OBJECT_H_
#define _SCRIPTABLE_OBJECT_H_

#include <manager/ScriptableObjectManager.h>

namespace Twin2Engine::Manager
{
	class ScriptableObjectManager;
}

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

		size_t _id;
	protected:
		static std::hash<std::string> hasher;
		struct ScriptableObjectData
		{
			std::string scriptableObjectName;
			std::function<ScriptableObject* ()> createSpecificScriptableObject;
		};

		static std::unordered_map<size_t, ScriptableObjectData> scriptableObjects;

	public:
		size_t GetId() const;

		virtual void Serialize(YAML::Node& node) const;
		virtual bool Deserialize(const YAML::Node& node);

		static ScriptableObject* Create();
	};


	template<class T>
	class ScriptableObjectRegister
	{
		bool _canBeRegistered = true;
	public:
		ScriptableObjectRegister();
	};
	template<class T>
	ScriptableObjectRegister<T>::ScriptableObjectRegister()
	{
		if (_canBeRegistered)
		{
			T::Register();
			_canBeRegistered = false;
		}
	}
}

#define SCRIPTABLE_OBJECT_BODY(ScriptableObjectClass) \
		friend class Twin2Engine::Core::ScriptableObjectRegister<ScriptableObjectClass>; \
		static std::string _registeredName; \
		static Twin2Engine::Core::ScriptableObject* Create(); \
		static void Register(); \

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
		scriptableObjects[hasher(_registeredName)] = ScriptableObjectData { .scriptableObjectName = _registeredName, .createSpecificScriptableObject = ScriptableObjectClass::Create }; \
	} \
	  \
	Twin2Engine::Core::ScriptableObjectRegister<ScriptableObjectClass> registererInstance##ScriptableObjectClass(); \
} \

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
#define SERIALIZABLE_SCRIPTABLE_OBJECT(ScriptableObjectClass) \
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
	field = deserializer(node[#field]);
#define SO_DESERIALIZE_FIELD_R(name, field) \
	field = node[name].as<decltype(field)>();
#define SO_DESERIALIZE_FIELD_F_R(name, field, deserializer) \
	field = deserializer(node[name]);
//#define SO_DESERIALIZE_FIELD(field, type) \
//	field = node[#field].as<type>();
//#define SO_DESERIALIZE_FIELD_R(name, field, type) \
//	field = node[name].as<type>();
//#define SO_DESERIALIZE_FIELD_F(field, deserializer) \
//	field = deserializer(node[#field]);
//#define SO_DESERIALIZE_FIELD_F_R(name, field, deserializer) \
//	field = deserializer(node[name]);

/**/
#endif // !_SCRIPTABLE_OBJECT_H_
