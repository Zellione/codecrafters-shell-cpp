#include "expander.h"

#include <sstream>

Expander::Expander(Ast::Node *start_node, const VariableRegistry &variables)
    : m_start_node(start_node), m_variables(variables)
{
}

void Expander::Expand() { WalkTree(m_start_node); }

void Expander::WalkTree(Ast::Node *node)
{
    switch (node->Type)
    {
    case Ast::NodeType::SEQUENCE:
        if (node->AstNode->SequenceNode->Left != nullptr)
        {
            WalkTree(node->AstNode->SequenceNode->Left);
        }

        if (node->AstNode->SequenceNode->Left != nullptr)
        {
            WalkTree(node->AstNode->SequenceNode->Right);
        }
        break;
    case Ast::NodeType::PIPELINE:
        for (auto &command : node->AstNode->PipelineNode->Commands)
        {
            ReplaceVars(command);
        }
        break;
    };
}

void Expander::ReplaceVars(Ast::Command &comm)
{
    for (auto &part : comm.Args)
    {
        std::stringstream ss;
        std::string variable_name;
        size_t pos = 0;
        bool braces = false;
        while (pos < part.length() && pos != std::string::npos)
        {
            if (part[pos] == '$' && pos + 1 < part.length())
            {
                size_t end_pos;
                if (part[pos + 1] == '{')
                {
                    braces = true;
                    end_pos = part.find('}', pos + 2);
                }
                else
                {
                    end_pos = part.find(' ', pos + 1);
                }

                if (end_pos == std::string::npos)
                {
                    variable_name = part.substr(pos + 1);
                }
                else
                {
                    variable_name = part.substr(braces ? pos + 2 : pos + 1,
                                                braces ? end_pos - 2 - pos
                                                       : end_pos - 1 - pos);
                }

                ss << m_variables.GetVariable(variable_name);
                variable_name.clear();
                if (braces)
                {
                    end_pos++;
                }
                braces = false;
                pos = end_pos;
            }
            else
            {
                ss << part[pos];
                pos++;
            }
        }

        part = ss.str();
    }
}
