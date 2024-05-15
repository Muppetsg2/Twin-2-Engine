#pragma once

namespace Editor::Common
{
    template<typename Mutex>
    class ImGuiSink;

    struct ImGuiLogMessage
    {
        std::string messageContent;
        spdlog::level::level_enum level;
        spdlog::source_loc source;

        ImGuiLogMessage(const std::string& messageContent, const spdlog::level::level_enum& level, const spdlog::source_loc& source)
        {
            ImGuiLogMessage::messageContent = messageContent;
            ImGuiLogMessage::level = level;
            ImGuiLogMessage::source = source;
        }
    };

    class MessageHolder
    {
        template<typename Mutex>
        friend class ImGuiSink;

        //static std::vector<spdlog::details::log_msg> logMessages;
        static std::vector<ImGuiLogMessage> logMessages;
        static std::ofstream logFile;

    };

    template<typename Mutex>
    class ImGuiSink : public spdlog::sinks::base_sink<Mutex> 
    {
        std::list<std::string> _queueToSave;
        std::string _filename;
        std::mutex _mutex;
        std::thread* _loggingThread;
        std::chrono::duration<float, std::milli> _sleepTime;
        bool _run = true;

        static void ThreadFunction(ImGuiSink* sink)
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

    public:
        ImGuiSink(const std::string& filename, float sleepTimeMilis)
        {
            //MessageHolder::logFile.open(filename);
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

    protected:
        void sink_it_(const spdlog::details::log_msg& msg) override {
            // Convert log message to string
            spdlog::memory_buf_t formatted;
            this->formatter_->format(msg, formatted);
            std::string message = fmt::to_string(formatted);

            MessageHolder::logMessages.emplace_back(ImGuiLogMessage(message, msg.level, msg.source));

            _mutex.lock();

            _queueToSave.push_back(message);

            _mutex.unlock();

            //if (MessageHolder::logFile.is_open())
            //{
            //    MessageHolder::logFile << message << std::endl;
            //    MessageHolder::logFile.flush();
            //}
        }

        void flush_() override {}

    public:
        // Function to retrieve log messages
        //static const std::vector<spdlog::details::log_msg>& getLogMessages() {
        static const std::vector<ImGuiLogMessage>& getLogMessages() {
            return MessageHolder::logMessages;
        }

        static void Clear()
        {
            MessageHolder::logMessages.clear();
        }

        static void Draw()
        {
            static bool logLevels[7] = { true, true, true, true, true, true, true };

            ImGui::SetNextWindowSizeConstraints(ImVec2(600, 300), ImVec2(1920, 1080));

            ImGui::Begin("Console");
            ImGui::Checkbox("TRACE", &logLevels[SPDLOG_LEVEL_TRACE]);
            ImGui::SameLine();
            ImGui::Checkbox("DEBUG", &logLevels[SPDLOG_LEVEL_DEBUG]);
            ImGui::SameLine();
            ImGui::Checkbox("INFO", &logLevels[SPDLOG_LEVEL_INFO]);
            ImGui::SameLine();
            ImGui::Checkbox("WARN", &logLevels[SPDLOG_LEVEL_WARN]);
            ImGui::SameLine();
            ImGui::Checkbox("ERR", &logLevels[SPDLOG_LEVEL_ERROR]);
            ImGui::SameLine();
            ImGui::Checkbox("CRITICAL", &logLevels[SPDLOG_LEVEL_CRITICAL]);
            ImGui::SameLine();
            ImGui::Checkbox("OFF", &logLevels[SPDLOG_LEVEL_OFF]);

            float buttonPosX = ImGui::GetWindowWidth() - ImGui::GetStyle().ItemSpacing.x - ImGui::CalcTextSize("Clear").x;

            ImGui::SameLine();

            ImGui::SetCursorPosX(buttonPosX);

            // Draw button
            if (ImGui::Button("Clear")) {
                // Handle button click
                Clear();
            }

            ImGui::SameLine();
            float autoScrollPosX = buttonPosX - 4 * ImGui::GetStyle().ItemSpacing.x - ImGui::CalcTextSize("Auto Scroll").x;
            ImGui::SetCursorPosX(autoScrollPosX);

            static bool autoScroll = true;
            ImGui::Checkbox("Auto Scroll", &autoScroll);

            static char buf[128] = "";
            ImGui::InputText("##search", buf, IM_ARRAYSIZE(buf));
            ImGui::SameLine();
            static std::string searchContent;

            //if (ImGui::Button(ICON_FA_SEARCH "##search_button")) {
            if (ImGui::Button("?""##search_button")) 
            {
                searchContent = buf;
            }
            ImGui::SameLine();
            if (ImGui::Button("X""##clear_search_button"))
            {
                buf[0] = '\0';
                searchContent = buf;
            }
            ImGui::SameLine();
            static bool caseSensitive = true;
            ImGui::Checkbox("Case Sensitive", &caseSensitive);

            ImGui::BeginChild("LogWindow", ImVec2(0, 0), true);
            //const vector<ImGuiLogMessage> messages = ImGuiSink<mutex>::getLogMessages();
            for (size_t index = 0ull; index < MessageHolder::logMessages.size(); index++)
            {
                ImVec4 textColor;

                if (logLevels[MessageHolder::logMessages[index].level])
                {
                    if (!searchContent.size() || ContainsString(MessageHolder::logMessages[index].messageContent, searchContent, caseSensitive))
                    {
                        switch (MessageHolder::logMessages[index].level)
                        {
                        case SPDLOG_LEVEL_TRACE:
                            textColor = ImVec4(0.0, 0.5, 0.0, 1.0);
                            break;

                        case SPDLOG_LEVEL_DEBUG:
                            textColor = ImVec4(0.0, 0.5, 0.0, 1.0);
                            break;

                        case SPDLOG_LEVEL_INFO:
                            textColor = ImVec4(0.65, 0.65, 0.65, 1.0);
                            break;

                        case SPDLOG_LEVEL_WARN:
                            textColor = ImVec4(1.0, 1.0, 0.0, 1.0);
                            break;

                        case SPDLOG_LEVEL_ERROR:
                            textColor = ImVec4(1.0, 0.0, 0.0, 1.0);
                            break;

                        case SPDLOG_LEVEL_OFF:
                            textColor = ImVec4(0.5, 0.0, 0.0, 1.0);
                            break;
                        }

                        ImGui::TextColored(textColor, "%s", MessageHolder::logMessages[index].messageContent.c_str());
                    }
                }
            }

            //static float lastScrollPosition = 0.0;
            //static bool lastFrameOnEnd = true;
            //cout << "Scroll" << ImGui::GetScrollY() << endl;
            if (autoScroll)
            {
                ImGui::SetScrollHereY(1.0f);
            }
            //float currentScrollPos = ImGui::GetScrollY();
            //if (currentScrollPos == ImGui::GetScrollMaxY())
            //{
            //    lastFrameOnEnd = true;
            //}

            ImGui::EndChild();


            ImGui::End();
        }

    private:
        static bool ContainsString(const std::string& haystack, const std::string& needle, bool caseSensitive = true) {
        if (caseSensitive) {
            return haystack.find(needle) != std::string::npos;
        }
        else {
            // Convert both strings to lowercase for case-insensitive comparison
            std::string lowercaseHaystack = haystack;
            std::string lowercaseNeedle = needle;
            std::transform(lowercaseHaystack.begin(), lowercaseHaystack.end(), lowercaseHaystack.begin(), ::tolower);
            std::transform(lowercaseNeedle.begin(), lowercaseNeedle.end(), lowercaseNeedle.begin(), ::tolower);
            return lowercaseHaystack.find(lowercaseNeedle) != std::string::npos;
        }
    }
        //static std::vector<std::string> logMessages;
    };

    // Static member definition
    //std::vector<spdlog::details::log_msg> MessageHolder::logMessages;
    std::vector<ImGuiLogMessage> MessageHolder::logMessages;
    std::ofstream MessageHolder::logFile;
}