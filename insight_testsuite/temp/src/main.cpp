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


int main(int argc, char** argv) {

    if (argc != 4) {
        cout << "Please enter the name of the program, the name of the percentile input file, the name of the records input file and finally the name of the output file." << endl;
    };

    PipelineController * current = PipelineController::Instance();
    current->analyze(argv[1], argv[2], argv[3]);

    return 0;
}
