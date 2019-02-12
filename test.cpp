#include <iostream>
using std::cout;
using std::endl;
#include "tokenizer.hpp"

int main(int argc, char *argv[]) {
  if (argc != 2) {
    cout << "Usage: " << argv[0] << " [Filename]" << endl;
    return 0;
  }
  cout << "Starting tokenizer test." << endl
       << endl
       << "| Token Type | Token Value" << endl
       << "+------------+-- - - -  -  -" << endl;
  if (!initialize(argv[1]))
    exit(-1);
  Token t(TOKEN(0), 0, 0, "");
  while ((t = get_token()))
    cout << t << endl;
  cleanup();
  cout << "+------------+-- - - -  -  -" << endl;
}
