#pragma once

#define MAX_LOG_DRAW 500
#define COLLAPSE 2

namespace Editor::Common
{
    template<typename Mutex>
    class ImGuiSink;

    struct ImGuiLogMessage
    {
        std::string messageContent;
        spdlog::level::level_enum level;
        spdlog::source_loc source;
        size_t count;

        ImGuiLogMessage(const std::string& messageContent, const spdlog::level::level_enum& level, const spdlog::source_loc& source, size_t count)
        {
            ImGuiLogMessage::messageContent = messageContent;
            ImGuiLogMessage::level = level;
            ImGuiLogMessage::source = source;
            ImGuiLogMessage::count = count;
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

    struct MessageToDisplay {
        ImVec4 color;
        std::string content;
        size_t count;
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
            _loggingThread = nullptr;
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

            static const char* const tracy_GettingLogsName = "ImGui::Getting Loggs";

#if TRACY_PROFILER
            ZoneScoped;
            FrameMarkStart(tracy_GettingLogsName);
#endif

            // Convert log message to string
            spdlog::memory_buf_t formatted;
            this->formatter_->format(msg, formatted);
            std::string message = fmt::to_string(formatted);

#if COLLAPSE == 0
            MessageHolder::logMessages.emplace_back(ImGuiLogMessage(message, msg.level, msg.source, 1));
#elif COLLAPSE == 1
            bool saved = false;
            size_t size = MessageHolder::logMessages.size();
            if (size > 1) {
                ImGuiLogMessage& m = MessageHolder::logMessages[size - 1];
                std::string m1 = message.substr(26);
                std::string m2 = m.messageContent.substr(26);
                if (msg.level == m.level && msg.source.filename == m.source.filename && msg.source.line == m.source.line && m1 == m2) {
                    if (m.count != ULLONG_MAX) ++m.count;
                    m.messageContent = message;
                    saved = true;
                }
            }
            
            if (!saved) {
                MessageHolder::logMessages.emplace_back(ImGuiLogMessage(message, msg.level, msg.source, 1));
            }
#elif COLLAPSE == 2
            bool saved = false;
            size_t size = MessageHolder::logMessages.size();
            if (size > 1) {
                std::string m1 = message.substr(26);

                auto item = std::find_if(MessageHolder::logMessages.begin(), MessageHolder::logMessages.end(), [&](ImGuiLogMessage& mess) -> bool {
                    std::string m2 = mess.messageContent.substr(26);
                    return msg.level == mess.level && msg.source.filename == mess.source.filename && msg.source.line == mess.source.line && m1 == m2; 
                });

                if (item != MessageHolder::logMessages.end()) {
                    if ((*item).count != ULLONG_MAX) ++(*item).count;
                    (*item).messageContent = message;
                    saved = true;
                }
            }

            if (!saved) {
                MessageHolder::logMessages.emplace_back(ImGuiLogMessage(message, msg.level, msg.source, 1));
            }
#endif

            _mutex.lock();

            _queueToSave.push_back(message);

            _mutex.unlock();

            if (MessageHolder::logMessages.size() == ULLONG_MAX) {
                MessageHolder::logMessages.erase(MessageHolder::logMessages.begin(), MessageHolder::logMessages.begin() + MessageHolder::logMessages.size() / 2 + 1);
            }

            //if (MessageHolder::logFile.is_open())
            //{
            //    MessageHolder::logFile << message << std::endl;
            //    MessageHolder::logFile.flush();
            //}

#if TRACY_PROFILER
            FrameMarkEnd(tracy_GettingLogsName);
#endif
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

        static void DrawEditor(bool* p_open)
        {
            ImGui::SetNextWindowSizeConstraints(ImVec2(700, 300), ImVec2(1920, 1080));

            if (!ImGui::Begin("Console", p_open)) {
                ImGui::End();
                return;
            }

            static bool logLevels[7] = { true, true, true, false, true, true, true };

            ImGui::Checkbox("TRACE##Console", &logLevels[SPDLOG_LEVEL_TRACE]);
            ImGui::SameLine();
            ImGui::Checkbox("DEBUG##Console", &logLevels[SPDLOG_LEVEL_DEBUG]);
            ImGui::SameLine();
            ImGui::Checkbox("INFO##Console", &logLevels[SPDLOG_LEVEL_INFO]);
            ImGui::SameLine();
            ImGui::Checkbox("WARN##Console", &logLevels[SPDLOG_LEVEL_WARN]);
            ImGui::SameLine();
            ImGui::Checkbox("ERR##Console", &logLevels[SPDLOG_LEVEL_ERROR]);
            ImGui::SameLine();
            ImGui::Checkbox("CRITICAL##Console", &logLevels[SPDLOG_LEVEL_CRITICAL]);
            ImGui::SameLine();
            ImGui::Checkbox("OFF##Console", &logLevels[SPDLOG_LEVEL_OFF]);

            float buttonPosX = ImGui::GetWindowWidth() - ImGui::GetStyle().ItemSpacing.x - ImGui::CalcTextSize("Clear").x - 5;

            ImGui::SameLine();

            ImGui::SetCursorPosX(buttonPosX);

            // Draw button
            if (ImGui::Button("Clear##Console")) {
                // Handle button click
                Clear();
            }

            ImGui::SameLine();
            float autoScrollPosX = buttonPosX - 4 * ImGui::GetStyle().ItemSpacing.x - ImGui::CalcTextSize("Auto Scroll").x;
            ImGui::SetCursorPosX(autoScrollPosX);

            static bool autoScroll = true;
            ImGui::Checkbox("Auto Scroll##Console", &autoScroll);

            static char buf[128] = "";
            ImGui::InputText("##searchConsole", buf, IM_ARRAYSIZE(buf));
            ImGui::SameLine();
            static std::string searchContent;

            //if (ImGui::Button(ICON_FA_SEARCH "##search_button")) {
            if (ImGui::Button("?##search_buttonConsole")) 
            {
                searchContent = buf;
            }
            ImGui::SameLine();
            if (ImGui::Button("X##clear_search_buttonConsole"))
            {
                buf[0] = '\0';
                searchContent = buf;
            }
            ImGui::SameLine();
            static bool caseSensitive = true;
            ImGui::Checkbox("Case Sensitive##Console", &caseSensitive);

            ImGui::BeginChild("LogWindow##Console", ImVec2(0, 0), true);
            //const vector<ImGuiLogMessage> messages = ImGuiSink<mutex>::getLogMessages();

            std::vector<MessageToDisplay> messages = std::vector<MessageToDisplay>();
            messages.reserve(MAX_LOG_DRAW);
            size_t i = 0;
            for (size_t index = MessageHolder::logMessages.size(); index > 0 ; --index)
            {
                if (messages.size() == MAX_LOG_DRAW) break;

                if (logLevels[MessageHolder::logMessages[index - 1].level])
                {
                    if (!searchContent.size() || ContainsString(MessageHolder::logMessages[index - 1].messageContent, searchContent, caseSensitive))
                    {
                        ImVec4 textColor;
                        switch (MessageHolder::logMessages[index - 1].level)
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

                        messages.insert(messages.begin() + (i++), MessageToDisplay{ .color = textColor, .content = MessageHolder::logMessages[index - 1].messageContent, .count = MessageHolder::logMessages[index - 1].count });
                        //ImGui::TextColored(textColor, "%s", MessageHolder::logMessages[index].messageContent.c_str());
                    }
                }
            }

#if COLLAPSE == 0
            for (size_t i = messages.size(); i > 0; --i) {
                ImGui::Separator();
                ImGui::TextColored(messages[i - 1].color, "%s", messages[i - 1].content.c_str());
            }
            ImGui::Separator();
#elif COLLAPSE == 1
            const ImVec2 p = ImGui::GetCursorScreenPos();
            const float maxTextWidth = ImGui::CalcTextSize("  999+  ").x;
            ImDrawList* draw_list = ImGui::GetWindowDrawList();
            float x = p.x + ImGui::GetContentRegionAvail().x - 45, y = p.y + 4; // 4 is for separator

            for (size_t i = messages.size(); i > 0; --i) {
                ImGui::Separator();
                ImGui::TextColored(messages[i - 1].color, "%s", messages[i - 1].content.c_str());

                float height = ImGui::CalcTextSize(messages[i - 1].content.c_str()).y - ImGui::GetTextLineHeight();

                y += height / 2.f;

                if (messages[i - 1].count > 1) {
                    draw_list->AddRectFilled(ImVec2(x, y), ImVec2(x + maxTextWidth, y + ImGui::GetTextLineHeight()), ImColor(ImVec4(0.5f, 0.5f, 0.5f, 1.0f)), 10.0f);
                    std::string text = messages[i - 1].count > 999 ? std::string("999+") : std::to_string(messages[i - 1].count);

                    float width = ImGui::CalcTextSize(text.c_str()).x;

                    draw_list->AddText(ImVec2(x + (maxTextWidth - width) / 2.f, y), ImColor(ImVec4(1.0f, 1.0f, 1.0f, 1.0f)), text.c_str());
                }

                y += height / 2.f + ImGui::GetTextLineHeight() + 4 + 4;
            }
            ImGui::Separator();
#elif COLLAPSE == 2
            const ImVec2 p = ImGui::GetCursorScreenPos();
            const float maxTextWidth = ImGui::CalcTextSize("  999+  ").x;
            ImDrawList* draw_list = ImGui::GetWindowDrawList();
            float x = p.x + ImGui::GetContentRegionAvail().x - 45, y = p.y + 4; // 4 is for separator

            for (size_t i = messages.size(); i > 0; --i) {
                ImGui::Separator();
                ImGui::TextColored(messages[i - 1].color, "%s", messages[i - 1].content.c_str());

                float height = ImGui::CalcTextSize(messages[i - 1].content.c_str()).y - ImGui::GetTextLineHeight();

                y += height / 2.f;

                draw_list->AddRectFilled(ImVec2(x, y), ImVec2(x + maxTextWidth, y + ImGui::GetTextLineHeight()), ImColor(ImVec4(0.5f, 0.5f, 0.5f, 1.0f)), 10.0f);
                std::string text = messages[i - 1].count > 999 ? std::string("999+") : std::to_string(messages[i - 1].count);

                float width = ImGui::CalcTextSize(text.c_str()).x;

                draw_list->AddText(ImVec2(x + (maxTextWidth - width) / 2.f, y), ImColor(ImVec4(1.0f, 1.0f, 1.0f, 1.0f)), text.c_str());

                y += height / 2.f + ImGui::GetTextLineHeight() + 4 + 4;
            }
            ImGui::Separator();
#endif

            messages.clear();

            //static float lastScrollPosition = 0.0;
            //static bool lastFrameOnEnd = true;
            //cout << "Scroll" << ImGui::GetScrollY() << endl;

            if (ImGui::GetScrollY() < ImGui::GetScrollMaxY()) {
                autoScroll = false;
            }

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