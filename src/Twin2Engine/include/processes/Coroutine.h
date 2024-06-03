#ifndef _COROUTINE_H_
#define _COROUTINE_H_

#include <tools/EventHandler.h>
#include <chrono>
#include <thread>
//#include <pthread.h>



namespace Twin2Engine::Processes {
	class Coroutine {
		private:
			std::thread t;
			bool terminated = false;
			static void ThreadTask(Twin2Engine::Tools::Action<bool*> coroutineTask, bool* terminated) {
				coroutineTask(terminated);
			}

		public:
			Twin2Engine::Tools::Action<bool*> coroutineTask;
			Coroutine(Twin2Engine::Tools::Action<bool*> coroutineTask) : coroutineTask(coroutineTask) {	}
			~Coroutine() {
				End();
			}

			void Start() {
				t = std::thread(ThreadTask, coroutineTask, &terminated);
			}

			void End() {
				if (!terminated) {
					terminated = true;
					t.join();
				}
			}
	};
}


#endif // !_COROUTINE_H_
