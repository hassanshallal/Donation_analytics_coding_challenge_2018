/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   RecWiRepDon.h
 * Author: hassanshallal
 *
 * Created on February 8, 2018, 8:58 PM
 */

#ifndef RECWIREPDON_H
#define RECWIREPDON_H

#include <vector>
#include <string>
#include <math.h>

using namespace std;
class RecWiRepDon {
private:
    //Two private data members; a vector of ascending sorted contributions and an int of total contributions
    vector <int> contFrRepDon;
    int totalContributions;
    
    //One private method for insertion in the correct ascending order
    void asscendingSortedVectorPushBack(int curContribution) {
        for (size_t i = 0; i < contFrRepDon.size(); i++) {
            if (curContribution <= contFrRepDon[i]) {
                contFrRepDon.insert(contFrRepDon.begin() + i, curContribution);
                break;
            } 
        };
        contFrRepDon.push_back(curContribution);
    }

public:

    //There is only one constructor with an amount of current contribution. We only use this constructor
    // whenever we have a recipient with a repeat donor that hasn't been instantiated yet. 
    RecWiRepDon(int curContribution) {
        contFrRepDon.push_back(curContribution);
        totalContributions = curContribution;
    }

    //Default destructor
    virtual ~RecWiRepDon() {
    }
    
    //We update an existing RecWiRepDon in place using this method
    void updateRecWiRepDon(int curContribution) {
        asscendingSortedVectorPushBack(curContribution);
        totalContributions += curContribution;
    };

    // This final method is to return a string of what's needed from any RecWiRepDon. This string will still be concatenated
    // with other data in the pipelineController
    
    string printRecWiRepDon(int percentile){
        int percentileIndex = ceil((percentile / 100) * contFrRepDon.size());
        string result;
        result = "|" + to_string(contFrRepDon[percentileIndex]) + "|" + to_string(totalContributions) + "|" + to_string(contFrRepDon.size());
        return result;
    }
};

#endif /* RECWIREPDON_H */

