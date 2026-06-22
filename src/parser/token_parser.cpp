#include "token_parser.h"
#include "double_quotes.h"
#include "single_quotes.h"
#include "specialized_parser.h"

TokenParser::TokenParser() {
    m_Parsers = std::vector<SpecializedParser *>();
    m_Parsers.push_back(new SingleQuotes());
    m_Parsers.push_back(new DoubleQuotes());
}

// TODO: Probably refactor into a real tokenizer
std::vector<Node>
TokenParser::GetCommandAndArgs(const std::string &commandline) const {
    std::vector<Node> comm_and_args;

    std::string buffer = "";
    size_t pos = 0;
    bool skipOuter = false;
    while (pos >= 0 && pos < commandline.length()) {
        for (const auto special_parser : m_Parsers) {
            if (special_parser->IsActivated(commandline[pos])) {
                Token token = special_parser->Parse(commandline, pos);
                pos = token.index_after;
                if (token.token != "")
                    comm_and_args.emplace_back(token.token,
                                               special_parser->GetNodeType());
                skipOuter = true;
                break;
            }
        }

        if (skipOuter) {
            skipOuter = false;
            continue;
        }

        if (commandline[pos] == ' ') {
            comm_and_args.emplace_back(buffer, NodeType::NONE);
            buffer = "";
            if (commandline[pos] == ' ')
                comm_and_args.emplace_back(" ", NodeType::SPACE);
            while (commandline[pos] == ' ') {
                pos++;
            }
            continue;
        }

        buffer += commandline[pos];
        pos++;
    }
    if (buffer.length() > 0)
        comm_and_args.emplace_back(buffer, NodeType::NONE);

    return comm_and_args;
}
