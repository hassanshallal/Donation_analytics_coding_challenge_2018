/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   PipelineController.h
 * Author: hassanshallal
 *
 * Created on February 8, 2018, 8:57 PM
 */

#ifndef PIPELINECONTROLLER_H
#define PIPELINECONTROLLER_H

#include <iostream>
#include <fstream>
#include <map>
#include <ctime>

#include "RecWiRepDon.h"

using namespace std;

class PipelineController {
private:
    //Input and output file names are static private members
    const string inputPercentile = "percentile.txt";
    const string inputRecords = "/Users/hassanshallal/Downloads/itcont.txt"; 
    const string outputName = "/Users/hassanshallal/Downloads/repeat_donors.txt";
    const string statsName = "globalStatsExp2.txt";
    

    //The current percentile
    int percentile;

    //Initialize an empty map and a corresponding iterator for the previous donors. Also initialize a bool
    map<string, vector<int>> prevDonors;
    map<string, vector<int>>::iterator prevDonorsIt;
    bool isRepeatDonor = false;

    //Initialize an empty map and a corresponding iterator for the recipients with repeat donors
    map<string, RecWiRepDon*> RecWiRepDonMap;
    map<string, RecWiRepDon*>::iterator RecWiRepDonMapIt;

    //We will have line-specific variables that are needed for decision making and for output, these variables are update every line
    string recipientID;
    string name;
    string zipCode;
    int year;
    string donationAmount;

    //PipelineController is a singleton, it has a private constructor in which the input and output files are specified
    PipelineController();
    //private pointer to the solo-instance
    static PipelineController* _instance;

public:

    //public instance method for singleton
    static PipelineController* Instance();

    //Keep record count, keep track of valid and invalid lines
    int i = 0; //input record counter
    int j = 0; //output record counter
    int k = 1000000; //Print global stats in the globalStats output every k lines.
    vector<int> validRecords;
    vector<int> inValidRecords;

    vector<int> analyze();

    //Two utility methods
    vector<int> findPipeLocations(const string &sample) const; // Find the pipe locations in a string
    bool checkPriority(const vector<int> years, const int curRecordYear) const; // find whether a year is not prior to a vector of years

    // We need to have some validation functions that validates the important entries in each line
    bool checkRecipient(const string recipient) const;
    bool checkzipCode(const string zipCode) const;
    bool checkdonationDate(const string donationDate) const;
    bool checkdonationAmount(const string donationAmount) const;
    bool isValidRecord(string curLine);


};

#endif /* PIPELINECONTROLLER_H */

// Implementation of the .h methods
PipelineController* PipelineController::_instance = nullptr;

PipelineController::PipelineController() {
};

PipelineController* PipelineController::Instance() {
    if (_instance == nullptr) {
        _instance = new PipelineController;
    }
    return _instance;
}

vector<int> PipelineController::analyze() {
    auto start = std::chrono::system_clock::now();
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

    ofstream statsOutput;
    statsOutput.open(statsName);

    //We will start reading line by line and writing the results to the output file line by line
    string a; //whole line

    while (!itcont.eof()) {
        getline(itcont, a);
        if (!a.empty()) {//if a is empty, ignore it or otherwise you obtain segmentation fault when findLocations function act on an empty string, we haven;t included any exceptions for edge cases yet
            ++i; //increment the line/record counter

            //Whenever we have a valid record, we want to check whether we have a repeat donor or not AND we want to populate/update the prevDonors map with the current record accordingly
            if (isValidRecord(a)) {
                validRecords.push_back(i);

                string curKey = name + "_" + zipCode; //Based on the assumption that the a repeat donor must have the same name and the same zip code
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
                        RecWiRepDonMapIt = RecWiRepDonMap.find(recipientID);
                        if (RecWiRepDonMapIt == RecWiRepDonMap.end()) {
                            RecWiRepDonMap[recipientID] = new RecWiRepDon(stoi(donationAmount)); //Instantiate a new instance
                            curResult = RecWiRepDonMap[recipientID]->printRecWiRepDon(percentile);
                        } else {
                            RecWiRepDonMap[recipientID]->updateRecWiRepDon(stoi(donationAmount)); //Update a current instance
                            curResult = RecWiRepDonMap[recipientID]->printRecWiRepDon(percentile);
                        };
                        string finalResult;
                        finalResult = recipientID + "|" + zipCode + "|" + to_string(year) + curResult;
                        output << finalResult << endl;
                        j++;
                    }
                };
            } else {
                inValidRecords.push_back(i);
            }
            if (i % k == 0) {
                statsOutput << "The number of processed records so far is: " << to_string(i) << endl;
                statsOutput << "The number of records in the output file: " << to_string(j) << endl;
                statsOutput << "The number of invalid records is: " << inValidRecords.size() << endl;
                statsOutput << "The number of valid records is: " << validRecords.size() << endl;
                statsOutput << "The number of repeat donors detected so far is: " << prevDonors.size() << endl;
                statsOutput << "The number of recipients with repeat donors detected so far is: " << RecWiRepDonMap.size() << endl;
                auto end = chrono::system_clock::now();
                chrono::duration<double> elapsed_seconds = end - start;
                time_t end_time = chrono::system_clock::to_time_t(end);
                statsOutput << "Here is the time-taken to run the pipeline up to this point: " << flush;
                statsOutput << elapsed_seconds.count() << "s\n";
                statsOutput << "========================================================================================" << endl;
            }
        }
    }

    statsOutput << "========================================================================================" << endl;
    statsOutput << "Here are the final stats: " << endl;
    statsOutput << "The total number of processed records  is: " << to_string(i) << endl;
    statsOutput << "The number of records in the output file: " << to_string(j) << endl;
    statsOutput << "The number of invalid records is: " << inValidRecords.size() << endl;
    statsOutput << "The number of valid records is: " << validRecords.size() << endl;
    statsOutput << "The number of repeat donors detected so far is: " << prevDonors.size() << endl;
    statsOutput << "The number of recipients with repeat donors detected so far is: " << RecWiRepDonMap.size() << endl;

    auto end = chrono::system_clock::now();
    chrono::duration<double> elapsed_seconds = end - start;
    time_t end_time = chrono::system_clock::to_time_t(end);
    statsOutput << "========================================================================================" << endl;
    statsOutput << "Here is the time-taken to run the pipeline of all donations from 2013 till now: " << endl;
    statsOutput << "finished computation at " << ctime(&end_time) << "elapsed time: " << elapsed_seconds.count() << "s\n";


    //close all open files
    itcont.close();
    output.close();
    statsOutput.close();
    return inValidRecords;
}

bool PipelineController::isValidRecord(string curLine) {

    //Some local variables for validating a single record of the input file
    string donationDate;
    string individualOrEntity;
    bool validrecipientID = false;
    bool validzipCode = false;
    bool validDonationDate = false;
    bool validDonationAmount = false;
    bool isIndividual = false;
    bool validRecord = false;


    //find all the pipe locations
    vector<int> pipeLocVec = findPipeLocations(curLine);

    //extract and validate recipientID
    recipientID = curLine.substr(0, pipeLocVec.at(0));
    if (recipientID.size() > 0 && checkRecipient(recipientID)) {
        validrecipientID = true;
    };

    //Extract name, any thing can be a name, no validation is required here
    name = curLine.substr(pipeLocVec[6] + 1, pipeLocVec[7] - pipeLocVec[6] - 1);
    //Extract and validate zipCode
    zipCode = curLine.substr(pipeLocVec[9] + 1, (pipeLocVec[10] - pipeLocVec[9] - 1));
    if (zipCode.size() > 0 && checkzipCode(zipCode)) {
        zipCode = zipCode.substr(0, 5);
        validzipCode = true;
    };

    //Extract and validate donationDate

    donationDate = curLine.substr(pipeLocVec[12] + 1, pipeLocVec[13] - pipeLocVec[12] - 1);
    if (donationDate.size() > 0 && checkdonationDate(donationDate)) {
        validDonationDate = true;
        year = stoi(donationDate.substr(4, 4));
    };

    //Extract and validate donationAmount
    donationAmount = curLine.substr(pipeLocVec[13] + 1, pipeLocVec[14] - pipeLocVec[13] - 1);
    if (donationAmount.size() > 0 && checkdonationAmount(donationAmount)) {
        validDonationAmount = true;
    };

    //Extract and validate individualOrEntity
    individualOrEntity = curLine.substr(pipeLocVec[14] + 1, pipeLocVec[15] - pipeLocVec[14] - 1);
    if (individualOrEntity.empty()) {
        isIndividual = true;
    };

    return (validrecipientID && validzipCode && validDonationDate && validDonationAmount && isIndividual);
}

vector<int> PipelineController::findPipeLocations(const string &sample) const {
    vector<int> positions;
    for (size_t i = 0; i < sample.size(); ++i) {
        if (sample[i] == '|') {
            positions.push_back(i);
        };
    };
    return positions;
}

bool PipelineController::checkRecipient(const string recipient) const {
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

bool PipelineController::checkzipCode(const string zipCode) const {
    //at least 5 digits
    if (zipCode.size() < 5) {
        return false;
    } else {
        return all_of(zipCode.begin(), zipCode.end(), ::isdigit); //the zip code must only contain digits otherwise it is malformed
    }
}

bool PipelineController::checkdonationDate(const string donationDate) const {
    //at least 8 digits
    if (donationDate.size() != 8) {
        return false;
    } else {
        return true;
    };
}

bool PipelineController::checkdonationAmount(const string donationAmount) const {
    return all_of(donationAmount.begin(), donationAmount.end(), ::isdigit);
}

bool PipelineController::checkPriority(const vector<int> years, const int curRecordYear) const {
    for (size_t i = 0; i < years.size(); ++i) {
        if (years[i] > curRecordYear) {
            return true;
        };
    };
    return false;
}
