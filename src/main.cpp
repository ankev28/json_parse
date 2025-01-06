
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <streambuf>
#include "json.h"

using namespace std;

int main() {
    std::ifstream inputFile("./sample_inputs/official_example.txt"); // Open the file
    std::string line((istreambuf_iterator<char>(inputFile)), istreambuf_iterator<char>());

    json *j = new json(line);
    j->pretty_print();
}
