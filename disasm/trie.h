#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <stdint.h>

using namespace std;

struct Node {
    // Address (EIP) of this instruction
    uint64_t address;

    // Size of this instruction (number of instruction)
    uint16_t size;

    // Machine instruction of this instruction, with number of instruction indicated by @size above
    std::vector<uint8_t> instruction;

    int count;
    map<std::vector<uint8_t>, Node*> children;
};

class Trie {
  public:
    Trie() { head.size = 0; };
    ~Trie();

    void build_trie(std::vector<std::vector<std::vector<uint8_t> > > gadgets,std::vector<std::vector<uint64_t> > addresses ,  int no_gadgets);
    void insert(std::vector<std::vector<uint8_t> > gadget, std::vector<uint64_t>  address);
    uint64_t search(std::vector<std::vector<uint8_t> > gadget, bool &result);

    void print_tree(map<std::vector<uint8_t>, Node*> tree);
    void print();

  protected:
    Node head;
    // Keep all newly created node in an array, for the ease of
    // memory release.
    vector<Node*> children;
};


extern Trie trie;