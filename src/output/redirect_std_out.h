#pragma once

#include "output_type.h"

class RedirectStdOut : public OutputType {
  private:
    std::string GetStdOut(const std::vector<Token> &tokens) const;

  public:
    virtual void Print(const std::vector<Token> &tokens,
                       const char *out_buffer) const override;
    virtual bool IsApplicable(const std::vector<Token> &tokens) const override;
};
