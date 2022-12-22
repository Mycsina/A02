#include <algorithm>
#include <fstream>
#include <iostream>
#include <string>
#include <cmath>
#include <vector>
#include <queue>
#include <stack>
#include <thread>

using namespace std;
#define _max_word_size_ 32

class node
{
public:
    node(const wstring &word) : word(word)
    {
        parent = nullptr;
        visited = false;
        representative = this;
        vertices = 1;
        edges = 0;
    }
    wstring word;
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
        // Makes the dict only need to be resized once.
        size = 65536;
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
    void add(const wstring &word)
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
                // Linear probing is the fastest way.
                // Probably because it uses the cache more efficiently.
                // And that matters the most when the table is huge and we have memory to spare.
                index = (index + 1) % size;
            }
            create(index, word);
        }
    }
    node *get(const wstring &word)
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
            index = (index + 1) % size;
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
    int DFS(node *from, node *to, int maximum_depth = 0)
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
    void list_connected_components(const wstring &word)
    {
        vector<node *> components;
        node *vertex = get(word);
        if (vertex == nullptr)
        {
            cout << "Word not found" << endl;
            return;
        }
        node *representative = find(vertex);
        for (unsigned int i = 0; i < size; i++)
        {
            if (words[i] != nullptr && find(words[i]) == representative)
            {
                components.push_back(words[i]);
            }
        }
        wcout << "Belonging to same connected component as " << word << "are:" << endl;
        for (size_t i = 0; i < components.size(); i++)
        {
            wcout << components[i]->word << "\n";
        }
    }
    // hash table statistics
    double get_load_factor()
    {
        return (double)entries / size;
    }
    int get_collisions()
    {
        unsigned int collisions = 0;
        for (unsigned int i = 0; i < size; i++)
        {
            if (words[i] != nullptr)
            {
                if (hash(words[i]->word) != i)
                {
                    collisions++;
                }
            }
        }
        return collisions;
    }
    vector<bool> get_distribution()
    {
        vector<bool> distribution;
        for (unsigned int i = 0; i < size; i++)
        {
            if (words[i] != nullptr)
            {
                distribution.push_back(true);
            }
            else
            {
                distribution.push_back(false);
            }
        }
        return distribution;
    }
    // graph statistics
    int get_connected_components()
    {
        int components = 0;
        for (unsigned int i = 0; i < size; i++)
        {
            if (words[i] != nullptr)
            {
                if (words[i]->representative == words[i])
                {
                    components++;
                }
            }
        }
        return components;
    }
    int get_diameter(node *n)
    {
        int diameter = 0;
        node *max = nullptr;
        for (unsigned int i = 0; i < size; i++)
        {
            if (words[i] != nullptr)
            {
                int distance = BFS(words[i], n);
                if (distance > diameter)
                {
                    diameter = distance;
                    max = words[i];
                }
            }
        }
        // BFS data is wiped out every run.
        BFS(n, max);
        node *res = max;
        if (res == nullptr)
        {
            wcout << "No connected words." << endl;
            return 0;
        }
        while (res->parent != nullptr)
        {
            wcout << res->word << " -> ";
            res = res->parent;
        }
        wcout << res->word << endl;
        return diameter;
    }

private:
    void create(int index, const wstring &word)
    {
        entries++;
        connected_components++;
        words[index] = new node(word);
    }
    void resize()
    {
        // High resize coefficient to reduce resizes, which are expensive.
        int coeff = 4;
        size *= coeff;
        node **new_words = new node *[size];
        for (unsigned int i = 0; i < size; i++)
        {
            new_words[i] = nullptr;
        }
        for (unsigned int i = 0; i < size / coeff; i++)
        {
            if (words[i] != nullptr)
            {
                int index = hash(words[i]->word);
                if (new_words[index] == nullptr)
                {
                    new_words[index] = words[i];
                }
                else
                {
                    while (new_words[index] != nullptr)
                    {
                        index = (index + 1) % size;
                    }
                }
            }
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
        wcout << n->word << " -> ";
        for (size_t i = 0; i < n->adjacency_list.size(); i++)
        {
            wcout << n->adjacency_list[i]->word << " ";
        }
        wcout << endl;
    }

#define FNV_OFFSET 14695981039346656037UL
#define FNV_PRIME 1099511628211UL
    // Return 64-bit FNV-1a hash for key (NUL-terminated). See description:
    // https://en.wikipedia.org/wiki/Fowler–Noll–Vo_hash_function
    unsigned int hash(const wstring &word)
    {
        uint64_t hash = FNV_OFFSET;
        const wchar_t *key = word.c_str();
        for (const wchar_t *p = key; *p; p++)
        {
            hash ^= (uint64_t)(unsigned char)(*p);
            hash *= FNV_PRIME;
        }
        // Ensure hash is adjusted to the size of the table.
        return (size_t)(hash & (uint64_t)(size - 1));
    }
    // https://github.com/skeeto/hash-prospector#three-round-functions
    // Kept for reference.
    unsigned int hash(int x)
    {
        x ^= x >> 17;
        x *= 0xed5ad4bb;
        x ^= x >> 11;
        x *= 0xac4c1b51;
        x ^= x >> 15;
        x *= 0x31848bab;
        x ^= x >> 14;
        return x;
    }

    unsigned int unhash(int x)
    {
        x ^= x >> 14 ^ x >> 28;
        x *= 0x32b21703;
        x ^= x >> 15 ^ x >> 30;
        x *= 0x469e0db1;
        x ^= x >> 11 ^ x >> 22;
        x *= 0x79a85073;
        x ^= x >> 17;
        return x;
    }
};

int diameter(hashTable **dicts, const wstring &word)
{
    hashTable *dict = dicts[word.size() - 1];
    node *n = dict->get(word);
    return dict->get_diameter(n);
}

bool connected(const wstring &a, const wstring &b)
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

void path_finder(hashTable **dicts, const wstring &start, const wstring &end)
{
    if (start.size() != end.size())
    {
        cout << "Cannot compare different sizes." << endl;
        return;
    }
    hashTable *dict = dicts[start.size() - 1];
    wcout << "Trying to go from " << start << " to " << end << endl;
    node *from = dict->get(end);
    node *to = dict->get(start);
    if (from == nullptr || to == nullptr)
    {
        wcout << "No path found." << endl;
        return;
    }
    int travelled = dict->BFS(from, to);
    cout << "Travelled " << travelled << " nodes. " << endl;
    node *res = to;
    while (res->parent != nullptr)
    {
        wcout << res->word << " -> ";
        res = res->parent;
    }
    wcout << res->word << endl;
}

void connected_components(hashTable **dicts, const wstring &word)
{
    hashTable *dict = dicts[word.size() - 1];
    dict->list_connected_components(word);
}

void end(hashTable **dicts)
{
    ofstream file;
    file.open("stats.txt");
    for (size_t i = 0; i < _max_word_size_; i++)
    {
#if defined(_stats_) || defined(_detail_)
        file << endl;
        file << "Hash Table for " << i + 1 << " letter words" << endl;
        file << "Size: " << dicts[i]->size << endl;
        file << "Load factor: " << dicts[i]->get_load_factor() << endl;
        file << "Collisions: " << dicts[i]->get_collisions() << endl;
#if defined(_detail_)
        vector<bool> distribution = dicts[i]->get_distribution();
        file << "Distribution: " << endl;
        for (size_t j = 0; j < distribution.size(); j++)
        {
            if (distribution[j])
                file << j << " ";
        }
        file << endl;
#endif
#endif
        delete dicts[i];
    }
    file.close();
}

void graph_builder(hashTable *dict)
{
    int sizes = 0;
    // TODO: Optimize this, O(n^1.5) ish isn't good
    for (size_t i = 0; i < dict->size; i++)
    {
        node *from = dict->words[i];
        if (from == nullptr)
            continue;
        if (sizes == 0)
            sizes = from->word.size();
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
    if (sizes != 0)
        cout << "Processed " << sizes + 1 << " letter words" << endl;
}

int main()
{
    setlocale(LC_ALL, ".UTF8");
    hashTable *dicts[_max_word_size_];
    thread threads[_max_word_size_];
    for (size_t i = 0; i < _max_word_size_; i++)
    {
        dicts[i] = new hashTable;
    }
    std::wifstream in("wordlist-big-latest.txt");
    if (!in)
    {
        printf("Error: could not open words file\n");
    }
    wstring word;
    while (in >> word)
    {
        int size = word.size();
        dicts[size - 1]->add(word);
    }
    in.close();
    for (int sizes = 0; sizes < _max_word_size_; sizes++)
    {
        hashTable *dict = dicts[sizes];
        threads[sizes] = thread(graph_builder, dict);
    }
    for (int sizes = 0; sizes < _max_word_size_; sizes++)
    {
        threads[sizes].join();
    }
    path_finder(dicts, L"vidro", L"leite");
    // FIXME: Latin letters are not supported
    cout << "Longest path to "
         << "etano"
         << " is " << endl
         << diameter(dicts, L"etano") << " words long." << endl;
    // #if !defined(_detail_) && !defined(_stats_)
    //     for (unsigned int i = 0; i < dicts[4]->size; i++)
    //     {
    //         if (dicts[4]->words[i] != nullptr)
    //         {
    //             cout << diameter(dicts, dicts[4]->words[i]->word) << endl;
    //         }
    //     }
    // #endif
    // TODO: See graphs
    // TODO: Interesting diameters
    // etano is the extremety of (one of the) main connected component, as it shows up in lots of diameters
    // sitia is the extremety of (one of the) main connected component, as it shows up in lots of diameters

    // connected_components(dicts, L"vidro");
    end(dicts);
}