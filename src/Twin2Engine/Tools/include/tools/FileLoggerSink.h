#ifndef _FILE_LOGGER_SINK_H_
#define _FILE_LOGGER_SINK_H_

namespace Twin2Engine::Tools
{
    template<typename Mutex>
    class FileLoggerSink : public spdlog::sinks::base_sink<Mutex>
    {
        std::list<std::string> _queueToSave;
        std::string _filename;
        std::mutex _mutex;
        std::thread* _loggingThread;
        std::chrono::duration<float, std::milli> _sleepTime;
        bool _run = true;


        static void ThreadFunction(FileLoggerSink* sink)
        {
            std::ofstream logFile(sink->_filename);

            std::list<std::string> queueToSave;

            if (logFile.is_open())
            {
                while (sink->_run)
                {
                    sink->_mutex.lock();

                    queueToSave.swap(sink->_queueToSave);

                    sink->_mutex.unlock();

                    for (const std::string& message : queueToSave)
                    {
                        logFile << message;
                    }
                    logFile.flush();

                    queueToSave.clear();

                    std::this_thread::sleep_for(sink->_sleepTime);
                }
            }

            logFile.close();
        }

    protected:
        void sink_it_(const spdlog::details::log_msg& msg) override 
        {
            spdlog::memory_buf_t formatted;
            this->formatter_->format(msg, formatted);

            _mutex.lock();

            _queueToSave.push_back(fmt::to_string(formatted));

            _mutex.unlock();
        }

        void flush_() override {}

    public:

        FileLoggerSink(const std::string& filename, float sleepTimeMilis)
        {
            _filename = filename;
            _sleepTime = std::chrono::duration<float, std::milli>(sleepTimeMilis);
        }

        void StartLogging()
        {
            _run = true;

            _loggingThread = new std::thread(ThreadFunction, this);
        }

        void StopLogging()
        {
            _run = false;

            _loggingThread->join();

            delete _loggingThread;
        }
    };
}

#endif // !_FILE_LOGGER_SINK_H_