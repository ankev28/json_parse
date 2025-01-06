#include <string>
#include <map>
#include <vector>
#include <iostream>
#include "node.h"


using namespace std;


// top-level class to instantiate for representing the overall json "object"
// we can also access and modify contents through the inner nodes themselves, starting with the top (get_top())
class json {
    public:
        json();
        json(string input);
        node *get_top();
        bool populate(string input);
        bool clear();
        void pretty_print();
    private:
        node *head;
        bool is_populated;
        bool is_valid;
};
