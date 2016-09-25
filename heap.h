/* NOTES
 * 1.Node class are not necessary in vector based heap(having duplicate data is ok)
 * 2.Search "Ryan" for notes.
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 */

#ifndef HEAP_H
#define HEAP_H
#include <iostream>
#include <cstdlib>
#include <iostream>
#include <iomanip>
#include <cmath>
#include <map>
#include <vector>
#include "textparser.h"
#include "node.h"

using namespace std;

enum COMP_TYPE {LESS, GREATER}; //Comparison type
enum ERRORS {EMPTY, FULL};

template<typename T>
class heap
{
    public:
        heap( COMP_TYPE m = LESS,unsigned int s = 0); //minimum heap by default (the parent is less than child)
        heap(node<T> d, unsigned int s = 0, COMP_TYPE m = LESS); //minimum heap by default (the parent is less than child)
        ~heap();
        heap(const heap<T> &other);
        heap<T>& operator=(const heap<T> &other);


        heap<T>& operator<<(T &d);
        heap<T>& operator>>(T &d);

        void insert(T &d);
        void insertnode(node<T>& n); //this might be better;

        int& alphacount();
        node<T>* pop();
        node<T>* &peek();
        void clear();
        bool full();
        bool empty();
        unsigned int &size();


        vector<node<T> >& theVector();//test

        template<typename U>
        friend
        ostream& operator<<(ostream& out, heap<U> &h);

        template<typename U>
        friend
        istream& operator>>(istream& in, heap<U> &h);

        template<typename U>
        friend
        void operator>>(string& in, heap<U> &h); //TEST

        template<typename U>
        friend
        void operator>>(node<U>& n,heap<U> &h);

      /*  template<typename U>
        friend
        void HeapToMap(map<string,vector<pair<int,int> > >& );*/

    private:

        typedef bool(heap<T>::*fptr)(node<T>* &x,node<T>* &y);

        vector<node<T>* > heapVector;
        int alpha_count;
        unsigned int lastNode, capacity;
        COMP_TYPE Compare_Type;
        fptr compare[2];

        void copy(const heap<T> &other);
        void nukem();
        void reheapifyUp();
        void reheapifyDown();
        bool greater(node<T>* &x, node<T>* &y);
        bool less(node<T>* &x, node<T>* & y);

};



template<typename T>
heap<T>::heap(COMP_TYPE m,unsigned int s )
{

    alpha_count = 0;
    lastNode = 0;
    capacity = s;
    Compare_Type = m;
    compare[LESS] = &heap<T>::less;
    compare[GREATER] =&heap<T>::greater;
}

template<typename T>
heap<T>::heap(node<T> d, unsigned int s, COMP_TYPE m )
{
    heapVector.push_back(new node<T>(d));
    alpha_count = 0;
    lastNode = 1;
    capacity = s;
    Compare_Type = m;
    compare[LESS] = &heap<T>::less;
    compare[GREATER] = &heap<T>::greater;
}

template<typename T>
heap<T>::~heap()
{
    nukem();
}

template<typename T>
heap<T>::heap(const heap<T> &other)
{
    copy(other);
}

template<typename T>
heap<T>& heap<T>::operator=(const heap<T> &other)
{
    if(this != &other)
    {
        nukem();
        copy(other);
    }
    return *this;
}


template<typename T>
heap<T>& heap<T>::operator<<(T &d)
{

    insert(d);
    return *this;
}

template<typename T>
heap<T>& heap<T>::operator>>(T &d)
{
    d = pop();
    return *this;
}


template<typename T>
void heap<T>::insert(T& d)
{
    if(full())
        throw FULL;
    heapVector.push_back(new node<T>(d));
    ++lastNode;//crucial here.
    reheapifyUp();
}

template<typename T>
void heap<T>::insertnode(node<T>& n )
{
    if(full())
        throw FULL;
    heapVector.push_back(new node<T>(n));
    ++lastNode;//crucial here.
    reheapifyUp();
}

template<typename T>
node<T>* heap<T>::pop()
{
    if(empty())
        throw EMPTY;
    typename vector<node<T>* >::iterator iterRoot = heapVector.begin(),iterEnd = heapVector.end()-1;
    node<T> *rootData = new node<T>(**iterRoot);
    delete *iterRoot; //MEMORY MANAGEMENT.
    *iterRoot = *iterEnd;
    heapVector.erase(iterEnd);
    --lastNode; //size of vector should match lastNode.
    if(lastNode>1)//EDITED
        reheapifyDown();
    return rootData; //deference it
}

template<typename T>
node<T>* & heap<T>::peek()
{
    typename vector<node<T>* >::iterator root = heapVector.begin();
    return *root;
}

template<typename T>
void heap<T>::clear()
{
    nukem();
}

template<typename T>
bool heap<T>::full()
{
     return capacity && lastNode == capacity - 1;
}

template<typename T>
bool heap<T>::empty()
{
    return !lastNode;
}

template<typename T>
unsigned int& heap<T>::size()
{
    return lastNode;
}


template<typename T>
void heap<T>::copy(const heap<T> &other)
{

    heapVector.resize(other.heapVector.size());
    for(size_t i = 0; i < other.heapVector.size();++i)
        heapVector[i] = new node<T> (*(other.heapVector[i]));

    alpha_count = other.alpha_count;
    lastNode = other.lastNode;
    capacity = other.capacity;
    Compare_Type = other.Compare_Type;
    compare[LESS] = &heap<T>::less;
    compare[GREATER] =&heap<T>::greater;
}

template<typename T>
void heap<T>::nukem()
{
    heapVector.clear();
    lastNode = 0;
    capacity = 0;
    alpha_count= 0;
    compare[LESS] = &heap<T>::less;
    compare[GREATER] =&heap<T>::greater;
}

template<typename T>
void heap<T>::reheapifyUp()
{
    bool notDone = true;
    unsigned int child = lastNode-1, parent = (child-1) >> 1;//parent = (child-1)/2
    while(notDone && child)  //THIS ONE I HAVE DOUBT,RYAN, the child condition. originally i used parent.
    {
        node<T>* temp;

        //cPtr = getNode(child);
        //pPtr = getNode(parent);
        if((this->*compare[Compare_Type])(heapVector[child],heapVector[parent]))
            // if child Ptr data less than parent Ptr data, SWAP their data;
        {

            temp = heapVector[parent];
            heapVector[parent] = heapVector[child];
            heapVector[child] = temp;
            child = parent;
            parent = (child-1) >> 1; // (child-1)/2
        }
        else
            notDone = false;
   }

}

template<typename T>
void heap<T>::reheapifyDown()
{
    //Use arry index math, assume root is at 0 location;
    unsigned int root = 0, toSwap,leftchild = (root<<1)+2, rightchild=(root<<1)+1;
    bool notDone = true;

    while(notDone && (rightchild <= lastNode-1)) //notDone && (2n) < lastnode)
    //while(notDone && rightchild != 2)
    // while(notDone && ptr->child(RIGHT))
    {
       // bool notDone = true;
       // if(!(ptr->child(LEFT)))
        if(leftchild > (lastNode-1))
            //if left child is empty, pick right child (this is when there is only two nodes)
            //toSwap = ptr->child(RIGHT);
              toSwap = rightchild;
        else
              if((this->*compare[Compare_Type])(heapVector[rightchild], heapVector[leftchild])) // 2n = right child,2n+1 left child
                //if right child less than left child, pick right child
                  toSwap = rightchild;
              else//if left child is less than right child, pick left child.
                    toSwap = leftchild;
        if((this->*compare[Compare_Type])(heapVector[toSwap], heapVector[root]))
        //if toswap, which is a child, less than ptr(parent)
        {
            node<T>*temp = heapVector[toSwap];
            heapVector[toSwap] = heapVector[root];
            heapVector[root] = temp;
            root = toSwap;
        }
        else
            notDone = false;

        leftchild = (root<<1)+2;
        rightchild = (root<<1)+1;
    }
}

template<typename T>
bool heap<T>::greater(node<T>* &x, node<T>* &y)
{
    return *x > *y;
}

template<typename T>
bool heap<T>::less(node<T>* &x, node<T>* &y)
{
    return *x < *y;
}

template<typename U>
ostream& operator<<(ostream& out, heap<U> &h)
{
    heap<U> temp(h); //Create a copy of heap
    //U data;
    while(!temp.empty())
    //while(!h.empty())
    {
        //data = temp.pop().theData();
       // out<<data<<" ";
        out<<temp.pop()<<" | ";
    }
    return out;
}

template<typename U>
istream& operator>>(istream& in, heap<U> &h)
{
    U data;
    while(in>>data)
        h<<data;
    return in;
}

template<typename U>
void operator>>(string& in, heap<U> &h)
{
    h.insert(in);
}

template<typename string>
void operator>>(node<string> &n, heap<string> &h)
{
    h.insertnode(n);
}

template<typename T>
vector<node<T> >& heap<T>::theVector()
{
    return heapVector;
}

template<typename T>
int& heap<T>::alphacount()
{
    return alpha_count;
}
/*
template <typename U>
void HeapToMap(map<string,vector<pair<int,int> > >& m, heap<U> &h)
{
    heap<U> temp(h); //Create a copy of heap

    while(!temp.empty())
    pair<int,int> temppair;
    for(int i = 0; i < 26; ++i)
    {
        while()
        temppair.first = parser.theHeap((ALPHABET(i)));
        m[parser.theHeap((ALPHABET)i)].push_back()
   // out<<parser.
    }
}
*/


#endif // HEAP_H
