#ifndef PQUEUE_H
#define PQUEUE_H

#include <iostream>
#include <cstdlib>
#include <string>
#include <iomanip>
#include <fstream>
#include "pQueueNode.h"

using namespace std;

enum PQUEUE_ERRORS {PQUEUE_BAD_SIZE, PQUEUE_FULL, PQUEUE_EMPTY};
enum PRIORITY_TYPE {MAXPRIORITY,MINPRIORITY};

template<typename T, typename U>
class pQueue
{
    public:
        pQueue(int s = 1024);
        ~pQueue();
        pQueue(const pQueue<T,U> &other);
        pQueue<T,U>& operator=(const pQueue<T,U> &other);

        bool empty();
        bool full();
        void resize(int s = 1024);
        int size() const;
        int capacity() const;
        void clear();
        void makeQueue(int s);

        void enqueue(const T &d, const U &p);
        T dequeue();
        pQueue<T,U>& operator>>(T &d);
        T& front();
        U& frontRanking();

        template<typename A, typename B>
        friend
        ostream& operator<<(ostream &out, const pQueue<A,B> &pQ);

        template<typename A, typename B>
        friend
        istream& operator>>(istream &in,  pQueue<A,B> &pQ);




    private:
        int boq, eoq;
        int mySize, myCapacity;

        pQueueNode<T,U> *pQ;

        void nukem();
        void copy(const pQueue<T,U>&other);
};

template<typename T, typename U>
pQueue<T,U>::pQueue(int s)
{
   makeQueue(s);
}

template<typename T, typename U>
pQueue<T,U>::~pQueue()
{
    nukem();
}


template<typename T, typename U>
void pQueue<T,U>::makeQueue(int s)
{
    boq = eoq = 0;
    mySize = 0;
    myCapacity = s;
    pQ = new pQueueNode<T,U>[s]; //plus one because eoq needs an empty spot to point at
}

template<typename T, typename U>
pQueue<T,U>::pQueue(const pQueue<T,U> &other)
{
    copy(other);
}

template<typename T, typename U>
pQueue<T,U>& pQueue<T,U>::operator=(const pQueue<T,U> &other)
{
    if(this != &other)
    {
        nukem();
        copy(other);
    }
    return *this;
}

template<typename T, typename U>
bool pQueue<T,U>::empty()
{
    return !mySize;
}

template<typename T, typename U>
bool pQueue<T,U>::full()
{
    return mySize == myCapacity;
}

template<typename T, typename U>
void pQueue<T,U>::resize(int s)
{
    if(s <= 0)
        throw PQUEUE_BAD_SIZE;

    nukem();
    makeQueue(s);
}

template<typename T, typename U>
int pQueue<T,U>::size() const
{
    return mySize;
}

template<typename T, typename U>
int pQueue<T,U>::capacity() const
{
    return myCapacity;
}

template<typename T, typename U>
void pQueue<T,U>::clear()
{
    nukem();
}


template<typename T, typename U>
void pQueue<T,U>::enqueue(const T &d, const U &p)
{

    pQueueNode<T,U> tempnode(d,p);

    if(full())
        throw PQUEUE_FULL;

    if(empty())
    {
        pQ[eoq%myCapacity].theData()= tempnode.theData();
        pQ[eoq%myCapacity].thePriority()= tempnode.thePriority();
        ++eoq;
        ++mySize;
        return;
    }

      //start of sorted insert (Greater number = greater priority)
      for (int i = eoq; i>=boq;--i)
      {
          if(i != boq) //if i is not at boq
          {
              if(higherPriority(pQ[(i-1)%myCapacity],tempnode)
                      || pQ[(i-1)%myCapacity].thePriority() == tempnode.thePriority()) //if a is greater than or equal to b.
              {
                  pQ[i%myCapacity].theData()= tempnode.theData();
                  pQ[i%myCapacity].thePriority()= tempnode.thePriority();
                  ++eoq;
                  ++mySize;
                  return;
              }
              else
              {
                  pQ[i%myCapacity].theData()= pQ[(i-1)%myCapacity].theData();
                  pQ[i%myCapacity].thePriority()= pQ[(i-1)%myCapacity].thePriority();
              }
          }

          else if(i == boq)//when i == boq
          {
              pQ[i%myCapacity].theData()= tempnode.theData();
              pQ[i%myCapacity].thePriority()= tempnode.thePriority();
              ++eoq;
              ++mySize;
              return;
          }
      }

}

template<typename T, typename U>
T pQueue<T,U>::dequeue()
{
    if(empty())
        throw PQUEUE_EMPTY;

    T temp;
    temp = pQ[boq%capacity()].theData(); //POSTFIX!!

    pQ[boq%capacity()].theData()=T();
    pQ[boq%capacity()].thePriority()=U();

    ++boq;
    --mySize;

    return temp;
}


template<typename T, typename U>
T& pQueue<T,U>::front()
{
    if(empty())
        throw PQUEUE_EMPTY;
    return pQ[boq%myCapacity].theData();//POSTFIX
}

template<typename T,typename U>
U& pQueue<T,U>::frontRanking()
{
    if(empty())
        throw PQUEUE_EMPTY;
     return pQ[boq%myCapacity].thePriority();
}

template<typename T, typename U>
void pQueue<T,U>::nukem()
{
    delete [] pQ;
    boq = eoq = 0;
    mySize = 0;
    myCapacity = 0;

}

template<typename T, typename U>
void pQueue<T,U>::copy(const pQueue<T,U>&other)
{
    nukem(); //POSTFIX
    mySize = other.mySize;
    myCapacity = other.myCapacity;
    boq = other.boq;
    eoq = other.eoq;
    makeQueue(other.myCapacity); //POSTFIX

    for(int i = 0; i < myCapacity; ++i)
        pQ[i] = other.pQ[i];
}


template<typename A, typename B>
ostream& operator<<(ostream &out, const pQueue<A,B> &pQ)
{
    for(int i =pQ.boq; i< pQ.eoq;++i)
    {
        out<<pQ.pQ[i%pQ.capacity()];
    }
    return out;
}

template<typename A, typename B>
istream& operator>>(istream &in,  pQueue<A,B> &pQ)
{
    pQueueNode<A,B> newNode;
    while(in>>newNode)
        pQ.enqueue(newNode.theData(),newNode.thePriority());
    return in;
}


#endif // PQUEUE_H
