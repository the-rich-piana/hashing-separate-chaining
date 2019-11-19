#include <iostream>
#include <chrono>
#include <string>
#include <vector>

using namespace std;

static int compareCount = 0;


//
// NODE CLASS  
//

class Node{
  public:
    string key;
    int val = 0;
    Node *next = nullptr;
    Node *prev = nullptr;

    Node(string _key, int _val)  { // constructor
      key = _key; // initialize the key in the node to our passed in _key argument
      val = _val; // initialize the val in the node to our passed in _val argument
    }
}; // end Node Class


//
// SYMBOL TABLE CLASS
//

class SymbolTable{

  private:
    int currentSize; // number of entries in the SymbolTable
    int maxBuckets; // max size of array (static)
    Node **hashTable; // array of pointers to Nodes
    int bucketsUsed = 0;
    int debug = false;

  public: 

    SymbolTable(int _maxBuckets = 64, bool _debug = false){ // constructor
      currentSize = 0;
      maxBuckets = _maxBuckets;
      debug = _debug;
      hashTable = new Node*[maxBuckets]; // dynamic allocation of our hashTable of size maxBuckets pointers to nodes
      for(int i=0; i<maxBuckets; i++){
        hashTable[i] = nullptr; // initialize all table pointers to null
      }
    } // end constructor

    ~SymbolTable(){ // our destructor
      for(int i=0; i<maxBuckets; i++){
        if (hashTable[i] != nullptr)
          for(Node *temp = hashTable[i]; temp!= nullptr; ){
            Node *deleteThis = temp;
            temp = temp->next; // delete the current node
          }
      }
      delete[] hashTable;   // free the hashTable when we leave
    } // end destructor

  //
  // FOLD   fold each 4 characters into an integer, return the sum of the integers
  //

  unsigned int fold(string s ){  
    unsigned int sum = 0;
    for(int i=0; i< s.size(); i++){
      unsigned int val = s[i];
      //cout << "  fold(): shift " << s[i] << " by " << ((i%4)*8) << " bits = ";
      val = val << ((i%4)*8); // shift left by 0, 8, 16, or 24 
      //cout << val << endl;
      sum += val; // add shifted value to total sum
    }
    return sum; // return sum
  }

  //
  // HASH FUNCTION
  //

  int hashFunction(string s, int m){  // simple summuation of ascii values mod m
    unsigned int sum = fold(s); // incorporate folding into  function
    //cout << "myHash() folded " << s << " into " << sum << endl;
    return(sum%m);
  }

  //
  //  FIND NODE


  Node *findNodeInLinkedList( Node *temp, string key){
    
    Node * search = temp;
    
    while(search != nullptr){ 
       // cout <<"SEARCHING" << endl;
      if(search->key == key){ //if same key
        
        return search; //return that node
      }
      else // otherwise keep going
        search = search->next;
    }
    
    return nullptr; //if not found
  }


  //
  // insertKeyVal - insert the given key, value pair into the ST; return true if a value was inserted

  

    bool insertKeyVal(string key, int val){
      int bucket_id = hashFunction(key, maxBuckets);
      
      
      if(hashTable[bucket_id] == nullptr){ //if bucket is empty
        Node *insert = new Node(key, val); //new Node 
        hashTable[bucket_id] = insert;
        bucketsUsed++;                    //fresh bucket
        currentSize++;
        return true;
      }
      
      if(hashTable[bucket_id] != nullptr){ //Somethings in there
        Node * temp = findNodeInLinkedList(hashTable[bucket_id], key); //search for node with same key
        
        //cout << "BUCKET ID: " << bucket_id << endl;

        if(temp == nullptr){                    //if not found insert a new node
          temp = hashTable[bucket_id]; //reset temp to head just to make sure
          //cout << "INSERT: " << key << endl;
          Node *insert = new Node(key, val);
          
          temp->prev = insert;
          insert->next = temp;
          
          hashTable[bucket_id] = insert;// change the bucket head pointer
          currentSize++;
          return true;
        }
        if(temp != nullptr){ //if found overwrite the value.
          //cout << "OVERWRITE: " << key << endl;
          temp->val = val;
          return false;
        }
        
      }
      return false; // don't forget to change this
    }


  //
  // deleteKey - delete the requested key if it exists, return true if found & deleted
  //

    bool deleteKey(string key){

      int bucket_id = hashFunction(key, maxBuckets);
      
      if(hashTable[bucket_id] == nullptr){ //already empty
        return false;
      }
      if(hashTable[bucket_id] != nullptr){ // not empty
        Node * del = findNodeInLinkedList(hashTable[bucket_id], key); //search for node with same key
        
          //cout << "BUCKET ID: " << bucket_id << endl;
        
          if (hashTable[bucket_id] == nullptr || del == nullptr)  
              return false;  
  
          if (hashTable[bucket_id] == del){  
              hashTable[bucket_id] = del->next;  
              bucketsUsed--;          //if it's the last node in the bucket
          }

          if (del->next != NULL){ 
              del->next->prev = del->prev;
          }

          if (del->prev != NULL){  
              del->prev->next = del->next;
          }
      
          
          delete del; //delete this junk
          currentSize--;
          return true;
      }
      
      
      
      return false;  // true: we did delete

    } // end deleteKey()

  //
  // getValue  - return the value associated with a specific key , 
  //
  //

    int getValue(string key){
      int bucket_id = hashFunction(key, maxBuckets);

      if(hashTable[bucket_id] == nullptr){ //empty
        return -1;
      }
      else{
        Node * temp = findNodeInLinkedList(hashTable[bucket_id], key);
        
//        cout << endl << "TEMP VAL: " << temp->val << endl;
        if(temp == nullptr){ //not found
          return -1;
        }
        else{
          return temp->val;
        }
      }
    }
        
  //
  // containsKey - Return True if the ST contains the requested key
  //

    bool containsKey(string key){
      int bucket_id = hashFunction(key, maxBuckets);
      
      if(hashTable[bucket_id] == nullptr){
        return false; //empty
      }
      else{
        Node * temp = findNodeInLinkedList(hashTable[bucket_id], key);
        if(temp == nullptr){
          return false;
        }
        else{
          return true;
        }
      }
    } // end constainsKey()

  //
  // isEmpty() return true if the symbol table is isEmpty
  //

    bool isEmpty(){

      if (bucketsUsed==0)
        return true;
      else
        return false;
    }

  //
  // getSize() - return the number of members in the Symbol Table
  //

    int getSize(){
      return currentSize;
    }

  //
  // printForward
  //

    void printMe() {
      for(int i=0; i<maxBuckets; i++){  // for each bucket
        cout << "Bucket #" << i << "| ";
        for(Node *temp = hashTable[i]; temp!=nullptr; temp=temp->next){  // for each node in this bucket
          if (temp->next != nullptr)
            cout << temp->key << ":" << temp->val << ", ";  // add the comma
          else
            cout << temp->key << ":" << temp->val << ".";   // add a period
        }
        cout << endl; 
      }
      cout << "Load Factor: " << loadFactor() << endl << endl;
    }

  //
  // LOAD FACTOR
  //
  
// return the floating point load factor for the hash table (ratio of buckets use to total number of buckets)
  float loadFactor(){

    double lf = (static_cast<float>(bucketsUsed) / static_cast<float>(maxBuckets)); // be sure to update this
    
    if(lf < 0){
      lf = 0.0;
    }
    if(lf > 1){
      lf = 1.0;
    }
    return( lf );
  }


};// end SymbolTable Class



int main() {
  std::cout << "Symbol Table and Hashing \n";

  //testMe();         // uncomment when you are ready to self-test

  cout << "\nTo insert: Enter a key string and then an integer value, separated by a space, then hit return.\nTo Delete: Enter a key and the letter x, separated by a space, then hit return.\nTo Get a value: Enter a key and the letter g, separated by a space, then hit return." << endl;

  string key; // key string
  string sval; // value string or x to delete the specified key

  int val; // integer equivalent of sval

  int M = 10;

  cout << "Symbol Table size M (ex: 10) ? ";
  cin >> M;


  SymbolTable st(M); // create the symbol table of size N

  while( true ){
    cout << "key? "; cin >> key;
    cout << "val (or x or g)? "; cin >> sval;

    if (sval == "x"){
      st.deleteKey(key); // delete the key
    }
    else if (sval == "g"){
      cout << "The value for key " << key << " = " << st.getValue(key) << endl;; // delete the key
    }
    else{
      val = atoi(sval.c_str()); // convert string sval to int val
      st.insertKeyVal( key, val ); // insert the key
    }

    st.printMe();
  }
}


