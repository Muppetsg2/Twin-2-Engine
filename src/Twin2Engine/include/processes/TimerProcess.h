#ifndef _TIMERPROCESS_H_
#define _TIMERPROCESS_H_

#include <processes/Process.h>
#include <core/EventHandler.h>

#include <chrono>
#include <thread>

namespace Twin2Engine::Processes {
	class TimerProcess : public Process {
		private:
			Twin2Engine::Core::Action<> UpdateAction = []() {};
			std::thread t;
			bool stopped = true;

		protected:
			TimerProcess() {}
		public:
			bool work = false;
			std::chrono::time_point<std::chrono::steady_clock> repeatTimePoint;
			int _repeatTimeMillis = 1000;
			TimerProcess(Twin2Engine::Core::Action<> UpdateAction, int repeatTimeMillis = 1000) : UpdateAction(UpdateAction), _repeatTimeMillis(repeatTimeMillis) { }
			virtual ~TimerProcess();

			virtual void Initialize() override;
			virtual void Update() override;
			virtual void OnActivation() override;
			virtual void OnDeactivation() override;

			bool Begin();
			void Finish(bool waitForFinish);
			void SetRepeatTime(int repeatTimeMillis);
	};
}


#endif // !_TIMERPROCESS_H_
