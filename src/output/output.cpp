#include "output.h"

Output::~Output()
{
    for (auto &m_output : m_outputs)
    {
        delete m_output;
        m_output = nullptr;
    }
}

void Output::AddType(OutputType *output) { m_outputs.push_back(output); }
void Output::Put(const std::vector<Token> &tokens,
                 const std::string &out_buffer, OutputTarget target)
{
    OutputType *out = nullptr;
    for (auto *const output : m_outputs)
    {
        if (output->IsApplicable(tokens, target))
        {
            out = output;
            break;
        }
    }

    if (out == nullptr)
    {
        return;
    }

    out->Print(tokens, out_buffer);
}
