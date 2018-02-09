/*
 * This is my attempt at Insight Coding Challenge for the 2018 year!
 * File:   main.cpp
 * Author: hassanshallal
 *
 * Created on February 6, 2018, 6:06 PM
 */

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <map>
#include <math.h>
#include <cstddef> 

using namespace std;


//Let's write some utilities functions that we always need.
template<class ItemType>
void displayVec(vector<ItemType> vec) {
    for (size_t i = 0; i < vec.size(); ++i)
        cout << vec[i] << " " << flush;
    cout << "\n" << endl;
}

template<class ItemType>
bool arrayContains(const ItemType arr[], int size, int target) {
    for (size_t i = 0; i < size; ++i) {
        if (arr[i] == target)
            return true;
        else
            return false;
    }
}

vector<int> findLocations(const string &sample, char target) {
    vector<int> positions;
    for (size_t i = 0; i < sample.size(); ++i) {
        if (sample[i] == target) {
            positions.push_back(i);
        };
    };
    return positions;
}

bool checkPriority(const vector<int> years, const int curRecordYear) {
    for (size_t i = 0; i < years.size(); ++i) {
        if (years[i] > curRecordYear) {
            return true;
        };
    };
    return false;
}


// We need to have some validation functions that validates the important entries in each line

bool checkRecipient(string recipient) {
    if (recipient.size() != 9) {
        return false;
    };

    char firstChar;
    firstChar = recipient[0];
    bool isC = (firstChar == 'C');
    string last8 = recipient.substr(1, 8);
    size_t found = last8.find_first_not_of("0123456789");
    bool last8Digits = true;
    if (found != string::npos) {
        last8Digits = false;
    };

    if (!isC || !last8Digits) {
        return false;
    } else {
        return true;
    };
}

bool checkZIP_CODE(const string ZIP_CODE) {
    //at least 5 digits
    if (ZIP_CODE.size() < 5) {
        return false;
    } else {
        return all_of(ZIP_CODE.begin(), ZIP_CODE.end(), ::isdigit);
    }
}

bool checkTRANSACTION_DT(const string TRANSACTION_DT) {
    //at least 5 digits
    if (TRANSACTION_DT.size() != 8) {
        return false;
    } else {
        return true;
    };
}

bool checkTRANSACTION_AMT(const string TRANSACTION_AMT) {
    return all_of(TRANSACTION_AMT.begin(), TRANSACTION_AMT.end(), ::isdigit);
}

class RecipientWithRepeatDonors {
private:
    vector <int> contributionsFromRepeatDonors;
    int sumTotalContributionsFromRepeatDonors = 0;

public:

    RecipientWithRepeatDonors(int curContribution) {
        contributionsFromRepeatDonors.push_back(curContribution);
        sumTotalContributionsFromRepeatDonors += curContribution;
    }

    void asscendingSortedVectorPushBack(int curContribution) {
        for (size_t i = 0; i < contributionsFromRepeatDonors.size(); i++) {
            if (curContribution <= contributionsFromRepeatDonors[i]) {
                contributionsFromRepeatDonors.insert(contributionsFromRepeatDonors.begin() + i, curContribution);
                break;
            } 
        };
        contributionsFromRepeatDonors.push_back(curContribution);
    }

    void updateRecipientWithRepeatDonors(int curContribution) {
        asscendingSortedVectorPushBack(curContribution);
        sumTotalContributionsFromRepeatDonors += curContribution;
    };

    string printRecipientWithRepeatDonors(int percentile){
        int percentileIndex = ceil((percentile / 100) * contributionsFromRepeatDonors.size());
        string result;
        result = "|" + to_string(contributionsFromRepeatDonors[percentileIndex]) + "|" + to_string(sumTotalContributionsFromRepeatDonors) + "|" + to_string(contributionsFromRepeatDonors.size());
        return result;
    }
};

int main(int argc, char** argv) {


    // Open the percentile file, get the percentile value
    ifstream percentileInput;
    percentileInput.open("percentile.txt"); //open a file
    // Read the first line to get the percentile value then close the file
    int percentile;
    percentileInput >> percentile;
    cout << "The percentile is: " << percentile << endl;
    percentileInput.close();
    cout << "----------" << endl;


    //We eill read the input line by line
    ifstream itcont;
    itcont.open("itcont.txt");

    //Initialize an empty map and a corresponding iterator for the previous donors. Also initialize a bool
    map<string, vector<int>> prevDonors;
    map<string, vector<int>>::iterator prevDonorsIt;
    bool isRepeatDonor = false;
    
    //Initialize an empty map and a corresponding iterator for the recipients with repeat donors
    map<string, RecipientWithRepeatDonors*> recipientsWithRepeatDonors;
    map<string, RecipientWithRepeatDonors*>::iterator recipientsWithRepeatDonorsIt;

    string a; //whole line
    
    //Keep record count, keep track of valid and invalid lines
    int i = 0; //record counter
    vector<int> validRecords;
    vector<int> inValidRecords;

    while (!itcont.eof()) {
        getline(itcont, a);
        if (!a.empty()) {//if a is empty, ignore it or otherwise you obtain segmentation fault when findLocations function act on an empty string, we haven;t included any exceptions for edge cases yet
            
            //Initialize some validation variables
            string CMTE_ID;
            bool validCMTE_ID = false;

            string NAME;

            string ZIP_CODE;
            bool validZIP_CODE = false;

            string TRANSACTION_DT;
            bool validTRANSACTION_DT = false;

            int year;

            string TRANSACTION_AMT;
            bool validTRANSACTION_AMT = false;

            string OTHER_ID;
            bool isIndividual = false;

            bool validRecord = false;

            ++i; //increment the line/record counter
            cout << a << endl;
            
            //find all the pipe locations
            vector<int> pipeLocVec = findLocations(a, '|');
            //displayVec(pipeLocVec); //in case you want to display the indices of pipes in a line

            //extract and validate CMTE_ID
            CMTE_ID = a.substr(0, pipeLocVec.at(0));
            cout << "CMTE_ID: " << CMTE_ID << endl;

            if (checkRecipient(CMTE_ID)) {
                validCMTE_ID = true;
            };

            //Extract name, any thing can be a name, no validation is required here
            NAME = a.substr(pipeLocVec[6] + 1, pipeLocVec[7] - pipeLocVec[6] - 1);
            cout << "NAME: " << NAME << endl;

            //Extract and validate ZIP_CODE
            ZIP_CODE = a.substr(pipeLocVec[9] + 1, (pipeLocVec[10] - pipeLocVec[9] - 1));
            if (checkZIP_CODE(ZIP_CODE)) {
                ZIP_CODE = ZIP_CODE.substr(0, 5);
                validZIP_CODE = true;
            };
            cout << "ZIP_CODE: " << ZIP_CODE << endl;

            //Extract and validate TRANSACTION_DT
            TRANSACTION_DT = a.substr(pipeLocVec[12] + 1, pipeLocVec[13] - pipeLocVec[12] - 1);
            if (checkTRANSACTION_DT(TRANSACTION_DT)) {
                validTRANSACTION_DT = true;
            };

            cout << "TRANSACTION_DT: " << TRANSACTION_DT << endl;
            year = stoi(TRANSACTION_DT.substr(4, 4));
            cout << "Year: " << year << endl;

            //Extract and validate TRANSACTION_AMT
            TRANSACTION_AMT = a.substr(pipeLocVec[13] + 1, pipeLocVec[14] - pipeLocVec[13] - 1);
            if (checkTRANSACTION_AMT(TRANSACTION_AMT)) {
                validTRANSACTION_AMT = true;
            };
            cout << "TRANSACTION_AMT: " << TRANSACTION_AMT << endl;

            //Extract and validate OTHER_ID
            cout << "OTHER_ID: " << a.substr(pipeLocVec[14] + 1, pipeLocVec[15] - pipeLocVec[14] - 1) << endl;
            if (a.substr(pipeLocVec[14] + 1, pipeLocVec[15] - pipeLocVec[14] - 1).empty()) {
                isIndividual = true;
            };

            cout << "validCMTE_ID is " << validCMTE_ID << endl;
            cout << "validZIP_CODE is " << validZIP_CODE << endl;
            cout << "validTRANSACTION_DT is " << validTRANSACTION_DT << endl;
            cout << "validTRANSACTION_AMT is " << validTRANSACTION_AMT << endl;
            cout << "isIndividual is " << isIndividual << endl;

            validRecord = (validCMTE_ID && validZIP_CODE && validTRANSACTION_DT && validTRANSACTION_AMT && isIndividual);
            cout << "This record validity is: " << validRecord << endl;
            
            //When we have a valid record, we want to check whether we have a repeat donor or not AND we want to pupulate/update the prevDonors map with the current record accordingly
            
            if (validRecord) {
                validRecords.push_back(i);
                
                string curKey = NAME + "_" + ZIP_CODE; //Based on the assumption that the a repeat donor muct have the same name and the same zip code
                //cout << curKey << endl;
                prevDonorsIt = prevDonors.find(curKey); //is the curKey already in the prevDonors map?
                
                if (prevDonorsIt == prevDonors.end()) {//curKey is not in the prevDonors map
                    prevDonors.insert(pair<string, vector<int> >(curKey, vector<int>()));
                    prevDonors[curKey].push_back(year);
                } else {//curKey is in the prevDonors map, this is not yet a final say as of whether we have a repeat donor or not because lines are not streamed chronologically.
                    cout << "This recipient may have a valid repeat donor." << endl;
                    if (checkPriority(prevDonors[curKey], year)) {//check that the previous contributions of the current donor were donated in prior years to the year of the current record
                        cout << "This recipient doesn't have a valid repeat donor, we don't proceed any further." << endl;
                        prevDonors[curKey].push_back(year);//for the sake of consistency, indeed, this action may not be necessary!
                    } else {
                        cout << "This recipient has a valid repeat donor." << endl;
                        prevDonors[curKey].push_back(year);
                        displayVec(prevDonors[curKey]);
                        
                        //Now that we have a record with a repeat donor, we have to check whether the recipientsWithRepeatDonors map has a key for the current recipient along with a value of a pointer to its instance or not. If recipientsWithRepeatDonors map doesn't have the current recipeint, we add instantiate an instance and add a key/value pair. If it does, we update the current objecyt in place.
                        
                        string curResult; //this is the "|the ?th percentile contribution|the total dollar amount of contributions|the total number of contributions from repeat donors"
                        recipientsWithRepeatDonorsIt = recipientsWithRepeatDonors.find(CMTE_ID);
                        if (recipientsWithRepeatDonorsIt == recipientsWithRepeatDonors.end()) {
                            recipientsWithRepeatDonors[CMTE_ID] = new RecipientWithRepeatDonors(stoi(TRANSACTION_AMT)); //Instantiate a new instance
                            curResult = recipientsWithRepeatDonors[CMTE_ID]->printRecipientWithRepeatDonors(percentile);
                        } else {
                            recipientsWithRepeatDonors[CMTE_ID]->updateRecipientWithRepeatDonors(stoi(TRANSACTION_AMT)); //Update a current instance
                            curResult = recipientsWithRepeatDonors[CMTE_ID]->printRecipientWithRepeatDonors(percentile);
                        };
                        string finalResult;
                        finalResult = CMTE_ID + "|" + ZIP_CODE + "|" + to_string(year) + curResult;
                        cout << finalResult << endl; //output in terminal insted of to an output.txt file
                    }
                };
            } else {
                inValidRecords.push_back(i);
            }
            cout << "----------" << endl;
        }
    };
    itcont.close();
    //Now, simple summary:
    cout << "here are the valid records lines: " << flush;
    displayVec(validRecords);
    cout << "----------" << endl;
    cout << "here are the invalid records lines: " << flush;
    displayVec(inValidRecords);
    return 0;
}



 
