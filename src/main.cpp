/*
 * This is my attempt at Insight Coding Challenge for the 2018 year!
 * File:   main.cpp
 * Author: hassanshallal
 *
 * Created on February 6, 2018, 6:06 PM
 */


#include "PipelineController.h"

template<class ItemType>
void displayVec(vector<ItemType> vec) {
    for (size_t i = 0; i < vec.size(); ++i)
        cout << vec[i] << " " << flush;
    cout << "\n" << endl;
}

template<class ItemType>
void saveVecIntoTxt(vector<ItemType> vec, string outputName) {
    //Let's open an output file
    ofstream output;
    output.open(outputName);

    for (size_t i = 0; i < vec.size(); ++i)
        output << vec[i] << endl;

    output.close();
}

int main(int argc, char** argv) {

    PipelineController * current = PipelineController::Instance();
    vector<int> inValidRecords = current->analyze();
    saveVecIntoTxt(inValidRecords, "global_invalid_records.txt");
   
    return 0;
}

// RUN FINISHED; exit value 0; real time: 2h 21m 42s; user: 5m 46s; system: 2h 13m 52s