#ifndef _IMGUI_SINK_H_
#define _IMGUI_SINK_H_

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
    class ImGuiSink : public spdlog::sinks::base_sink<Mutex> {
    public:
        ImGuiSink(const std::string& filename)
        {
            MessageHolder::logFile.open(filename);
        }

    protected:
        void sink_it_(const spdlog::details::log_msg& msg) override {
            // Convert log message to string
            spdlog::memory_buf_t formatted;
            this->formatter_->format(msg, formatted);
            std::string message = fmt::to_string(formatted);

            // Store the log message
            MessageHolder::logMessages.emplace_back(ImGuiLogMessage(message, msg.level, msg.source));

            if (MessageHolder::logFile.is_open())
            {
                MessageHolder::logFile << message << std::endl;
                MessageHolder::logFile.flush();
            }
        }

        void flush_() override {}

    public:
        // Function to retrieve log messages
        //static const std::vector<spdlog::details::log_msg>& getLogMessages() {
        static const std::vector<ImGuiLogMessage>& getLogMessages() {
            return MessageHolder::logMessages;
        }

        static void clear()
        {
            MessageHolder::logMessages.clear();
        }

    private:
        //static std::vector<std::string> logMessages;
    };

    // Static member definition
    //std::vector<spdlog::details::log_msg> MessageHolder::logMessages;
    std::vector<ImGuiLogMessage> MessageHolder::logMessages;
    std::ofstream MessageHolder::logFile;
}

#endif // !_IMGUI_SINK_H_