#ifndef RECORD_H
#define RECORD_H

#include <iostream>

using namespace std;

const string path = "/Users/diegopachecoferrel/Documents/AED/BD2/tablas/";

struct Record
{
    int id;
    char district[15];
    int year;
    int month;
    int day;
    int vol;
    char street[30];


    long left=-1, right=-1;
    int height=0;
    pair<long, char> next = {-1, 'd'};

    int getKey() const {
        return id;
    }

    string get_street() const {
        return street;
    }
};



#endif // RECORD_H
