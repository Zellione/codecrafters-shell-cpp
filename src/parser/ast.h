#pragma once

#include "token.h"
#include <cstdint>
#include <ranges>
#include <string>
#include <vector>

namespace Ast
{

struct Node;

struct Redirect
{
    TokenType Type;
    std::string Target;
};

struct Command
{
    std::vector<std::string> Args;
    std::vector<Redirect> Redirects;
    bool Background;
};

enum class NodeType : uint8_t
{
    SEQUENCE,
    PIPELINE
};

struct Pipeline
{
    std::vector<Command> Commands;
};

struct Sequence
{
    Node *Left;
    TokenType Operator;
    Node *Right;
};

struct Node
{
    union NodeClass
    {
        NodeClass(Sequence *seq) : SequenceNode(seq) {}
        NodeClass(Pipeline *pipe) : PipelineNode(pipe) {}
        Sequence *SequenceNode;
        Pipeline *PipelineNode;
    };

    NodeClass *AstNode;
    NodeType Type;

    Node(NodeClass *astNode, NodeType type) : AstNode(astNode), Type(type) {}
    ~Node()
    {
        if (Type == NodeType::PIPELINE)
        {
            delete AstNode->PipelineNode;
        }

        if (Type == NodeType::SEQUENCE)
        {
            delete AstNode->SequenceNode->Left;
            delete AstNode->SequenceNode->Right;
            delete AstNode->SequenceNode;
        }
    }
};

inline const Command &get_last_command(Node *nodes)
{
    Node *current_node = nodes;
    Pipeline *last_pipeline;
    while (current_node != nullptr)
    {
        Node *tmp = nullptr;
        switch (current_node->Type)
        {
        case NodeType::SEQUENCE: {
            Sequence *seq = current_node->AstNode->SequenceNode;
            if (seq->Right != nullptr)
            {
                tmp = seq->Right;
            }
        }
        break;
        case NodeType::PIPELINE: {
            last_pipeline = current_node->AstNode->PipelineNode;
            tmp = nullptr;
        }
        break;
        }

        current_node = tmp;
    }

    return last_pipeline->Commands.back();
}

inline std::string stringify_command(const Command &command)
{
    std::string commandline = command.Args |
                              std::views::join_with(std::string_view(" ")) |
                              std::ranges::to<std::string>();
    if (command.Background)
    {
        commandline.append(" &");
    }

    return commandline;
}

}; // namespace Ast
