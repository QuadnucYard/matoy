#pragma once

namespace matoy::syntax {

class AstNode {
  public:
    virtual ~AstNode() = default;
};

class Ident : public AstNode {
  public:
};

class Int : public AstNode {
  public:
};

class Float : public AstNode {
  public:
};

} // namespace matoy::syntax
