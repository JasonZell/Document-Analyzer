#ifndef TEXTPARSER_H
#define TEXTPARSER_H
#include "heap.h"
#include <iostream>
#include <string>
#include <fstream>
#include <iomanip>
#include <cstdlib>
#include <map>
#include <sstream>
#include "node.h"
#include <queue>
#include "pQueue.h"
#include "pQueueNode.h"

using namespace std;

enum ALPHABET {A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P,Q,R,S,T,U,V,W,X,Y,Z};

class TextParser
{
    public:
        TextParser();
        ~TextParser();
        TextParser(const TextParser&);
        TextParser& operator=(const TextParser&);

        int& theSentenceCount();
        int& theParagraphCount();
        int& theWordCount();
        double& theSyllableCount();
        int theHeapSize(ALPHABET n);
        int& alphabet_count(ALPHABET n);
        int& data_frequency(string s);
        heap<string>& theHeap(ALPHABET n); //return heap in the wordstore


        void DataProcessing();
        deque<string>& Word_List();
        pQueue<string,int>& word_rank();
        map<int,vector<int> >& WordDataMap(string& s);
        vector<pair<string,int> >& TopTen();
        vector<int>& ParaList(string& s);

        friend
        void tolowercase(string& s);

        friend
        void operator<<(map<string,int>& m, TextParser& parser);

        friend
        istream& operator>>(istream& in, TextParser& parser);

        friend
        istream& readIntoParser(istream& in, TextParser* parser);


    private:

        map<string,int> word_frequency;
        map<string,map<int,vector<int> > > word_data;
        heap<string> WordStore[26]; //26 heaps for 26 alphabets
        deque<string> WordList;
        pQueue<string, int> Word_Ranking;
        vector<pair<string,int> > TopTenWord;
        map<string,vector<int> > para_list;

        int paragraph_count; //Total paragraph count.
        int sentence_count; // total sentence count.
        int word_count; //total word count.
        int line_count;
        double syllable_count;

        void copy(const TextParser&);
        void nukem();

        template <typename string>
        void parse(string&, node<string>&);

};

#endif // TEXTPARSER_H
