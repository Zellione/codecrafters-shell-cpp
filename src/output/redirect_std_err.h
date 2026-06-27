#pragma once

#include "output_type.h"

class RedirectStdErr : public OutputType
{
  private:
    [[nodiscard]] static const Token *
    GetStdErr(const std::vector<Token> &tokens);

  public:
    void Print(const std::vector<Token> &tokens,
               const std::string &out_buffer) const override;
    [[nodiscard]] bool IsApplicable(const std::vector<Token> &tokens,
                                    OutputTarget target) const override;
};
