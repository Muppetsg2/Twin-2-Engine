#ifndef _THREADPROCESS_H_
#define _THREADPROCESS_H_


#include <processes/Process.h>
#include <tools/EventHandler.h>
#include <chrono>
#include <thread>

namespace Twin2Engine::Processes {
	class ThreadProcess : public Process {
	private:
		Twin2Engine::Tools::Method UpdateAction = []() {};
		std::thread t;
		bool stopped = true;

		//void ThreadLoop();
	protected:
		ThreadProcess() {}
	public:
		bool work = false;
		bool goSleep = false;
		std::chrono::duration<float, std::milli> sleepTime{};// = std::chrono::duration_cast<std::milli>(std::chrono::milliseconds(0.0f));
		std::chrono::duration<float, std::milli> delayTime = std::chrono::duration<float, std::milli>(1000.0f);
		ThreadProcess(Twin2Engine::Tools::Method UpdateAction, float delayTimeMillis = 1000.0f) : UpdateAction(UpdateAction), delayTime(delayTimeMillis) { }
		virtual ~ThreadProcess();

		virtual void Initialize() override;
		virtual void Update() override;
		virtual void OnActivation() override;
		virtual void OnDeactivation() override;

		bool Begin();
		void Finish(bool waitForFinish);
		std::thread* getThread();
		void GoSleep(float timeMillis);
		void SetDelayTime(float timeMillis);

		virtual YAML::Node Serialize() const override;
	};
}

#endif //_THREADPROCESS_H_