#include "output.h"

using Ast::Command;

Output::~Output()
{
    for (auto &m_output : m_outputs)
    {
        delete m_output;
        m_output = nullptr;
    }
}

void Output::AddType(OutputType *output) { m_outputs.push_back(output); }
void Output::Put(const Command &comm, const std::string &out_buffer,
                 OutputTarget target) const
{
    OutputType *out = nullptr;
    for (auto *const output : m_outputs)
    {
        if (output->IsApplicable(comm, target))
        {
            out = output;
            break;
        }
    }

    if (out == nullptr)
    {
        return;
    }

    out->Print(comm, out_buffer);
}
