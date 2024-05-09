#ifndef _SYNCHRONIZEDPROCESS_H_
#define _SYNCHRONIZEDPROCESS_H_

#include <processes/Process.h>
#include <core/EventHandler.h>

namespace Twin2Engine::Processes {
	//class Process;

	class SynchronizedProcess : public Process {
		private:
			Twin2Engine::Core::Action<> UpdateAction = []() {};
		protected:
			SynchronizedProcess() {}
		public:
			SynchronizedProcess(Twin2Engine::Core::Action<> UpdateAction) : UpdateAction(UpdateAction) {}
			virtual ~SynchronizedProcess();

			virtual void Initialize() override;
			virtual void Update() override;

			virtual YAML::Node Serialize() const override;
	};
}

#endif // !_SYNCHRONIZEDPROCESS_H_
