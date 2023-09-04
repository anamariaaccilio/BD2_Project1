//
// Created by ACER on 03/09/2023.
//

#ifndef BD2_PROJECT1_AVL_H
#define BD2_PROJECT1_AVL_H

#include <iostream>
#include <fstream>
#include <cstring>
#include <climits>
#include <vector>

using namespace std;
struct Record
{
    int cod;
    char nombre[12];
    int ciclo;
    long left = -1, right = -1;
    int height = 0;

    void setData(){
        cout << "Codigo:";
        cin >> cod;
        cout << "Nombre: ";
        cin >> nombre;
        cout << "Ciclo: ";
        cin >> ciclo;
    }

    void showData(){
        /*
        cout << "\nCodigo: " << cod;
        cout << "\nNombre: " << nombre;
        cout << "\nCiclo : " << ciclo <<endl;*/
        cout << cod << " " << nombre << " " << ciclo << " " << left << " " << right << " " << height << endl;
    }

};

class AVLFile
{
private:
    bool primary_key = true;
    string filename;
    long pos_root;
public:
    AVLFile(string filename){
        this->pos_root = 0;
        this->filename = filename;
    }
    Record find(int key){
        return find(pos_root, key);
    }
    void insert(Record record){
        insert(pos_root, record);
    }
    vector<Record> inorder(){
        return inorder(pos_root);
    }
private:

    //altura de los nodos
    long height(long pos_node){
        ifstream file(filename, ios::binary);
        Record record;
        file.seekg(pos_node, ios::beg);
        file.read((char*)&record, sizeof(Record));
        file.close();
        if (pos_node == -1) {
            return -1;
        } else {
            return record.height;
        }
    }

    // factor de balanceo

    int balancing_factor(long pos_node){
        ifstream file(filename, ios::binary);
        Record record;
        file.seekg(pos_node, ios::beg);
        file.read((char*)&record, sizeof(Record));
        file.close();
        if (pos_node == -1) {
            return 0;
        } else {
            return height(record.right) - height(record.left);
        }
    }

    // actualización de la altura por cada nodo
    void update_height(long pos_node){
        ifstream file(filename, ios::binary);
        Record record;
        file.seekg(pos_node, ios::beg);
        file.read((char*)&record, sizeof(Record));
        file.close();
        if (pos_node != -1) {
            record.height = 1 + max(height(record.left), height(record.right));
            fstream file(filename, ios::in | ios::out | ios::binary);
            file.seekp(pos_node, ios::beg);
            file.write((char*)&record, sizeof(Record));
            file.close();
        }
    }



    //  find


    Record find(long pos_node, int key){
        ifstream file(filename, ios::binary);
        Record record;
        file.seekg(pos_node, ios::beg);
        file.read((char*)&record, sizeof(Record));

        if (record.cod == key) {
            return record;
        } else {
            if (key > record.cod) {
                if (record.right != -1) {
                    return find(record.right, key);
                } else {
                    Record notFoundRecord;
                    notFoundRecord.cod = -1;
                    return notFoundRecord;
                }
            } else if (key < record.cod) {
                if (record.left != -1) {
                    return find(record.left, key);
                } else {
                    Record notFoundRecord;
                    notFoundRecord.cod = -1;
                    return notFoundRecord;
                }
            }
        }
    }

    //insert

    void insert(long pos_node, Record record){
        fstream f(this->filename, ios::app | ios::in | ios::out | ios::binary);

        if(f.tellg() == 0){
            f.write((char*)&record, sizeof(record));
            f.close();
            return;
        }


        else{
            Record curr_record;
            long curr_pos = pos_node;

            while(true){
                f.seekg(curr_pos, ios::beg);
                f.read((char*)&curr_record, sizeof(Record));

                if(record.cod > curr_record.cod){
                    if(curr_record.right == -1){
                        f.seekg(0, ios::end);
                        curr_record.right = f.tellg();
                        f.write((char*)&record, sizeof(Record));
                        f.close();

                        fstream file(this->filename, ios::in | ios::out | ios::binary);
                        file.seekp(curr_pos, ios::beg);
                        file.write((char*)&curr_record, sizeof(Record));
                        file.close();
                        return;
                    }
                    else{
                        curr_pos = curr_record.right;
                    }
                }else if(record.cod < curr_record.cod){
                    if(curr_record.left == -1){
                        f.seekg(0, ios::end);
                        curr_record.left = f.tellg();
                        f.write((char*)&record, sizeof(Record));
                        f.close();

                        fstream file(this->filename, ios::in | ios::out | ios::binary);
                        file.seekp(curr_pos, ios::beg);
                        file.write((char*)&curr_record, sizeof(Record));
                        file.close();

                        return;
                    }
                    else{
                        curr_pos = curr_record.left;
                    }
                }else{
                    f.close();
                    return;
                }
            }

        }
    }
    /*

    vector<Record> inorder(long pos_node){
        vector<Record> result;

        if (pos_node != -1) {
            ifstream file(filename, ios::binary);
            Record record;
            file.seekg(pos_node, ios::beg);
            file.read((char*)&record, sizeof(Record));

            vector<Record> leftSubtree = inorder(record.left);
            result.insert(result.end(), leftSubtree.begin(), leftSubtree.end());

            result.push_back(record);

            vector<Record> rightSubtree = inorder(record.right);
            result.insert(result.end(), rightSubtree.begin(), rightSubtree.end());

            file.close();
        }

        return result;
    }
     */

    // rotación a la derecha
    void right_rotation(long &pos_node){
    }

    // rotación a la izquierda
    void left_rotation(long &pos_node){
    }

    // balanceo

    void balance(long &pos_node){
    }

};

void writeFile(string filename){
    AVLFile file(filename);
    Record record;
    for (int i = 0; i < 3; i++)
    {
        record.setData();
        file.insert(record);
    }
}

void readFile(string filename){
    AVLFile file(filename);
    cout<<"--------- show all sorted data -----------\n";
    vector<Record> result = file.inorder();
    for(Record re : result){
        re.showData();
    }
}

int main() {
    writeFile("data.bin");
    readFile("data.bin");
    return 0;
}

#endif //BD2_PROJECT1_AVL_H
