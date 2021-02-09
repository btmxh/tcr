#include "tcr_input.h"

std::string _nextLineChecked(std::function<std::optional<std::string>()> nextLineFunc, std::string exceptionOutput) {
    if(auto line = nextLineFunc()) {
        if(std::all_of(line->begin(), line->end(), [](unsigned char c) { return std::isspace(c); })) {
            return _nextLineChecked(nextLineFunc, exceptionOutput);
        } else {
            return *line;
        }
    } else {
        throw tcr::LineNotFoundException { exceptionOutput };
    }
}

tcr::RawChatMessage parseMessage(std::function<std::string(std::string)> nextLineChecked) {
    tcr::RawChatMessage msg;
    msg.time = tcr::timestampFromString(tcr::trim(nextLineChecked("")));
    msg.author.name = tcr::trim(nextLineChecked("Message author line missing"));
    msg.author.nameColor = tcr::colorFromString(tcr::trim(nextLineChecked("Message author color missing")));
    
    auto icons = tcr::trim(nextLineChecked("Message author icons missing"));
    if(icons != "$null") {
        size_t prev = 0, curr;
        while((curr = icons.find(' ', prev)) != std::string::npos) {
            std::string icon = icons.substr(prev, curr - prev);
            if(!icon.empty())   msg.author.icons.push_back(icon);
            prev = curr + 1;
        }
        std::string icon = icons.substr(prev, curr - prev);
        if(!icon.empty())   msg.author.icons.push_back(icon);
    }

    msg.content = tcr::trim(nextLineChecked("Message content missing"));
    if(msg.content == "$null")
    return msg;
}

tcr::RawChatScript tcr::parse(std::function<std::optional<std::string>()> nextLine) {
    auto nextLineChecked = [&](std::string ex) { return _nextLineChecked(nextLine, ex); };
    tcr::RawChatScript script;

    try {
        while(true) {
            script.messages.push_back(parseMessage(nextLineChecked));
        }
    } catch (tcr::LineNotFoundException& ex) {
        if(ex.what.size() != 0) {
            std::stringstream ss;
            ss << "Error parsing chat script: " << ex.what;
            throw std::runtime_error(ss.str());
        }
    }

    return script;
}