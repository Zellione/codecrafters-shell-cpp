#pragma once

#include "output_type.h"

class RedirectStdOut : public OutputType
{
  private:
    [[nodiscard]] static const Ast::Redirect *
    GetStdOut(const Ast::Command &comm);

  public:
    void Print(const Ast::Command &comm,
               const std::string &out_buffer) const override;
    [[nodiscard]] bool IsApplicable(const Ast::Command &comm,
                                    OutputTarget target) const override;
};
