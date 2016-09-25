#ifndef NODE_H
#define NODE_H
#include <iostream>
#include <cstdlib>
#include <iostream>
#include <iomanip>

enum nodeERRORS{NO_DATA};

using namespace std;

template<typename T>
class node
{
    public:
        node(T d = T(), int line = int(),int para = int());
        ~node();
        node(const node<T> &other);
        node<T>& operator=(const node<T>& other);

        T& theData();
        int& theline();
        int& thepara();

        template<typename U>
        friend
        bool operator<(const node<U>& x,const node<U>& y);

        template<typename U>
        friend
        bool operator>(const node<U>& x, const node<U>& y);

        template<typename U>
        friend
        ostream& operator<<(ostream &out, const node<U> &n);

        template<typename U>
        friend
        istream& operator>>(istream& in, node<U>& n);


    private:
        T data;
        int line_num;
        int para_num;

        void copy(const node<T> &other);

};

template<typename T>
node<T>::node(T d, int line, int para)
{
    data = d;
    line_num = line;
    para_num = para;
}

template<typename T>
node<T>::~node()
{
    data = T();
    line_num = 0;
    para_num = 0;
}

template<typename T>
node<T>::node(const node<T> &other)
{
    copy(other);
}

template<typename T>
node<T>& node<T>::operator=(const node<T>& other)
{
    if(this != &other)
        copy(other);
    return *this;
}

template<typename T>
T& node<T>::theData()
{
    return data;
}
template<typename T>
int& node<T>::theline()
{
    return line_num;
}
template<typename T>
int& node<T>::thepara()
{
    return para_num;
}

template<typename T>
void node<T>::copy(const node<T> &other)
{
    data = other.data;
    para_num = other.para_num;
    line_num = other.line_num;
}

template<typename U>
bool operator<(const node<U>& x,const node<U>& y)
{
    return  x.data < y.data;
}

template<typename U>
bool operator>(const node<U>& x, const node<U>& y)
{
    return x.data > y.data;
}

template<typename U>
ostream& operator<<(ostream &out, const node<U> &n)
{
    out<<n.data<<setw(5)<<" Paragraph: "<<n.para_num<<setw(5)<<" Line: "<<n.line_num<<" ;";

    return out;
}

template<typename U>
istream& operator>>(istream& in, node<U>& n)
{
    /*
    U data;
    if(in>>data)
        n.data = data;
    else
        throw NO_DATA;
        */
    in>>n.data;
    return in;
}


#endif // NODE_H
