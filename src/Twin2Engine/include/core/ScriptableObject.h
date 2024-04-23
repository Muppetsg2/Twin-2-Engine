#ifndef _SCRIPTABLE_OBJECT_H_
#define _SCRIPTABLE_OBJECT_H_

namespace Twin2Engine::Core
{
	class ScriptableObject;

	template<class T>
	class ScriptableObjectRegister;

	class ScriptableObject
	{
		friend class ScriptableObjectRegister<ScriptableObject>;
		static void Register();
		static std::hash<std::string> hasher;

		struct ScriptableObjectData
		{
			std::string scriptableObjectName;
			std::function<ScriptableObject* ()> createSpecificScriptableObject;
		};

		static std::unordered_map<size_t, ScriptableObjectData> scriptableObjects;

	public:

		virtual void Serialize(YAML::Node& node);
		virtual void Deserialize(const YAML::Node& node);

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

#define SCIPTABLE_OBJECT_BODY(ScriptableObjectClass) \
		friend class Twin2Engine::Core::ScriptableObjectRegister<ScriptableObjectClass>; \
		static ScriptableObject* Create(); \
		static void Register(); \

#define SCRIPTABLE_OBJECT_SOURCE_CODE(ScriptableObjectClass, ScriptableObjectNamespace, RegisteredName) \
{ \
	using namespace ScriptableObjectNamespace; \
	ScriptableObject* ScriptableObjectClass::Create() \
	{ \
		return new ScriptableObjectClass(); \
	} \
	  \
	void ScriptableObjectClass::Register() \
	{ \
		scriptableObjects[hasher(RegisteredName)] = ScriptableObjectData { .scriptableObjectName = RegisteredName, .createSpecificScriptableObject = ScriptableObjectClass::Create }; \
	} \
	  \
	Twin2Engine::Core::ScriptableObjectRegister<ScriptableObjectClass> registererInstance##ScriptableObjectClass(); \
} \

#endif // !_SCRIPTABLE_OBJECT_H_
