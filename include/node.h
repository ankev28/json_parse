#include <string>
#include <map>
#include <vector>
#include <iostream>


using namespace std;

typedef enum truetype
{
    invalidtype = -1,
    stringtype   = 0,
    vectortype   = 1,
    maptype      = 2,
    emptytype    = 3,
} truetype;

class node {
    public:
        node(string input);
        node();
        ~node();
        void settype(int val);
        int gettype();
        string &get_string();
        vector<node> &get_vector();
        map<string, node> &get_map();
    private:
        int type;
        void breakdown_recursively(string json);
        map<string, node> map_data;
        vector<node> list_data;
        string str_data;
        string self_string;
};
