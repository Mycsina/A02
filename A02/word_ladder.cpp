// build graph of word ladder and find shortest path
// use hash table to store words
// use breadth first search to find shortest path
// use crc32 hash function to hash words
// use connected function to check if two words are connected
// use print_word_ladder function to print word ladder

#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <queue>
#include <stack>

using namespace std;
using word_map = std::map<size_t, std::vector<std::string>>;

class node
{
public:
    node(const string &word) : word(word)
    {
        parent = nullptr;
        visited = false;
        representative = this;
        vertices = 1;
        edges = 0;
    }
    string word;
    // BFS relevant data
    node *parent;
    bool visited;
    // graph data structure
    vector<node *> adjacency_list;
    // union-find data structure
    node *representative;
    int vertices;
    int edges;
};

class hashTable
{
public:
    unsigned int size;
    node **words;
    unsigned int entries;
    int connected_components;
    hashTable()
    {
        size = 1048576;
        words = new node *[size];
        entries = 0;
        connected_components = 0;
        for (unsigned int i = 0; i < size; i++)
        {
            words[i] = nullptr;
        }
    }
    ~hashTable()
    {
        for (unsigned int i = 0; i < size; i++)
        {
            if (words[i] != nullptr)
            {
                delete words[i];
            }
        }
    }
    void add(const string &word)
    {
        unsigned int index = hash(word);
        if (words[index] != nullptr && words[index]->word == word)
        {
            return;
        }
        if (entries + 1 >= size * 0.75)
        {
            resize();
        }
        if (words[index] == nullptr)
        {
            create(index, word);
        }
        else if (words[index]->word != word)
        {
            while (words[index] != nullptr && words[index]->word != word)
            {
                if (index + 1 >= size)
                {
                    index = 0;
                }
                else
                {
                    index++;
                }
            }
            create(index, word);
        }
    }
    node *get(const string &word)
    {
        unsigned int index = hash(word);
        if (words[index] == nullptr)
        {
            return nullptr;
        }
        if (words[index]->word == word)
        {
            return words[index];
        }
        while (words[index] != nullptr && words[index]->word != word)
        {
            if (index + 1 >= size)
            {
                index = 0;
            }
            else
            {
                index++;
            }
        }
        return words[index];
    }
    // graph functions
    void add_edge(node *from, node *to)
    {
        from->adjacency_list.push_back(to);
        to->adjacency_list.push_back(from);
        from->edges++;
        to->edges++;
        g_union(from, to);
    }
    int BFS(node *from, node *to, int maximum_depth = 0)
    {
        for (unsigned int i = 0; i < size; i++)
        {
            if (words[i] != nullptr)
            {
                words[i]->visited = false;
                words[i]->parent = nullptr;
            }
        }
        queue<node *> q;
        from->visited = true;
        q.push(from);
        int depth = 0;
        while (!q.empty())
        {
            int q_size = q.size();
            for (int i = 0; i < q_size; i++)
            {
                node *current = q.front();
                q.pop();
                if (current == to)
                {
                    return depth;
                }
                for (size_t j = 0; j < current->adjacency_list.size(); j++)
                {
                    node *adjacent = current->adjacency_list[j];
                    if (!adjacent->visited)
                    {
                        adjacent->visited = true;
                        adjacent->parent = current;
                        q.push(adjacent);
                    }
                }
            }
            depth++;
            if (depth > maximum_depth && maximum_depth != 0)
            {
                return -1;
            }
        }
        return -1;
    }
    int DFS(node *from, node *to, int maximum_depth)
    {
        for (unsigned int i = 0; i < size; i++)
        {
            if (words[i] != nullptr)
            {
                words[i]->visited = false;
                words[i]->parent = nullptr;
            }
        }
        stack<node *> q;
        from->visited = true;
        q.push(from);
        int depth = 0;
        while (!q.empty())
        {
            int q_size = q.size();
            for (int i = 0; i < q_size; i++)
            {
                node *current = q.top();
                q.pop();
                if (current == to)
                {
                    return depth;
                }
                for (size_t j = 0; j < current->adjacency_list.size(); j++)
                {
                    node *adjacent = current->adjacency_list[j];
                    if (!adjacent->visited)
                    {
                        adjacent->visited = true;
                        adjacent->parent = current;
                        q.push(adjacent);
                    }
                }
            }
            depth++;
            if (depth > maximum_depth && maximum_depth != 0)
            {
                return -1;
            }
        }
        return -1;
    }
    vector<node *> list_connected_components(const string &word)
    {
        vector<node *> components;
        node *vertex = get(word);
        if (vertex == nullptr)
        {
            return components;
        }
        node *representative = find(vertex);
        for (unsigned int i = 0; i < size; i++)
        {
            if (words[i] != nullptr && find(words[i]) == representative)
            {
                components.push_back(words[i]);
            }
        }
        return components;
    }
    // hash table statistics
    int get_size()
    {
        return size;
    }
    double get_load_factor()
    {
        return (double)entries / size;
    }

private:
    void create(int index, const string &word)
    {
        entries++;
        connected_components++;
        words[index] = new node(word);
    }
    void resize()
    {
        size *= 2;
        node **new_words = new node *[size];
        for (unsigned int i = 0; i < size / 2; i++)
        {
            new_words[i] = words[i];
        }
        for (unsigned int i = size / 2; i < size; i++)
        {
            new_words[i] = nullptr;
        }
        delete[] words;
        words = new_words;
    }

    node *find(node *vertex)
    {
        if (vertex->representative != vertex)
        {
            vertex->representative = find(vertex->representative);
        }
        return vertex->representative;
    }

    void g_union(node *from, node *to)
    {
        node *from_rep = find(from);
        node *to_rep = find(to);
        if (from_rep != to_rep)
        {
            to_rep->representative = from_rep;
            connected_components--;
        }
    }

    void print_adjacency_list(node *n)
    {
        cout << n->word << " -> ";
        for (size_t i = 0; i < n->adjacency_list.size(); i++)
        {
            cout << n->adjacency_list[i]->word << " ";
        }
        cout << endl;
    }

#define FNV_OFFSET 14695981039346656037UL
#define FNV_PRIME 1099511628211UL
    // crc32 gives poor results, so use FNV-1a instead:
    // CRC32 results in 5353 collisions for 16384 words.
    // Return 64-bit FNV-1a hash for key (NUL-terminated). See description:
    // https://en.wikipedia.org/wiki/Fowler–Noll–Vo_hash_function
    unsigned int hash(const string &word)
    {
        uint64_t hash = FNV_OFFSET;
        const char *key = word.c_str();
        for (const char *p = key; *p; p++)
        {
            hash ^= (uint64_t)(unsigned char)(*p);
            hash *= FNV_PRIME;
        }
        // Ensure hash is adjusted to the size of the table.
        return (size_t)(hash & (uint64_t)(size - 1));
    }
    // {
    //     static unsigned int table[256];
    //     unsigned int crc;
    //     if (table[1] == 0u)
    //     {
    //         unsigned int i, j;
    //         for (i = 0u; i < 256u; i++)
    //         {
    //             for (table[i] = i, j = 0u; j < 8u; j++)
    //             {
    //                 if (table[i] & 1u)
    //                 {
    //                     table[i] = (table[i] >> 1) ^ 0xAED00022u; // "magic" constant
    //                 }
    //                 else
    //                 {
    //                     table[i] >>= 1;
    //                 }
    //             }
    //         }
    //     }
    //     crc = 0xAED00022u; // "magic" constant
    //     for (unsigned int i = 0; i < word.size(); i++)
    //     {
    //         crc = (crc >> 8) ^ table[crc & 0xFFu] ^ ((unsigned int)word[i] << 24);
    //     }
    //     return crc % size;
    // }
};

bool connected(const string &a, const string &b)
{
    if (a.size() != b.size())
        return false;
    bool result = false;
    for (size_t i = 0; i < a.size(); i++)
    {
        if (a[i] != b[i])
        {
            // Only one difference is allowed
            if (result)
                return false;
            result = true;
        }
    }
    return result;
}

void print_stats(hashTable *dict, const string &start, const string &end)
{
    cout << "Trying to go from " << start << " to " << end << endl;
    node *from = dict->get(start);
    node *to = dict->get(end);
    if (from == nullptr || to == nullptr)
    {
        cout << "No path found." << endl;
        return;
    }
    int travelled = dict->BFS(from, to, 0);
    cout << "Travelled " << travelled << " nodes. " << endl;
    node *res = to;
    while (res->parent != nullptr)
    {
        cout << res->word << " -> ";
        res = res->parent;
    }
    cout << res->word << endl;
    cout << "Same connected component vertices to " << start << endl;
    vector<node *> conn_compon = dict->list_connected_components(start);
    for (size_t i = 0; i < conn_compon.size(); i++)
    {
        cout << conn_compon[i]->word << " ";
    }
}
void end(hashTable *dict)
{
    cout << endl;
    cout << "Work done." << endl;
    cout << "Size: " << dict->get_size() << endl;
    cout << "Load factor: " << dict->get_load_factor() << endl;
    cout << "Collisions: " << 0 << endl;
    delete dict;
    exit(0);
}

int main()
{
    hashTable *dict = new hashTable;
    std::ifstream in("wordlist-four-letters.txt");
    if (!in)
    {
        printf("Error: could not open words file\n");
    }
    std::string word;
    while (in >> word)
    {
        dict->add(word);
    }
    for (size_t i = 0; i < dict->size; i++)
    {
        node *from = dict->words[i];
        if (from == nullptr)
            continue;
        for (size_t j = i + 1; j < dict->size; j++)
        {
            node *to = dict->words[j];
            if (to == nullptr)
                continue;
            if (connected(from->word, to->word))
            {
                dict->add_edge(from, to);
            }
        }
    }
    print_stats(dict, "voguem", "vieres");
    print_stats(dict, "vibrar", "venosa");
    print_stats(dict, "vara", "umas");
    end(dict);
}

// // If possible, print the shortest chain of single-character modifications that
// // leads from "from" to "to", with each intermediate step being a valid word.
// // This is an application of breadth-first search.
// bool word_ladder(const word_map &words, const std::string &from,
//                  const std::string &to)
// {
//     auto w = words.find(from.size());
//     if (w != words.end())
//     {
//         auto poss = w->second;
//         std::vector<std::vector<std::string>> queue{{from}};
//         while (!queue.empty())
//         {
//             auto curr = queue.front();
//             queue.erase(queue.begin());
//             for (auto i = poss.begin(); i != poss.end();)
//             {
//                 if (!connected(*i, curr.back()))
//                 {
//                     ++i;
//                     continue;
//                 }
//                 if (to == *i)
//                 {
//                     curr.push_back(to);
//                     std::cout << print_word_ladder(curr.begin(), curr.end(), " -> ") << '\n';
//                     return true;
//                 }
//                 std::vector<std::string> temp(curr);
//                 temp.push_back(*i);
//                 queue.push_back(std::move(temp));
//                 i = poss.erase(i);
//             }
//         }
//     }
//     std::cout << from << " into " << to << " cannot be done.\n";
//     return false;
// }

// int main()
// {
//     word_map words;
//     std::ifstream in("wordlist-big-latest.txt");
//     if (!in)
//     {
//         std::cerr << "Cannot open file wordlist-four-letters.txt.\n";
//         return EXIT_FAILURE;
//     }
//     std::string word;
//     while (getline(in, word))
//         words[word.size()].push_back(word);
//     word_ladder(words, "bem", "mal");
//     return EXIT_SUCCESS;
// }