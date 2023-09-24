#ifndef RECORD_H
#define RECORD_H

#include <iostream>

using namespace std;

const string path = "/Users/diegopachecoferrel/Documents/AED/BD2/tablas/";

struct Record
{
    char nombre[30];
    char carrera[20];
    int ciclo;
    long left=-1, right=-1;
    int height=0;
    pair<long, char> next = {-1, 'd'};

    void setData() {
        cout << "Alumno:";
        cin >> nombre;
        cout << "Carrera: ";
        cin >> carrera;
        cout << "Ciclo: ";
        cin >> ciclo;
    }

    void showData() {
        cout << nombre << " " << carrera << " " << ciclo;
        cout << endl;
    }

   string get_data() const {
        string _nombre = nombre;
        string _carrera = carrera;
        string _ciclo = to_string(ciclo);
        return _nombre + " " + _carrera + " " +_ciclo;
    }

    string getKey() const {
        return nombre;
    }

    string get_carrera() const {
        return carrera;
    }

    int get_ciclo() const {
        return ciclo;
    }
};


struct NYC{
    char id[8];
    int year;
    int month;
    int day;
    int volume;
    char street[30];

    string getKey(){
        return id;
    }

    template<class TV>
    TV getAtribute(string atribute){
        if(atribute == "id")
            return id;
        else if(atribute == "year")
            return year;
        else if(atribute == "month")
            return month;
        else if(atribute == "day")
            return day;
        else if(atribute == "volume")
            return volume;
        else if(atribute == "street")
            return street;
    }
};


#endif // RECORD_H
