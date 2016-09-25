#ifndef PQUEUEpQueueNode_H
#define PQUEUEpQueueNode_H

#include <iostream>
#include <cstdlib>
#include <string>
#include <iomanip>
#include <fstream>


using namespace std;

template<typename T, typename U>
class pQueueNode
{
    public:
        pQueueNode();
        pQueueNode(const T &,const U&);
        ~pQueueNode();
        pQueueNode(const pQueueNode<T,U> &);
        pQueueNode<T,U> operator=(const pQueueNode<T,U> &);
        U& thePriority();
        T& theData();

        template<typename A,typename B>
        friend
        ostream& operator<<(ostream &, const pQueueNode<A,B> &);

        template<typename A,typename B>
        friend
        istream& operator>>(istream &, pQueueNode<A,B> &);

        template<typename A,typename B>
        friend
        bool higherPriority(const pQueueNode<A,B> &, const pQueueNode<A,B> &);

    private:
        T data;
        U priority;

        void copy(const pQueueNode<T,U> &);
        void nukem();
};

template<typename T, typename U>
pQueueNode<T,U>::pQueueNode()
{
    nukem();
}

template<typename T, typename U>
pQueueNode<T,U>::pQueueNode(const T &d,const U &p)
{
    data = d;
    priority = p;
}

template<typename T, typename U>
pQueueNode<T,U>::~pQueueNode()
{
    nukem();
}

template<typename T, typename U>
pQueueNode<T,U>::pQueueNode(const pQueueNode<T,U> &other)
{
    copy(other);
}

template<typename T, typename U>
pQueueNode<T,U> pQueueNode<T,U>::operator=(const pQueueNode<T,U> &other)
{
    if(this != &other)
    {
        nukem();
        copy(other);
    }
    return *this;
}


template<typename T, typename U>
U& pQueueNode<T,U>::thePriority()
{
    return priority;
}

template<typename T, typename U>
T& pQueueNode<T,U>::theData()
{
    return data;
}

template<typename T, typename U>
void pQueueNode<T,U>::copy(const pQueueNode<T,U> &other)
{
    priority = other.priority;
    data = other.data;
}

template<typename T, typename U>
void pQueueNode<T,U>::nukem()
{
    data = T();
    priority = U();
}

template<typename A,typename B>
ostream& operator<<(ostream &out, const pQueueNode<A,B> &item)
{
    out<<"("<<item.data<<", "<<item.priority<<") ";
    return out;
}

template<typename A,typename B>
istream& operator>>(istream &in, pQueueNode<A,B> &item)
{
    A data;
    B priority;
    char junk;
    if(&in == &cin)
    {
        cout<<"Data: ";
        in>>data;
        cout<<"Priority: ";
        in>>priority;
    }
    else
        in>>junk>>data>>junk>>priority>>junk;
    item.theData() = data;
    item.thePriority() = priority;
    return in;
}

template<typename A, typename B>
bool higherPriority(const pQueueNode<A,B> &a, const pQueueNode<A,B> &b)
{
    return a.priority > b.priority; //greater number is greater priority
}


#endif // PQUEUEpQueueNode_H
