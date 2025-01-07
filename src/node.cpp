#include "node.h"
#include <stack>
#include <stdexcept>
#include <sstream>

void node::settype(int val) {
    type = val;
}

string preprocess(string input, int &type) {
    stack<char> cleanup;
    string ret = "";
    map<char, char> closed = {{'}','{'}, {']','['}};
    map<char, char> open = {{'{','}'}, {'[',']'}};
    
    size_t begin = 0;
    while (begin < input.size() && (input[begin] == ' ' || input[begin] == '\n' || input[begin] == '\t')) {
        begin++;
    }

    if (input.empty()) {
        type = emptytype;
        return "";
    }

    if (begin >= input.size()) {
        type = invalidtype;
        return "";
        // basically invalid input here
    } 

    input.replace(0,begin, "");

    if (open.find(input[0]) != open.end()) {
        cleanup.push(input[0]);
    }
    ret.push_back(input[0]);
    if (ret.back() == '{') {
        type = maptype;
    }
    else if (ret.back() == '[') {
        type = vectortype;
    }
    else {
        type = stringtype; // even if it's true/false, null, a number, or "xyz", we say it's a string is all
        // we can actually just pick up the whole thing here if we know it's going to be a string only
        for (size_t i = 1 ; i < input.size(); i++) {
            ret.push_back(input[i]);
        }
    }

    size_t i = 1;
    while (i < input.size() && cleanup.size() > 0) {
        if (open.find(input[i]) != open.end() || (input[i] == '"' && cleanup.top() != '"')) {
            cleanup.push(input[i]);
            ret.push_back(input[i]);
            i++;
            continue;
        }
        if ((closed.find(input[i]) != closed.end() && cleanup.top() == closed.at(input[i])) || (input[i] == '"' && cleanup.top() == '"')) {
            if (input[i-1] != '\\') {
                cleanup.pop();
            }
            ret.push_back(input[i]);
            i++;
            continue;
        }
        if ((input[i] == ' ' || input[i] == '\n' || input[i] == '\t') && cleanup.top() != '"') {
            i++;
            continue;
        }
        ret.push_back(input[i]);
        i++;
    }
    if (cleanup.size() == 0) {
        return ret;
    }
    type = invalidtype;
    return "";
}

// we expect the string to be surrounded in "" if we call this function.
string strip_outer_quotation_marks(string input) {
    string ret = input;

    if (ret.size() >= 2 && ret.at(0) == '"' && ret.at(ret.size() - 1) == '"') {
        ret.erase(0,1);
        ret.erase(ret.size() - 1);
    }
    return ret;
}

// thanks google for recommending this method, although i wouldn't have thought of it myself
bool is_number(const string & s) {
    istringstream iss(s);
    double d;
    return iss >> d && iss.eof();
}

// if we determine it's a string, then it can either only be the following:
// word surrounded by ""
// number (ie. 30)
// null
// true
// false
bool string_validate(string input) {
    bool ret = false;
    if (input.size() >= 2 && input.at(0) == '"' && input.at(input.size() - 1) == '"') {
        ret = true;
    }
    else if (input == "null" || input == "true" || input == "false") {// the exact word
        ret = true;
    }
    else {
        if (is_number(input)) {
            ret = true;
        }
    }
    return ret;
}

// for getting the key and pair "strings", will also tell if it's valid by is_valid
pair<string, string> get_key_and_pair(string raw, bool &is_valid) {
    bool found = false;
    stack<char> key_pair_divider;
    string key;
    string val;
    map<char, char> closed = {{'}','{'}, {']','['}};
    map<char, char> open = {{'{','}'}, {'[',']'}};
    pair<string, string> ret("", "");

    size_t i = 0;
    while (i < raw.size()) {
        if (found == false) {
            if (key_pair_divider.size() == 0 && raw.at(i) == ':') {
                found = true;
                i++;
                continue;
            }
            if (open.find(raw.at(i)) != open.end() || (raw.at(i) == '"' && (key_pair_divider.empty() || (!key_pair_divider.empty() && key_pair_divider.top() != '"')))) {
                key_pair_divider.push(raw.at(i));
            }
            else if ((closed.find(raw.at(i)) != closed.end() && closed.at(raw.at(i)) == key_pair_divider.top()) || (raw.at(i) == '"' && !key_pair_divider.empty() && key_pair_divider.top() == '"')) {
                key_pair_divider.pop();
            }
            key.push_back(raw.at(i));
            i++;
            continue;
        }
        else {
            if (open.find(raw.at(i)) != open.end() || (raw.at(i) == '"' && (key_pair_divider.empty() || (!key_pair_divider.empty() && key_pair_divider.top() != '"')))) {
                key_pair_divider.push(raw.at(i));
            }
            else if ((closed.find(raw.at(i)) != closed.end() && closed.at(raw.at(i)) == key_pair_divider.top()) || (raw.at(i) == '"' && !key_pair_divider.empty() && key_pair_divider.top() == '"')) {
                key_pair_divider.pop();
            }
            val.push_back(raw.at(i));
            i++;
            continue;
        }
    }

    if (!found) { // it just looks like {"stuff"}, no : inside
        is_valid = false;
        return ret;
    }
    is_valid = true;
    ret.first = key;
    ret.second = val;
    return ret;
}

void node::breakdown_recursively(string json) {
    int cur_type = invalidtype;
    stack<char> parse;
    string raw;
    string key;
    string val;
    // obscure_type ret(json);
    map<char, char> closed = {{'}','{'}, {']','['}};
    map<char, char> open = {{'{','}'}, {'[',']'}};
    size_t i = 0;
    bool check_map;

    string input = preprocess(json, cur_type);
    if (cur_type == invalidtype) {
        settype(invalidtype);
        return;
    }

    settype(cur_type);

    if (i < input.size()) {
        if (cur_type == maptype) { // it's a map with key(s) and value(s)
            // first get the key, which is expected to just be a string
            parse.push(input[i]);
            i++;
            // need to decompose first if it's a "list" still within the json (it's not really a list, but it'll have commas and stuff separating pairs)
            while (i < input.size()) {
                // end of list
                if (parse.size() == 1 && parse.top() == '{' && input[i] == '}') {
                    pair<string, string> key_val = get_key_and_pair(raw, check_map);
                    if (!check_map) {
                        cout << "invalid input map formatting" << endl;
                        cout << raw << endl;
                        return;
                    }
                    string key_clean = strip_outer_quotation_marks(key_val.first);
                    node *val_to_node = new node(key_val.second);
                    pair<string, node> to_insert = make_pair(key_clean, *val_to_node);

                    map_data.insert(to_insert);
                    break;
                }
                // we find an open symbol and it's not in a string element, push
                else if (open.find(input[i]) != open.end() && parse.top() != '"') {
                    parse.push(input[i]);
                }
                // we find a matching closed symbol, pop
                else if (closed.find(input[i]) != closed.end() && parse.top() == closed.at(input[i])) {
                    parse.pop();
                }
                // start of a string element in array
                else if (input[i] == '"' && parse.top() != '"') {
                    parse.push(input[i]);
                }
                // end of a string element in array
                else if (input[i] == '"' && parse.top() == '"') {
                    if (input[i-1] != '\\') {
                        parse.pop();
                    }
                }
                else if ((input[i] == ',') && parse.size() == 1) {
                    i++;
                    pair<string, string> key_val = get_key_and_pair(raw, check_map);
                    if (!check_map) {
                        cout << "invalid input map formatting" << endl;
                        cout << raw << endl;
                        return;
                    }
                    string key_clean = strip_outer_quotation_marks(key_val.first);
                    node *val_to_node = new node(key_val.second);
                    pair<string, node> to_insert = make_pair(key_clean, *val_to_node);

                    map_data.insert(to_insert);
                    raw.clear();
                    continue;
                }
                raw.push_back(input[i]);
                i++;
            }
        }

        else if (cur_type == vectortype) {// it's a list of elements
            parse.push(input[i]);
            i++;
            while (i < input.size()) {
                // end of list
                if (parse.size() == 1 && parse.top() == '[' && input[i] == ']') {
                    node *to_push_back = new node(raw);
                    list_data.push_back(*to_push_back);
                    break;
                }
                // we find an open symbol and it's not in a string element, push
                else if (open.find(input[i]) != open.end() && parse.top() != '"') {
                    parse.push(input[i]);
                }
                // we find a matching closed symbol, pop
                else if (closed.find(input[i]) != closed.end() && parse.top() == closed.at(input[i])) {
                    parse.pop();
                }
                // start of a string element in array
                else if (input[i] == '"' && parse.top() != '"') {
                    parse.push(input[i]);
                }
                // end of a string element in array
                else if (input[i] == '"' && parse.top() == '"') {
                    parse.pop();
                }
                else if ((input[i] == ',') && parse.size() == 1) {
                    i++;
                    node *to_push_back = new node(raw);
                    list_data.push_back(*to_push_back);
                    raw.clear();
                    continue;
                }
                raw.push_back(input[i]);
                i++;
            }
        }
        else { // assume string
            bool is_valid = string_validate(input);
            if (is_valid) {
                str_data = strip_outer_quotation_marks(input);
            }
            else {
                settype(invalidtype);
                cout << "invalid input string formatting" << endl;
                cout << input << endl;
                return;
            }
        }
    }
}


node::node(string input) {
    self_string = input;
    breakdown_recursively(input);
}

node::~node() {

}

string cur_type_to_string(int cur_type) {
    switch(cur_type) {
        case invalidtype:
            return "invalid";
        case stringtype:
            return "string";
        case vectortype:
            return "vector<node>";
        case maptype:
            return "map<string, node>";
        default:
            return "unknown type";
        }
}


string & node::get_string() {
    if (type != stringtype) {
        string error_msg = "Invalid get_string() operation, type is "; 
        error_msg.append(cur_type_to_string(type));
        throw logic_error(error_msg);
    }

    return str_data;
}

vector<node> & node::get_vector() {
    if (type != vectortype) {
        string error_msg = "Invalid get_vector() operation, type is "; 
        error_msg.append(cur_type_to_string(type));
        throw logic_error(error_msg);
    }

    return list_data;
}

map<string, node> & node::get_map() {
    if (type != maptype) {
        string error_msg = "Invalid get_map() operation, type is "; 
        error_msg.append(cur_type_to_string(type));
        throw logic_error(error_msg);;
    }

    return map_data;
}

int node::gettype() {
    return type;
}
