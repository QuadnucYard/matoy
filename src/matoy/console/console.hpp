#pragma once

#include <string>

namespace matoy::console {

class Console {
  public:
    void start();

    void loop();

  private:
    std::string input(bool cont);
};

} // namespace matoy::console
