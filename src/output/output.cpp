#include "output.h"

Output::Output() {}

Output::~Output() {
    for (size_t i = 0; i < m_outputs.size(); i++) {
        delete m_outputs[i];
        m_outputs[i] = nullptr;
    }
}

void Output::AddType(OutputType *output) { m_outputs.push_back(output); }
void Output::Put(const std::vector<Token> &tokens, const char *out_buffer) {
    OutputType *out = nullptr;
    for (const auto output : m_outputs) {
        if (output->IsApplicable(tokens)) {
            out = output;
            break;
        }
    }

    if (out == nullptr)
        return;

    out->Print(tokens, out_buffer);
}
