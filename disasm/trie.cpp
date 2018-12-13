// not functional as of now

#include "trie.h"

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

    void build_trie(std::vector<std::vector<uint8_t> > gadgets[],std::vector<std::vector<uint64_t> > addresses ,  int no_gadgets);
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

Trie::~Trie() {
  for (int i=0; i<children.size(); ++i) {
    delete children[i];
  }
}

void Trie::build_trie(std::vector<std::vector<uint8_t>> gadgets[], std::vector<std::vector<uint64_t> > addresses, int no_gadgets) {
  for (int i=0; i<no_gadgets; ++i) {
    insert(gadgets[i], addresses[i]);
  }
}

void Trie::insert(std::vector<std::vector<uint8_t>> gadget, std::vector<uint64_t> addresses) {
  map<std::vector<uint8_t>, Node*> *current_tree = &head.children;
  map<std::vector<uint8_t>, Node*>::iterator it;

  for (int i=0; i<gadget.size(); ++i) {
    std::vector<uint8_t> insn = gadget[i];

    if ((it = current_tree->find(insn)) != current_tree->end()) {
      current_tree = &it->second->children;
      continue;
    }

    if (it == current_tree->end()) {
      // Display inserting position in the tree, for debug use
      //
      // cout << "Inserting " << ch << endl;
      // cout << "on layer " << endl;
      // map<char, Node*>::iterator temp = current_tree->begin();
      // for (; temp != current_tree->end(); ++temp)
      //   cout << temp->first << endl;

      Node* new_node = new Node();
      for(int j=0; j< insn.size(); j++) {
          new_node->instruction.push_back(insn[j]);
      }
      new_node->address = addresses[i];
      (*current_tree)[insn] = new_node;

      // For continuous inserting a word.
      current_tree = &new_node->children;
      
      // For the ease of memory clean up.
      children.push_back(new_node);
    }
  }
}

uint64_t Trie::search(std::vector<std::vector<uint8_t>> gadget, bool &result) {
  map<std::vector<uint8_t>, Node*> current_tree = head.children;
  map<std::vector<uint8_t>, Node*>::iterator it;
  uint64_t address = 0;

  bool f = false;
  for (int i=0; i<gadget.size(); ++i) {
    if ((it = current_tree.find(gadget[i])) == current_tree.end()) {
      address = 0;
      result = false;
      return address;
    }
    current_tree = it->second->children;
    address = it->second->address;
  }

  result= true;
  return address;
}

void Trie::print_tree(map<std::vector<uint8_t>, Node*> tree) {
  if (tree.empty()) {
    return;
  }

  for (map<std::vector<uint8_t>, Node*>::iterator it=tree.begin(); it!=tree.end(); ++it) {
    for(int i=0; i < it->first.size() ;i++) {
        printf("%x ", it->first[i]);
    }
    cout << std::endl;
    print_tree(it->second->children);
  }
}

void Trie::print() {
  map<std::vector<uint8_t>, Node*> current_tree = head.children;
  print_tree(current_tree);
}
