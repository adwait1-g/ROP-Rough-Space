/* This is a test program for storing the harvested gadgets in a trie for fast search.
 * Each node in the trie has the machine code of the particular instruction, no of bytes,
 * address of instruction and children.
 * Vectors have been used for each of these and hence these all dynamic attributes
 * The search() function is the main highlight of the trie. Partial gadgets will not be found.
 * When a gadget IS found, the address of the first instruction of the gadget is returned along with 
 * the gadget itself. (can be extended later as address of each instruction is being stored when the 
 * tree is constructed). So when it is integrated in main.cpp with the elfparse logic, we'll be inserting 
 * gadgets backward so the address of the first insn will be returned. In the examples I've given below, 
 * gadgets are entered as they are. In our compiler, it is almost like we will be storing the gadgets in
 * "little endian" form!
*/

// To run this program, execute it with the following command: 
// g++ -std=c++11  trying.cpp

// TODO: (bonus) *Add a count if the same gadget is found multiple times and store address of each instance 
// move this code to trie.cpp
// make a trie.h with all the necessary include statments
// modify the GetAllGadgets() function such that it returns the gadgets that it harvests instead of 
// simply printing them. Possibly use a global variable for this 
// Generate separate gadgets[] and addresses[] arrays 
// Once all this is done, build_tree() can be simply called.



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


int main(int argc, char** argv)
{
  /*
    Two gadgets are inserted into the trie - 
    00 5d c3
    and
    c3
  */
  std::vector<std::vector<uint8_t>> gadgets[] = {{{0,0}, {93}, {195}}, {{195}}};
  Trie trie;


  trie.build_trie(gadgets, {{123,234,345}, {567,678,789}}, 2);
  cout << "All nodes..." << endl;
  trie.print();

  cout << "Searching..." << endl;
  bool in_trie = false;


  /*
    Search for 00 5d c3 in tree.
    It will find it, and print in this format:
    address machinecode presntornot
    if not found address and presentornot will be 0.

    There's no gadget such as 5d c3 so that won't be found.
  */
  cout << trie.search({{0,0}, {93}, {195}}, in_trie) << " ";
  cout << "00 5d c3 "  <<  " " << in_trie << endl;;
  cout << trie.search({{93},{195}}, in_trie) << " ";
  cout << "5d c3 " << " " << in_trie << endl;;

  return 0;
}