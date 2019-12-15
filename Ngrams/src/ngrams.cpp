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
void generateWordMap(int N, Vector<string> inVec, Map<Queue<string>, Vector<string>>& nGramMap);
void generateNGram(int numWords, Map<Queue<string>, Vector<string>>& inMap);

int main() {
    printIntro();
    bool notDone = true, badName = true, badNum = true;
    int N = 0, numGen = 0;
    string word = "";
    ifstream input; //fstream used to read in from txt file
    /*NOTE: assume once we do obtain a valid file name that it does contain the
      right info for our purposes*/
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

    //TODO: scan fileName one word at a time and parse appropriately into a map object
    //store the contents of our file into a vector (as a temp container):
    Vector<string> fileContents;
    while (input >> word)
    {
        fileContents.push_back(word);
    }
    //input.clear() //NOTE: should probably do this...for safety
    input.close(); //For best practice, but probably not necc

    //Conduct user input validation on N...
    while (badNum)
    {
        N = getInteger("Value of N? ");
        if (N < 2)
        {
            cerr << "Warning: N must be greater than or equal to 2. "
                 << "Try again." << endl;
        }
        else badNum = false;
    }
    badNum = true;

    try //Purely for testing purposes...
    {
        /*Loop until user enters 0*/
        while (notDone)
        {
            //Conduct user input validation on numGen as well...
            while (badNum)
            {
                numGen = getInteger("# of random words to generate (0 to quit)? ");
                if (numGen != 0 && numGen < N) //have a problem if our N-Gram must be larger than
                {
                    cerr << "Warning: number of words to generate must exceed N. "
                         << "Try again." << endl;
                }
                else badNum = false;
            }
            badNum = true;
            if (numGen == 0) notDone = false; //End program requirements met
            else
            {
                //initialize a map object of vectors of strings s.t. we may pass
                //by reference to the generateNGram algorithm
                Map<Queue<string>, Vector<string>> wordMap;
                generateWordMap(N, fileContents, wordMap);
                generateNGram(numGen, wordMap);
                //CONSIDERATIONS: Create separate printNGram function and call
                //in main, or call in createNGram, or do not create at all and
                //simply print within createNGram?
            }
        }
        cout << "Exiting." << endl;
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

//NOTE: Consider simply passing fstream by reference containing the input file
//in its buffer, rather than creating a temporary vector container...
//ALSO NOTE: reading conents from fstream into a vector is O(N); however, would
//have to alter the loop significantly to accomdate reading straight from the stream...
//The trade offs: a question for a professor maybe?


/*
 * function: generateWordMap
 * parameters: int N - the "N" in "N-Gram," or the number of words contained in each key container
 *             Vector<string> inVec - The vector which stores the contents of the file in the order
 *                                    in which they appear in the file
 * uses: Map<Vector<string>, Vector<string>> wordMap = generateWordMap(someInt, someVecOfStrings)
 * ------------------------------------------------------------------------------------------------
 * Implementation notes (As of 11/25/18) : The algorithm can be broken down into 3 defining steps: 1) Initialize the
 * "window" (which is implemented as a queue, which is really the natural choice for the behaviour we
 * are aiming for) with the first N values from our temporary vector. 2.) Now loop over the entire
 * length of the vector, starting with an offset of N (N-1 is used due to indexing starting at 0) and
 * a.) add the current values in the window into our keys object; however, we wish to merely copy over
 * the values, keeping the queue unmodified. One method (and the method currently being used) is to
 * re-enqueue the object just previously added to the keys object. By the time we are finished adding
 * all values to the current key, we will "loop" back over and end up with the queue which we started
 * with. Now b.) dequeue the first object in our window, effectively discarding it, add the current value
 * in our file-containg vector to our value object and to the end of the queue, and finally clear the keys
 * and values objects for the next key. One thing to note: the if (nGramMap.containsKey(keys))... statement
 * is necessary so that we associate multiple values with a key if new values show up after the same key.
 * This part of the algorithm effectively rewrites the value for that given key with its previous contents
 * so that we are always adding the new value if it appears with a previously used key.
 */
void generateWordMap(int N, Vector<string> inVec, Map<Queue<string>, Vector<string>>& nGramMap)
{
   //NOTE: will want a test case for when numWords == N
    Vector<string> values;
    Queue<string> window; //Creating a queue which serves as our active window
    //Initialize the queue with the first N - 1 words from file
    for (int i = 0; i < N-1; i++)
    {
        window.enqueue(inVec[i]);
    }
    for(int i = N-1; i < (inVec.size()+N-1); i++)
    {
        //cout << "Printing contents in window" << endl;
        //cout << window.toString() << endl;
        //cout << "Printing keys..." << endl;
        //cout << keys.toString() << endl;
        if (nGramMap.containsKey(window))
        {
            values = nGramMap.get(window);
        }
        values.push_back(inVec[i % inVec.size()]);
        nGramMap.put(window, values);
        window.dequeue();
        window.enqueue(inVec[i % inVec.size()]);
        values.clear();
        //cout << "printing contents of map so far..." << endl;
        //cout << nGramMap.toString() << endl;
    }
}

void generateNGram(int numWords, Map<Queue<string>, Vector<string>>& inMap)
{
    string printWord = " ", nextWord = " ";
    Vector<string> activeValue;
    Vector<Queue<string>> keys;
    Queue<string> window;
    keys = inMap.keys();
    //initializing the active window...
    window = keys[randomInteger(0, keys.size()-1)];
    //initialize the active value associated with initial window (key)
    activeValue = inMap.get(window);
    for (int i = 0; i < numWords; i++)
    {
        printWord = window.dequeue();
        if (i == 0) cout << "... ";
        cout << printWord << " ";
        if (i == numWords-1) cout << "..." << endl;
        nextWord = activeValue[randomInteger(0, activeValue.size()-1)];
        window.enqueue(nextWord);
        //NOTE: for iteration k choosing activeValue for iteration k+1
        //(after printing everything we need to for iteration k and sliding
        //window over, in preparation for iteration k+1
        activeValue = inMap.get(window);
        //Probably want to loop over contents of activeValue to accumulate
        //statistics on elems...
    }
}
