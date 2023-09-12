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
        //cout << "\nCodigo: " << cod;
        //cout << "\nNombre: " << nombre;
        //cout << "\nCiclo : " << ciclo <<endl;
        cout << cod << " " << nombre << " " << ciclo << endl;
    }

};

class AVLFile
{
private:
    string filename;
    long pos_root;
public:
    AVLFile(string filename){
        this->pos_root = 0;
        this->filename = filename;

        ifstream file(filename, ios::app | ios::binary);

        if(!file.good()){
            file.close();
            ofstream f(filename, ios::app | ios::binary);
            f.close();
        }

    }

    Record find(int key){
        ifstream file(filename, ios::binary);
        return find(pos_root, key, file);
        file.close();
    }

    void insert(Record record){
        fstream f(this->filename, ios::in | ios::out | ios::binary);
        insert(pos_root, record, f);
        f.close();
    }

    template <class T>
    void remove(T key){
        fstream f(this->filename, ios::in | ios::out | ios::binary);
        remove(pos_root, key, f);
        f.close();
    }

    vector<Record> inorder(){
        return inorder(pos_root);
    }

    //searchrange 


    vector<Record> searchRange(int start, int end, int i, vector<struct Record> vector) {
        std::vector<Record> result;
        searchRange(pos_root, start, end, result);
        return result;
    }

private:
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

    Record find(long pos_node, int key, ifstream& file){
        Record record;
        file.seekg(pos_node, ios::beg);
        file.read((char*)&record, sizeof(Record));

        if (record.cod == key) {
            return record;
        } else {
            if (key > record.cod) {
                if (record.right != -1) {
                    return find(record.right, key, file);
                } else {
                    Record notFoundRecord;
                    notFoundRecord.cod = -1;
                    return notFoundRecord;
                }
            } else if (key < record.cod) {
                if (record.left != -1) {
                    return find(record.left, key, file);
                } else {
                    Record notFoundRecord;
                    notFoundRecord.cod = -1;
                    return notFoundRecord;
                }
            }
        }
    }

    void insert(long pos_node, Record record, fstream& file){
        file.seekg(0, ios::end);
        if(file.tellg() == 0){
            file.write((char*)&record, sizeof(Record));
            return;
        }

        Record curr_record;
        file.seekg(pos_node, ios::beg);
        file.read((char*)&curr_record, sizeof(Record));

        if(record.cod < curr_record.cod){
            if(curr_record.left == -1){
                file.seekp(0, ios::end);
                curr_record.left = file.tellg();
                file.write((char*)&record, sizeof(Record));
                file.seekg(pos_node, ios::beg);
                file.write((char*)&curr_record, sizeof(Record));
            }
            else
                insert(curr_record.left, record, file);
        }else if(record.cod > curr_record.cod){
            if(curr_record.right == -1){
                file.seekp(0, ios::end);
                curr_record.right = file.tellg();
                file.write((char*)&record, sizeof(Record));
                file.seekg(pos_node, ios::beg);
                file.write((char*)&curr_record, sizeof(Record));
            }
            else
                insert(curr_record.right, record, file);
        }

        updateHeight(pos_node, file);
        balance(pos_node, file);

    }

    //complete

    template <class T>
    void remove(long pos_node, T key, fstream& file){
        if(pos_node == -1)
            return;
        Record curr_record;
        file.seekg(pos_node, ios::beg);
        file.read((char*)&curr_record, sizeof(Record));

        if(key < curr_record.cod)
            remove(curr_record.left, key, file);
        else if(key > curr_record.cod)
            remove(curr_record.right, key, file);
        else{
            if(curr_record.left == -1 && curr_record.right == -1){
                curr_record.cod = -1;
                file.seekp(pos_node, ios::beg);
                file.write((char*)&curr_record, sizeof(Record));
            }
            else if(curr_record.left == -1){
                Record right_record;
                file.seekg(curr_record.right, ios::beg);
                file.read((char*)&right_record, sizeof(Record));
                curr_record.cod = right_record.cod;
                curr_record.right = right_record.right;
                curr_record.left = right_record.left;
                file.seekp(pos_node, ios::beg);
                file.write((char*)&curr_record, sizeof(Record));
            }
            else if(curr_record.right == -1){
                Record left_record;
                file.seekg(curr_record.left, ios::beg);
                file.read((char*)&left_record, sizeof(Record));
                curr_record.cod = left_record.cod;
                curr_record.right = left_record.right;
                curr_record.left = left_record.left;
                file.seekp(pos_node, ios::beg);
                file.write((char*)&curr_record, sizeof(Record));
            }
            else{
                Record left_record;
                file.seekg(curr_record.left, ios::beg);
                file.read((char*)&left_record, sizeof(Record));
                curr_record.cod = left_record.cod;
                curr_record.right = left_record.right;
                curr_record.left = left_record.left;
                file.seekp(pos_node, ios::beg);
                file.write((char*)&curr_record, sizeof(Record));
                remove(curr_record.left, left_record.cod, file);

            }
        }
        updateHeight(pos_node, file);
        balance(pos_node, file);

    }

    //searchrange

    void searchRange(long pos_node, int start, int end, vector<Record>& result, ifstream& file){
        if(pos_node == -1)
            return;
        Record curr_record;
        file.seekg(pos_node, ios::beg);
        file.read((char*)&curr_record, sizeof(Record));

        if(curr_record.cod >= start && curr_record.cod <= end){
            result.push_back(curr_record);
            searchRange(curr_record.left, start, end, result, file);
            searchRange(curr_record.right, start, end, result, file);
        }
        else if(curr_record.cod < start)
            searchRange(curr_record.right, start, end, result, file);
        else if(curr_record.cod > end)
            searchRange(curr_record.left, start, end, result, file);
    }

    // AVL

    int height(long pos_node, fstream& file){
        if(pos_node == -1)
            return -1;
        Record record;
        file.seekg(pos_node, ios::beg);
        file.read((char*)&record, sizeof(Record));
        return record.height;
    }

    void updateHeight(long pos_node, fstream& file){
        if (pos_node == -1)
            return;

        Record record;
        file.seekg(pos_node, ios::beg);
        file.read((char*)&record, sizeof(Record));

        int left_height = height(record.left, file);
        int right_height = height(record.right, file);

        record.height = max(left_height, right_height) + 1;

        file.seekp(pos_node, ios::beg);
        file.write((char*)&record, sizeof(Record));
    }

    int balancingFactor(long pos_node, fstream& file){
        if(pos_node == -1)
            return 0;
        Record record;
        file.seekg(pos_node, ios::beg);
        file.read((char*)&record, sizeof(Record));
        return height(record.left, file) - height(record.right, file);
    }

    void balance(long pos_node, fstream& file){
        if(pos_node == -1)
            return;

        Record record;
        file.seekg(pos_node, ios::beg);
        file.read((char*)&record, sizeof(Record));

        if(balancingFactor(pos_node, file) == 2){
            if(balancingFactor(record.left, file) < 0){
                left_rota(record.left, file);
            }
            right_rota(pos_node, file);
        }
        else if(balancingFactor(pos_node, file) == -2){
            if(balancingFactor(record.right, file) > 0){
                right_rota(record.right, file);
            }
            left_rota(pos_node, file);
        }
        updateHeight(pos_node, file);
    }

    void left_rota(long pos_node, fstream& file){
        Record record;
        file.seekg(pos_node, ios::beg);
        file.read((char*)&record, sizeof(Record));

        Record temp;
        long pos_temp = record.right;
        file.seekg(pos_temp, ios::beg);
        file.read((char*)&temp, sizeof(Record));

        record.right = temp.left;
        temp.left = pos_temp;

        file.seekg(pos_node, ios::beg);
        file.write((char*)&temp, sizeof(Record));

        file.seekg(pos_temp, ios::beg);
        file.write((char*)&record, sizeof(Record));

        updateHeight(pos_node, file);
        updateHeight(pos_temp, file);
    }

    void right_rota(long pos_node, fstream& file){
        Record record;
        file.seekg(pos_node, ios::beg);
        file.read((char*)&record, sizeof(Record));

        Record temp;
        long pos_temp = record.left;
        file.seekg(pos_temp, ios::beg);
        file.read((char*)&temp, sizeof(Record));

        record.left = temp.right;
        temp.right = pos_temp;

        file.seekg(pos_node, ios::beg);
        file.write((char*)&temp, sizeof(Record));

        file.seekg(pos_temp, ios::beg);
        file.write((char*)&record, sizeof(Record));

        updateHeight(pos_node, file);
        updateHeight(pos_temp, file);
    }


};

void writeFile(string filename){
    AVLFile file(filename);
    Record record;
    for (int i = 0; i < 1; i++)
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