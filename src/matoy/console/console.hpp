#pragma once

#include <string>

namespace matoy::console {

class Console {
  public:
    void start();

    void loop();

  private:
    std::string input();
};

} // namespace matoy::console
