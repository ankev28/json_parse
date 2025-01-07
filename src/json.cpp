#include "json.h"


json::json() {
    head = NULL;
    is_populated = false;
    is_valid = true;
}

json::json(string input) {
    head = new node(input);
    is_valid = true;
    if (head->gettype() == invalidtype) {
        is_valid = false;
    }
    is_populated = true;
}

node * json::get_top() {
    return head;
}

// only populate if we know the json is empty at the moment.
bool json::populate(string input) {
    if (is_populated) {
        return false;
    }
    is_valid = true;
    head = new node(input);
    if (head->gettype() == invalidtype) {
        is_valid = false;
    }
    is_populated = true;

    return true;
}

// basically just frees the top node, since it's all stl it should automatically deallocate anyways
bool json::clear() {
    if (is_populated == true) {
        delete head;
    }
    is_valid = true;
    is_populated = false;
    return true;
}

// an empty vector will always have a emptytype node inside (ie. size 1, but the node didn't have data)
bool is_vector_node_empty(vector<node> v) {
    if (v.size() > 1) {
        return false;
    }
    if (v.size() == 1) {
        if (v.at(0).gettype() == emptytype) {
            return true;
        }
    }
    return false;
}

// recursive helper for pretty_print()
// currently may be buggy with formatting though
void traverse(int depth, node cur, bool begin_with_tab) {
    int cur_type = cur.gettype();
    string tab = "    ";

    if (cur_type == invalidtype) {
        return;
    }
    else if (cur_type == stringtype) {
        cout << '"' << cur.get_string() << '"';
    }
    else if (cur_type == vectortype) {
        if (!begin_with_tab) {
            for (int i = 0; i < depth; i++) {
                cout << tab;
            }
        }
        if (is_vector_node_empty(cur.get_vector())) {
            if (begin_with_tab) {
                cout << '[' << ']';
            }
            else {
                cout << '[' << ']' << endl;
            }
        }
        else {
            cout << '[' << endl;
            for (size_t i = 0; i < cur.get_vector().size(); i++) {
                for (int j = 0; j < depth + 1; j++) {
                    cout << tab;
                }
                traverse(depth+1, cur.get_vector().at(i), true);
                if (i < cur.get_vector().size() - 1) {
                    cout << ',' << endl;
                }
            }
            cout << endl;
            for (int i = 0; i < depth; i++) {
                cout << tab;
            }
            if (begin_with_tab) {
                cout << ']';
            }
            else {
                cout << ']' << endl;
            }
        }
    }
    else if (cur_type == maptype) {
        cout << '{' << endl;

        map<string, node>::iterator it;
        map<string, node>::iterator final_it = cur.get_map().end();
        --final_it;

        for (it = cur.get_map().begin(); it != cur.get_map().end(); it++) {
            for (int i = 0; i < depth + 1; i++) {
                cout << tab;
            }

            cout << '"' << it->first << "\" : ";
            traverse(depth+1, cur.get_map().at(it->first), true);
            if (it != final_it) {
                cout << ',' << endl;
            }
        }
        cout << endl;
        for (int i = 0; i < depth; i++) {
            cout << tab;
        }
        cout << '}';
    }
}

// printing in terminal basically. good for debugging and stuff
void json::pretty_print() {
    traverse(0, *head, false);
    cout << endl;
}
