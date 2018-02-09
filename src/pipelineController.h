/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   pipelineController.h
 * Author: hassanshallal
 *
 * Created on February 8, 2018, 8:57 PM
 */

#ifndef PIPELINECONTROLLER_H
#define PIPELINECONTROLLER_H

#include <iostream>
#include <fstream>
#include <map>

#include "RecWiRepDon.h"

using namespace std;

class pipelineController {
    
private:
    
    int percentile;
    //Initialize an empty map and a corresponding iterator for the previous donors. Also initialize a bool
    map<string, vector<int>> prevDonors;
    map<string, vector<int>>::iterator prevDonorsIt;
    bool isRepeatDonor = false;

    //Initialize an empty map and a corresponding iterator for the recipients with repeat donors
    map<string, RecWiRepDon*> RecWiRepDonMap;
    map<string, RecWiRepDon*>::iterator RecWiRepDonMapIt;


    //Keep record count, keep track of valid and invalid lines
    int i = 0; //record counter
    vector<int> validRecords;
    vector<int> inValidRecords;

    //We will have line-specific variables that are needed for decision making and for output, these variables are update every line
    string CMTE_ID;
    string NAME;
    string ZIP_CODE;
    int year;
    string TRANSACTION_AMT;

public:

    pipelineController(const string inputPercentile, const string inputRecords, const string outputName);

    //Two utility methods
    vector<int> findPipeLocations(const string &sample) const; // Find the pipe locations in a string
    bool checkPriority(const vector<int> years, const int curRecordYear) const; // find whether a year is not prior to a vector of years

    // We need to have some validation functions that validates the important entries in each line
    bool checkRecipient(const string recipient) const;
    bool checkZIP_CODE(const string ZIP_CODE) const;
    bool checkTRANSACTION_DT(const string TRANSACTION_DT) const;
    bool checkTRANSACTION_AMT(const string TRANSACTION_AMT) const;
    bool isValidRecord(string curLine);


};

#endif /* PIPELINECONTROLLER_H */

// Implementation of the .h methods

pipelineController::pipelineController(const string inputPercentile, const string inputRecords, const string outputName) {

    // Open the percentile file, get the percentile value
    ifstream percentileInput;
    percentileInput.open(inputPercentile);
    percentileInput >> percentile; //percentile is in the first line
    percentileInput.close();


    //We will read the input line by line
    ifstream itcont;
    itcont.open(inputRecords);


    //Let's open an output file
    ofstream output;
    output.open(outputName);

    //We will start reading line by line and writing the results to the output file line by line
    string a; //whole line

    while (!itcont.eof()) {
        getline(itcont, a);
        if (!a.empty()) {//if a is empty, ignore it or otherwise you obtain segmentation fault when findLocations function act on an empty string, we haven;t included any exceptions for edge cases yet
            ++i; //increment the line/record counter

            //Whenever we have a valid record, we want to check whether we have a repeat donor or not AND we want to populate/update the prevDonors map with the current record accordingly
            if (isValidRecord(a)) {
                validRecords.push_back(i);

                string curKey = NAME + "_" + ZIP_CODE; //Based on the assumption that the a repeat donor must have the same name and the same zip code
                prevDonorsIt = prevDonors.find(curKey); //is the curKey already in the prevDonors map?

                if (prevDonorsIt == prevDonors.end()) {//curKey is not in the prevDonors map
                    prevDonors.insert(pair<string, vector<int> >(curKey, vector<int>()));
                    prevDonors[curKey].push_back(year);
                } else {//curKey is in the prevDonors map, this is not yet a final say as of whether we have a repeat donor or not because lines are not streamed chronologically.
                    if (checkPriority(prevDonors[curKey], year)) {//check that the previous contributions of the current donor were donated in prior years to the year of the current record
                        prevDonors[curKey].push_back(year); //for the sake of consistency, indeed, this action may not be necessary!
                    } else {
                        prevDonors[curKey].push_back(year);

                        //Now that we have a record with a repeat donor, we have to check whether the RecWiRepDonMap map has a key for the current recipient along with a value of a pointer to its instance or not. If RecWiRepDonMap map doesn't have the current recipient, we add instantiate an instance and add a key/value pair. If it does, we update the current object in place.
                        string curResult; //this is the "|the ?th percentile contribution|the total dollar amount of contributions|the total number of contributions from repeat donors"
                        RecWiRepDonMapIt = RecWiRepDonMap.find(CMTE_ID);
                        if (RecWiRepDonMapIt == RecWiRepDonMap.end()) {
                            RecWiRepDonMap[CMTE_ID] = new RecWiRepDon(stoi(TRANSACTION_AMT)); //Instantiate a new instance
                            curResult = RecWiRepDonMap[CMTE_ID]->printRecWiRepDon(percentile);
                        } else {
                            RecWiRepDonMap[CMTE_ID]->updateRecWiRepDon(stoi(TRANSACTION_AMT)); //Update a current instance
                            curResult = RecWiRepDonMap[CMTE_ID]->printRecWiRepDon(percentile);
                        };
                        string finalResult;
                        finalResult = CMTE_ID + "|" + ZIP_CODE + "|" + to_string(year) + curResult;
                        output << finalResult << endl;
                    }
                };
            } else {
                inValidRecords.push_back(i);
            }
        }
    }
    //close all open files
    itcont.close();
    output.close();
}

bool pipelineController::isValidRecord(string curLine) {

    //Some local variables for validating a single record of the input file
    string TRANSACTION_DT;
    string OTHER_ID;
    bool validCMTE_ID = false;
    bool validZIP_CODE = false;
    bool validTRANSACTION_DT = false;
    bool validTRANSACTION_AMT = false;
    bool isIndividual = false;
    bool validRecord = false;


    //find all the pipe locations
    vector<int> pipeLocVec = findPipeLocations(curLine);

    //extract and validate CMTE_ID
    CMTE_ID = curLine.substr(0, pipeLocVec.at(0));
    if (checkRecipient(CMTE_ID)) {
        validCMTE_ID = true;
    };

    //Extract name, any thing can be a name, no validation is required here
    NAME = curLine.substr(pipeLocVec[6] + 1, pipeLocVec[7] - pipeLocVec[6] - 1);
    //Extract and validate ZIP_CODE
    ZIP_CODE = curLine.substr(pipeLocVec[9] + 1, (pipeLocVec[10] - pipeLocVec[9] - 1));
    if (checkZIP_CODE(ZIP_CODE)) {
        ZIP_CODE = ZIP_CODE.substr(0, 5);
        validZIP_CODE = true;
    };

    //Extract and validate TRANSACTION_DT
    TRANSACTION_DT = curLine.substr(pipeLocVec[12] + 1, pipeLocVec[13] - pipeLocVec[12] - 1);
    if (checkTRANSACTION_DT(TRANSACTION_DT)) {
        validTRANSACTION_DT = true;
    };

    year = stoi(TRANSACTION_DT.substr(4, 4));

    //Extract and validate TRANSACTION_AMT
    TRANSACTION_AMT = curLine.substr(pipeLocVec[13] + 1, pipeLocVec[14] - pipeLocVec[13] - 1);
    if (checkTRANSACTION_AMT(TRANSACTION_AMT)) {
        validTRANSACTION_AMT = true;
    };

    //Extract and validate OTHER_ID
    OTHER_ID = curLine.substr(pipeLocVec[14] + 1, pipeLocVec[15] - pipeLocVec[14] - 1);
    if (OTHER_ID.empty()) {
        isIndividual = true;
    };

    return (validCMTE_ID && validZIP_CODE && validTRANSACTION_DT && validTRANSACTION_AMT && isIndividual);
}

vector<int> pipelineController::findPipeLocations(const string &sample) const {
    vector<int> positions;
    for (size_t i = 0; i < sample.size(); ++i) {
        if (sample[i] == '|') {
            positions.push_back(i);
        };
    };
    return positions;
}

bool pipelineController::checkRecipient(const string recipient) const {
    if (recipient.size() != 9) {
        return false;
    };

    //Is first character == 'C'
    char firstChar;
    firstChar = recipient[0];
    bool isC = (firstChar == 'C');

    //Are the last 8 characters all digits?
    string last8 = recipient.substr(1, 8);
    size_t found = last8.find_first_not_of("0123456789");
    bool last8Digits = true;
    if (found != string::npos) {
        last8Digits = false;
    };

    //final check
    if (!isC || !last8Digits) {
        return false;
    } else {
        return true;
    };
}

bool pipelineController::checkZIP_CODE(const string ZIP_CODE) const {
    //at least 5 digits
    if (ZIP_CODE.size() < 5) {
        return false;
    } else {
        return all_of(ZIP_CODE.begin(), ZIP_CODE.end(), ::isdigit); //the zip code must only contain digits otherwise it is malformed
    }
}

bool pipelineController::checkTRANSACTION_DT(const string TRANSACTION_DT) const {
    //at least 8 digits
    if (TRANSACTION_DT.size() != 8) {
        return false;
    } else {
        return true;
    };
}

bool pipelineController::checkTRANSACTION_AMT(const string TRANSACTION_AMT) const {
    return all_of(TRANSACTION_AMT.begin(), TRANSACTION_AMT.end(), ::isdigit);
}

bool pipelineController::checkPriority(const vector<int> years, const int curRecordYear) const {
    for (size_t i = 0; i < years.size(); ++i) {
        if (years[i] > curRecordYear) {
            return true;
        };
    };
    return false;
}
