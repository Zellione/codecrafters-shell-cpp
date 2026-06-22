#include "specialized_parser.h"

SpecializedParser::SpecializedParser() {}
SpecializedParser::~SpecializedParser() {}

bool SpecializedParser::IsEndOfCommand(const std::string &command,
                                       size_t pos) const {
    return pos + 1 == command.length();
}
