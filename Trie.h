#ifndef TRIE_INCLUDED
#define TRIE_INCLUDED

#include <string>
#include <vector>
using namespace std;

template<typename ValueType>
class Trie
{
public:
    Trie();
    ~Trie();
    void reset();
    void insert(const std::string& key, const ValueType& value);
    std::vector<ValueType> find(const std::string& key, bool exactMatchOnly) const;
    
    // C++11 syntax for preventing copying and assignment
    Trie(const Trie&) = delete;
    Trie& operator=(const Trie&) = delete;
private:
    //Node Structure
    
    class Node{
    public:
        Node(){
            label = 'r'; //r for root, but it doesn't matter
        }
        Node(char a){
            label = a;
        }
        
        char label; //each node has a label
        vector<ValueType> values; //Each node has a list of values
        vector<Node*> children; //Each nodes potential pointers to children
    };
    
    //Ptr to root
    Node* root;
    
    //HELPER FUNCTIONS
    
    //Free the tree
    void freeTree(Node* a);
    
    //Insert
    void insert(Node* a, string key, const ValueType value);
    
    //Find
    vector<ValueType> find(Node* a, string key, bool exactMatchOnly, bool errorUsed) const;
};

//CONSTRUCTOR IMPLEMENTATION
template<typename ValueType>
Trie<ValueType>::Trie(){
    root = new Node(); //point root to first Node
}

//DESTRUCTOR IMPLEMENTATION
template<typename ValueType>
Trie<ValueType>::~Trie(){
   freeTree(root); //See helper methods
}

//RESET IMPLEMENTATION
template<typename ValueType>
void Trie<ValueType>::reset(){
    freeTree(root);
    root = new Node();
}

//INSERT IMPLEMENTATION
template<typename ValueType>
void Trie<ValueType>::insert(const std::string& key, const ValueType& value){
    insert(root, key, value); //Call helper function
}

//FIND IMPLEMENTATION
template<typename ValueType>
vector<ValueType> Trie<ValueType>::find(const std::string& key, bool exactMatchOnly) const{
    return find(root, key, exactMatchOnly, false);
}

//***HELPER METHODS***//

//Inserts
template<typename ValueType>
void Trie<ValueType>::insert(Node* a, string key, const ValueType value){
    if(a == root && key.size() == 0){ //if we are at root and no chars are left
        return;
    }
    if(key.size() == 0){ //if there are no chars left, place value in node
        a->values.push_back(value);
        return;
    }

    for(int i = 0; i < a->children.size(); i++){ //Check if the Letter Child Already exists,
        if(a->children[i]->label == key[0]){
            insert(a->children[i], key.substr(1), value); //Move to that child!
            return;
        }
    }
    //Otherwise, make one!
    
    a->children.push_back(new Node(key[0])); //Otherwise, create a node with that letter
    insert(a->children.back(), key.substr(1), value); //Move to that node
}

//Completely clears the tree
template<typename ValueType>
void Trie<ValueType>::freeTree(Node* a){
    if(a == nullptr) //Return once its reached an end
        return;
    //Call for each child
    for(int i = 0; i < a->children.size(); i++){
        freeTree(a->children[i]);
    }
    delete a;
}

//Merge Into
template<typename ValueType>
void merge(vector<ValueType> &mergeInto, const vector<ValueType> &giveUp){
    for(int i = 0; i < giveUp.size(); i++){
        mergeInto.push_back(giveUp[i]);
    }
}

//Find vectors
template<typename ValueType>
vector<ValueType> Trie<ValueType>::find(Node* a, string key, bool exactMatchOnly, bool errorUsed) const{
    //Reached the end, return values up
    if(key.size() == 0)
        return a->values; //Only actually return values if we've run out of letters
    
    vector<ValueType> solution;
    vector<ValueType> lowerSolution;
    
    for(int i = 0; i < a->children.size(); i++){ //If its a perfect match
        if(a->children[i]->label == key[0]){
            lowerSolution = find(a->children[i], key.substr(1), exactMatchOnly, errorUsed);
            merge(solution, lowerSolution);
        }
        else if(a != root && !exactMatchOnly && !errorUsed){ //If its off by one
            lowerSolution = find(a->children[i], key.substr(1), exactMatchOnly, true);
            merge(solution, lowerSolution);
        }
    }
    return solution;
}

#endif // TRIE_INCLUDED
