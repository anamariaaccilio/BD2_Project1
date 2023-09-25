#ifndef RECORD_H
#define RECORD_H

#include <iostream>
#include <fstream>

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

    // Función de serialización para Record
    void serialize(ofstream& file) const {
        file.write(reinterpret_cast<const char*>(&id), sizeof(int));
        file.write(district, sizeof(district));
        file.write(reinterpret_cast<const char*>(&year), sizeof(int));
        file.write(reinterpret_cast<const char*>(&month), sizeof(int));
        file.write(reinterpret_cast<const char*>(&day), sizeof(int));
        file.write(reinterpret_cast<const char*>(&vol), sizeof(int));
        file.write(street, sizeof(street));
    }
    void serialize(fstream& file) const {
        file.write(reinterpret_cast<const char*>(&id), sizeof(int));
        file.write(district, sizeof(district));
        file.write(reinterpret_cast<const char*>(&year), sizeof(int));
        file.write(reinterpret_cast<const char*>(&month), sizeof(int));
        file.write(reinterpret_cast<const char*>(&day), sizeof(int));
        file.write(reinterpret_cast<const char*>(&vol), sizeof(int));
        file.write(street, sizeof(street));
    }

    // Función de deserialización para Record
    void deserialize(ifstream& file) {
        file.read(reinterpret_cast<char*>(&id), sizeof(int));
        file.read(district, sizeof(district));
        file.read(reinterpret_cast<char*>(&year), sizeof(int));
        file.read(reinterpret_cast<char*>(&month), sizeof(int));
        file.read(reinterpret_cast<char*>(&day), sizeof(int));
        file.read(reinterpret_cast<char*>(&vol), sizeof(int));
        file.read(street, sizeof(street));
    }
    void deserialize(fstream& file) {
        file.read(reinterpret_cast<char*>(&id), sizeof(int));
        file.read(district, sizeof(district));
        file.read(reinterpret_cast<char*>(&year), sizeof(int));
        file.read(reinterpret_cast<char*>(&month), sizeof(int));
        file.read(reinterpret_cast<char*>(&day), sizeof(int));
        file.read(reinterpret_cast<char*>(&vol), sizeof(int));
        file.read(street, sizeof(street));
    }
};



#endif // RECORD_H
