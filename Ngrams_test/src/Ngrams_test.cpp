#include <cctype>
#include <cmath>
#include <fstream>
#include <iostream>
#include <string>
#include "random.h"
#include "simpio.h"
#include "console.h"
#include "queue.h"
#include "map.h"
#include "vector.h"

using namespace std;

const string filePrompt = "Input file name? ";
const string reprompt = "File not recognized. Try again.";

void printIntro();
//Map<Vector<string>, Vector<string>> generateWordMap(int N, Vector<string> inVec);

int main(/*int argc, char *argv[]*/)
{
//    printIntro();
//    bool notDone = true, badName = true, badNum = true;
//    int N = 0, numGen = 0;
//    string word = "";
//    ifstream input; //fstream used to read in from txt file
//    /*NOTE: assume once we do obtain a valid file name that it does contain the
//      right info for our purposes*/
//    while (badName)
//    {
//        string fileName = getLine(filePrompt);
//        input.open(fileName.c_str());
//        if(!input.is_open())
//        {
//            cerr << reprompt << endl;
//            //input.clear(); //NOTE: uncomment to stay SAFE
//        }
//        else badName = false;
//    }

//    //TODO: scan fileName one word at a time and parse appropriately into a map object
//    //store the contents of our file into a vector:
//    Vector<string> fileContents;
//    while (input >> word)
//    {
//        fileContents.push_back(word);
//    }
//    //input.clear() //NOTE: should probably do this...for safety
//    input.close(); //For best practice, but probably not necc

//    //Conduct user input validation on N...
//    while (badNum)
//    {
//        N = getInteger("Value of N? ");
//        if (N > fileContents.size())
//        {
//            cerr << "Warning: N cannot be greater than number of words in file. "
//                 << "Try again." << endl;
//        }
//        else badNum = false;
//    }
//    badNum = true;

//    /*Loop until user enters 0*/
//    while (notDone)
//    {
//        while (badNum)
//        {
//            numGen = getInteger("# of random words to generate (0 to quit)? ");
//            if (numGen != 0 && numGen < N) //have a problem if our N-Gram must be larger than
//            {
//                cerr << "Warning: number of words to generate cannot exceed N. "
//                     << "Try again." << endl;
//            }
//            else badNum = false;
//        }
//        badNum = true;
//        if (numGen == 0) notDone = false;
//        else
//        {
//            //createNGram(N, fileContents)
              //createNGram(N, numGen, generatedMap)
//            //CONSIDERATIONS: Create separate printNGram function and call
//            //in main, or call in createNGram, or do not create at all and
//            //simply print within createNGram?
//
//        }
//    }

    bool badName = true;
    int N = 0;
    int numGen = 0;
    string word = " ";
    ifstream input;
    Vector<string> fileContents, values;
    //NOTE: Change_log 11/26/18: removed key "middleman:" now made map of
    //Queue<string> to Vector<string> rather than Vector<string> to Vector<string>

    while (badName)
    {
        string fileName = getLine(filePrompt);
        input.open(fileName.c_str());
        if(!input.is_open())
        {
            cerr << reprompt << endl;
            //input.clear(); //NOTE: uncomment to stay SAFE
        }
        else badName = false;
    }

    while (input >> word)
    {
        fileContents.push_back(word);
    }
    input.clear();
    input.close();

    N = getInteger("Value of N? ");
    numGen = getInteger("# of random words to generate (0 to quit)? ");

    try
    {
       /*implementation of generateMap algorithm*/
        //UPDATED 11/26/18 with simplified functionality using map of
        //Queue<string> to vector<string> befitting of necessary behaviour
        Queue<string> window;
        Map<Queue<string>, Vector<string>> nGramMap;
        //Initialize window
        for (int i = 0; i < N-1; i++)
        {
            window.enqueue(fileContents[i]);
        }
        for(int i = N-1; i < (fileContents.size()+N-1); i++)
        {
//            cout << "Printing contents in window" << endl;
//            cout << window.toString() << endl;
            if (nGramMap.containsKey(window))
            {
                values = nGramMap.get(window);
            }
            values.push_back(fileContents[i % fileContents.size()]);
            nGramMap.put(window, values);
            window.dequeue();
            window.enqueue(fileContents[i % fileContents.size()]);
            values.clear();
//            cout << "printing contents of map so far..." << endl;
//            cout << nGramMap.toString() << endl; 12/1/2018: dont need toString: << is overloaded to accept object itself (or at least the variable identifier) as an argument!
        }
        window.clear();
        /*Implementation of generateNGram algorithm*/
        //NOTE: Could cut out the middle man by making keys a Vector<Queue<string>> --> Done (11/26/18)
        //s.t. we read directly from keys the random initial active key, effectively
        //initializing our "window" as a queue
        //Vector<string> activeKey;
        string printWord = " ", nextWord = " ";
        Vector<string> activeValue;
        Vector<Queue<string>> keys; //Only need this container for initializing the first window ig
        keys = nGramMap.keys();
        //initialize our window...
        window = keys[randomInteger(0, keys.size()-1)];
        //initialize the active value associated with initial window (key)
        activeValue = nGramMap.get(window);
        //Begin looping
        //TODO: Need means of tallying up statistics of words in values in order
        //to choose next word with a weighted probabilty. --> NOPE, intrinsic to how we
        //set up map!
        for (int i = 0; i < numGen; i++)
        {
            //string printWord = " ";
            printWord = window.dequeue();
            if (i == 0) cout << "... ";
            cout << printWord << " ";
            if (i == numGen-1) cout << "..." << endl;
            nextWord = activeValue[randomInteger(0, activeValue.size()-1)];
            window.enqueue(nextWord);
            //NOTE: for iteration k choosing activeValue for iteration k+1
            //(after printing everything we need to for iteration k and sliding
            //window over, in preparation for iteration k+1
            activeValue = nGramMap.get(window);
            //Probably want to loop over contents of activeValue to accumulate
            //statistics on elems...
        }



        cout << "Exiting normally." << endl;
        getLine("Press Enter to quit...");
        return 0;
    }
    catch (runtime_error& e)
    {
        cout << e.what() << endl;
        cout << "Exiting with error code 1." << endl;
        getLine("Press Enter to quit...");
        return 1;
    }
}

void printIntro()
{
    cout << "Welcome to My Random Writer ('N-Grams').\nThis program makes "
         << "random text based on a document.\nGive me an input file and an 'N' "
         << "value for groups\nof words, and I'll create random text for you.\n"
         << endl;
}
