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

using namespace std;
using word_map = std::map<size_t, std::vector<std::string>>;

class node
{
public:
    node(const string &word) : word(word) {}
    string word;
    vector<node *> neighbors;
};

class entry
{
public:
    entry(const string &pattern) : pattern(pattern) {}
    string pattern;
    vector<node *> words;
};

class hashTable
{
public:
    hashTable() {}
    ~hashTable()
    {
        for (int i = 0; i < size; i++)
        {
            if (patterns[i] != nullptr)
            {
                for (int j = 0; j < patterns[i]->words.size(); j++)
                {
                    delete patterns[i]->words[j];
                }
                delete patterns[i];
            }
        }
        delete[] patterns;
    }
    void add(const string &word)
    {
        for (int i = 0; i < word.size(); i++)
        {
            string pattern = word;
            pattern[i] = '*';
            unsigned int index = hash(pattern);
            if (index >= size * 0.75)
            {
                resize();
            }
            if (patterns[index] == nullptr)
            {
                patterns[index] = new entry(pattern);
            }
            else if (patterns[index]->pattern != pattern)
            {
                while (patterns[index] != nullptr && patterns[index]->pattern != pattern)
                {
                    index++;
                }
                if (patterns[index] == nullptr)
                {
                    patterns[index] = new entry(pattern);
                }
            }
            patterns[index]->words.push_back(new node(word));
        }
    }
    vector<node *> get(const string &word)
    {
        vector<node *> result;
        for (int i = 0; i < word.size(); i++)
        {
            string pattern = word;
            pattern[i] = '*';
            unsigned int index = hash(pattern);
            while (patterns[index] != nullptr && patterns[index]->pattern != pattern)
            {
                index++;
            }
            if (patterns[index] != nullptr)
            {
                for (int j = 0; j < patterns[index]->words.size(); j++)
                {
                    if (patterns[index]->words[j]->word == word)
                    {
                        result.push_back(patterns[index]->words[j]);
                    }
                }
            }
        }
        return result;
    }
    // hash table statistics
    int get_size()
    {
        return size;
    }
    double get_load_factor()
    {
        int count = 0;
        for (int i = 0; i < size; i++)
        {
            if (patterns[i] != nullptr)
            {
                count++;
            }
        }
        return count / size;
    }
    int get_collisions()
    {
        int count = 0;
        for (int i = 0; i < size; i++)
        {
            if (patterns[i] != nullptr)
            {
                for (int j = 0; j < patterns[i]->words.size(); j++)
                {
                    if (hash(patterns[i]->words[j]->word) != i)
                    {
                        count++;
                    }
                }
            }
        }
        return count;
    }

private:
    entry *patterns[256];
    int size = 256;

    void resize()
    {
        entry **new_patterns = new entry *[size * 2];
        for (int i = 0; i < size; i++)
        {
            new_patterns[i] = patterns[i];
        }
        for (int i = size; i < size * 2; i++)
        {
            new_patterns[i] = nullptr;
        }
        delete[] patterns;
        *patterns = *new_patterns;
        size *= 2;
    }

    unsigned int hash(const string &word)
    {
        static unsigned int table[256];
        unsigned int crc;
        if (table[1] == 0u)
        {
            unsigned int i, j;
            for (i = 0u; i < 256u; i++)
            {
                for (table[i] = i, j = 0u; j < 8u; j++)
                {
                    if (table[i] & 1u)
                    {
                        table[i] = (table[i] >> 1) ^ 0xAED00022u; // "magic" constant
                    }
                    else
                    {
                        table[i] >>= 1;
                    }
                }
            }
        }
        crc = 0xAED00022u; // "magic" constant
        for (unsigned int i = 0; i < word.size(); i++)
        {
            crc = (crc >> 8) ^ table[crc & 0xFFu] ^ ((unsigned int)word[i] << 24);
        }
        return crc % size;
    }
};

bool connected(const string &a, const string &b)
{
    if (a.size() != b.size())
        return false;
    bool result = false;
    for (int i = 0; i < a.size(); i++)
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

template <typename iterator_type, typename separator_type>
string print_word_ladder(iterator_type start, iterator_type end, separator_type separator)
{
    string res;
    while (start != end)
    {
        res += *start;
        start++;
        if (start != end)
            res += separator;
    }
    return res;
}

// If possible, print the shortest chain of single-character modifications that
// leads from "from" to "to", with each intermediate step being a valid word.
// This is an application of breadth-first search.
bool word_ladder(const word_map &words, const std::string &from,
                 const std::string &to)
{
    auto w = words.find(from.size());
    if (w != words.end())
    {
        auto poss = w->second;
        std::vector<std::vector<std::string>> queue{{from}};
        while (!queue.empty())
        {
            auto curr = queue.front();
            queue.erase(queue.begin());
            for (auto i = poss.begin(); i != poss.end();)
            {
                if (!connected(*i, curr.back()))
                {
                    ++i;
                    continue;
                }
                if (to == *i)
                {
                    curr.push_back(to);
                    std::cout << print_word_ladder(curr.begin(), curr.end(), " -> ") << '\n';
                    return true;
                }
                std::vector<std::string> temp(curr);
                temp.push_back(*i);
                queue.push_back(std::move(temp));
                i = poss.erase(i);
            }
        }
    }
    std::cout << from << " into " << to << " cannot be done.\n";
    return false;
}

int main()
{
    word_map words;
    std::ifstream in("wordlist-four-letters.txt");
    if (!in)
    {
        std::cerr << "Cannot open file wordlist-four-letters.txt.\n";
        return EXIT_FAILURE;
    }
    std::string word;
    while (getline(in, word))
        words[word.size()].push_back(word);
    word_ladder(words, "boy", "man");
    word_ladder(words, "girl", "lady");
    word_ladder(words, "john", "jane");
    word_ladder(words, "child", "adult");
    word_ladder(words, "cat", "dog");
    word_ladder(words, "lead", "gold");
    word_ladder(words, "white", "black");
    word_ladder(words, "bubble", "tickle");
    return EXIT_SUCCESS;
}