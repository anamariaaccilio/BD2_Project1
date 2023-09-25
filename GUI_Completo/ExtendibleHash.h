#ifndef EXTENDIBLEHASH_H
#define EXTENDIBLEHASH_H


#include <string>
#include <iostream>
#include <vector>
#include <ostream>
#include <fstream>
#include <cstring>
#include <cmath> // Para log2
#include <unordered_map>
#include <bitset>
#include <Record.h>

using namespace std;

struct Bucket {
    vector<Record> records;
    int capacidad;
    int size;
    Bucket(){

    }

    size_t sizeInBits() {
        size_t sizeBits = 0;

        // Tamaño de 'capacidad' en bits
        sizeBits += sizeof(this->capacidad);

        // Tamaño de 'size' en bits
        sizeBits += sizeof(this->size);

        // Tamaño de cada registro en bits (key + data)
        for (const Record& record : this->records) {
            sizeBits += sizeof(record.id);
            sizeBits += sizeof(record.district);
            sizeBits += sizeof(record.year);
            sizeBits += sizeof(record.month);
            sizeBits += sizeof(record.day);
            sizeBits += sizeof(record.vol);
            sizeBits += sizeof(record.street);
        }

        return sizeBits;
    }

    Bucket(int fb) : capacidad(fb), size(0) {
        records = vector<Record>(fb);
        for (int i = 0; i < fb; ++i) {
            records[i].id = -1; // Inicializa todos los registros con -1
            memset(records[i].district, 0, sizeof(records[i].district)); // Inicializa los datos con 0
            records[i].year = -1; // Inicializa todos los registros con -1
            records[i].month = -1; // Inicializa todos los registros con -1
            records[i].day = -1; // Inicializa todos los registros con -1
            records[i].vol = -1; // Inicializa todos los registros con -1
            memset(records[i].street, 0, sizeof(records[i].street)); // Inicializa los datos con 0
        }
    }
    Bucket(const Bucket& other) : capacidad(other.capacidad), size(other.size), records(other.records) {
    }

    // Operador de asignación de copia
    Bucket& operator=(const Bucket& other) {
        if (this != &other) {
            capacidad = other.capacidad;
            size = other.size;
            records = other.records;
        }
        return *this;
    }
    // Serializa el Bucket en un flujo de salida binario
    void serialize(ofstream& file) {
        file.write(reinterpret_cast<const char*>(&capacidad), sizeof(int));
        file.write(reinterpret_cast<const char*>(&size), sizeof(int));

        // Utiliza la función de serialización de Record para escribir cada registro en el vector
        for (const Record& record : records) {
            record.serialize(file);
        }
    }
    void serialize(fstream& file) {
        file.write(reinterpret_cast<const char*>(&capacidad), sizeof(int));
        file.write(reinterpret_cast<const char*>(&size), sizeof(int));

        // Utiliza la función de serialización de Record para escribir cada registro en el vector
        for (const Record& record : records) {
            record.serialize(file);
        }
    }

    // Deserializa el Bucket desde un flujo de entrada binario
    void deserialize(ifstream& file) {
        file.read(reinterpret_cast<char*>(&capacidad), sizeof(int));
        file.read(reinterpret_cast<char*>(&size), sizeof(int));

        // Utiliza la función de deserialización de Record para leer cada registro del archivo
        for (Record& record : records) {
            record.deserialize(file);
        }
    }
    void deserialize(fstream & file) {
        file.read(reinterpret_cast<char*>(&capacidad), sizeof(int));
        file.read(reinterpret_cast<char*>(&size), sizeof(int));

        // Utiliza la función de deserialización de Record para leer cada registro del archivo
        for (Record& record : records) {
            record.deserialize(file);
        }
    }

    Record remove_(int i) {
        Record temp = records[i];  // Guardar el elemento a eliminar
        records.erase(records.begin() + i);  // Eliminar el elemento del vector
        return temp;  // Retornar el elemento eliminado
    }


    int size_of() {
        return sizeof(Record);
    }

    void insert(Record &record) {
        for(auto &i : this->records){
            if(i.id == -1){
                i = record;
            }
        }
    }
};

struct IndexEntry {
    string bits; // Representación de cadena de bits de longitud fija
    int local_index;

    IndexEntry(int D) : bits(D, '0'), local_index(0) {} // Inicializa la cadena de bits con longitud D llena de ceros

    int size_of() {
        return (bits.size() + sizeof(int)); // Tamaño de la cadena de bits + tamaño de int
    }

    // Serializa el IndexEntry en un flujo de salida binario
    void serialize(ofstream& file) {
        file.write(reinterpret_cast<const char*>(&local_index), sizeof(int));

        // Escribe la cadena de bits como una secuencia de caracteres
        file.write(bits.c_str(), bits.size());
    }

    // Deserializa el IndexEntry desde un flujo de entrada binario
    void deserialize(ifstream& file, int D) {
        file.read(reinterpret_cast<char*>(&local_index), sizeof(int));

        // Lee solo los primeros D caracteres de la cadena de bits desde el archivo
        char buffer[1024]; // Asumiendo un tamaño máximo de cadena de bits
        file.read(buffer, D);
        buffer[D] = '\0'; // Agrega el carácter nulo para terminar la cadena
        bits.assign(buffer);
    }
    void deserialize(fstream& file, int D) {
        file.read(reinterpret_cast<char*>(&local_index), sizeof(int));

        // Lee solo los primeros D caracteres de la cadena de bits desde el archivo
        char buffer[1024]; // Asumiendo un tamaño máximo de cadena de bits
        file.read(buffer, D);
        buffer[D] = '\0'; // Agrega el carácter nulo para terminar la cadena
        bits.assign(buffer);
    }

};


class DynamicHash {
private:
    unordered_map<string, int> posBuckets; // archivo de datos
    string datafile;
    string indexfile;
    string metadatafile = path + "metadata.bin"; // Nuevo archivo de metadata
    int D, FB;
    int globalDepth;
    vector<IndexEntry> index;
public:
    virtual ~DynamicHash() {
        // Cerrar los archivos de datos, índice y metadata si están abiertos
        ifstream dataFile(path + datafile);
        ifstream indexFile(path + indexfile);
        ifstream metadataFile(metadatafile);

        if (dataFile.is_open()) {
            dataFile.close();
        }
        if (indexFile.is_open()) {
            indexFile.close();
        }
        if (metadataFile.is_open()) {
            metadataFile.close();
        }
    }

    void generarBinarios(int n, string binarioActual, std::vector<string>& resultados) {
        if (n == 0) {
            resultados.push_back(binarioActual);
            return;
        }
        // Agregar '0' y '1' al número binario actual y llamar recursivamente
        generarBinarios(n - 1, binarioActual + '0', resultados);
        generarBinarios(n - 1, binarioActual + '1', resultados);
    }

    string hashing(int key) {
        // Convierte el entero 'key' en una cadena de bits de longitud fija D
        string binaryKey = bitset<32>(key).to_string().substr(32 - D);

        // Aplica el hash utilizando la cadena de bits
        return binaryKey;
    }


    DynamicHash(string datafile, string indexfile, int D, int FB) : datafile(path + datafile),
        indexfile(path + indexfile), D(D),FB(FB) {
        initialize();
    }

    void initialize() {
        ifstream dataFile(datafile, ios::binary);
        ifstream indexFile(indexfile, ios::binary);
        ifstream meta(metadatafile, ios::binary);


        if (!dataFile.is_open() || !indexFile.is_open()) {
            globalDepth = 1; // Inicialmente, solo hay un bit de profundidad global
            // Los archivos no existen, así que crea los archivos y buckets iniciales
            ofstream newDataFile(datafile, ios::binary | ios::out | ios::app);
            ofstream newIndexFile(indexfile, ios::binary | ios::out | ios::app);
            ofstream newmetadataFile(metadatafile, ios::binary | ios::out);
            int freelist = -1;
            newmetadataFile.write(reinterpret_cast<const char*>(&freelist), sizeof(int));
            newmetadataFile.close();
            // Crea y escribe los buckets iniciales en el archivo de datos
            for (int i = 0; i < 2; ++i) {
                Bucket bucket(FB);// Serializa el bucket y escríbelo en el archivo de datos
                bucket.serialize(newDataFile);
            }

            // Cierra los archivos recién creados
            newDataFile.close();
            // Inicializa la estructura de índice con todas las combinaciones posibles de bits
            vector<string> binarios;
            generarBinarios(D, "", binarios);

            for (int i = 0; i < binarios.size(); ++i) {
                IndexEntry entry(D); // Usa D para inicializar la cadena de bits
                entry.bits = hashing(i); // Usa la nueva función de hash

                // Ajustar la posición en consecuencia
                if (i ==0 || i % 2 == 0){
                    entry.local_index = 0;
                    posBuckets[entry.bits] = entry.local_index;
                    index.push_back(entry);
                } else{
                    Bucket temp(FB);
                    entry.local_index = temp.sizeInBits();
                    posBuckets[entry.bits] = entry.local_index;
                    index.push_back(entry);
                }
            }

            // Escribe el número de entradas de índice en el archivo de índice (metadatos)
            int numEntries = binarios.size();
            newIndexFile.write(reinterpret_cast<const char*>(&numEntries), sizeof(int));

            // Escribe la profundidad global en el archivo índice (metadatos)
            newIndexFile.write(reinterpret_cast<const char*>(&globalDepth), sizeof(int));

            // Escribe la estructura de índice en el archivo de índice
            newIndexFile.seekp(sizeof(int)+ sizeof(int));
            for (IndexEntry entry : index) {
                entry.serialize(newIndexFile);
            }

            newIndexFile.close();
        } else {
            // Los archivos existen, así que leemos los datos y el índice

            // Leer el número de entradas de índice en el archivo de índice (metadatos)
            int numEntries;
            indexFile.read(reinterpret_cast<char*>(&numEntries), sizeof(int));

            // Leer la estructura de índice desde el archivo de índice
            index.clear();
            indexFile.seekg(sizeof(int));
            indexFile.read(reinterpret_cast<char*>(&globalDepth), sizeof(int));
            indexFile.seekg(sizeof(int) + sizeof(int));
            for (int i = 0; i < numEntries; ++i) {
                IndexEntry entry(D); // Usa D para inicializar la cadena de bits
                entry.deserialize(indexFile,D);
                index.push_back(entry);
            }
            for (auto i : index){
                posBuckets[i.bits] = i.local_index;
            }

            // Cerrar los archivos después de leer los datos
            dataFile.close();
            indexFile.close();
        }
        dataFile.close();
        indexFile.close();
    }

    vector<Record> inorder() {
        vector<Record> found;
        ifstream indexFile(indexfile, ios::binary);

        if (!indexFile.is_open()) {
            cerr << "No se puede abrir el archivo de índice para verificar los datos." << endl;
                return found;
        }

        // Leer la cantidad de entradas de índice desde el archivo de índice (metadatos)
        int numEntries;
        indexFile.read(reinterpret_cast<char*>(&numEntries), sizeof(int));

        // Leer la profundidad global desde el archivo de índice
        int globalDepth;
        indexFile.read(reinterpret_cast<char*>(&globalDepth), sizeof(int));

        // Verificar las entradas de índice
        vector<IndexEntry> indexEntries;
        for (int i = 0; i < numEntries; ++i) {
            IndexEntry entry(D); // Usa D para inicializar la cadena de bits
            entry.deserialize(indexFile, D);
            indexEntries.push_back(entry);
        }

        // Cerrar el archivo de índice después de leer los datos
        indexFile.close();

        ifstream dataFile(datafile, ios::binary);

        if (!dataFile.is_open()) {
            cerr << "No se puede abrir el archivo de datos para verificar los datos." << endl;
            return found;
        }

        // Leer y verificar los datos en el archivo de datos
        int bucketPos = 0;
        while (true) {
            Bucket bucket(FB);
            bucket.deserialize(dataFile);

            // Verificar si se alcanzó el final del archivo de datos
            if (dataFile.eof()) {
                break;
            }

            if (bucket.size != 0) {
                for (const Record& record : bucket.records) {
                    if(record.id != -1) found.push_back(record);
                }
            }

            bucketPos++;
        }

        // Cerrar el archivo de datos después de leer los datos
        dataFile.close();

        ifstream meta(metadatafile, ios::binary);

        if (!meta.is_open()) {
            cerr << "No se puede abrir el archivo de metadatos para verificar los datos." << endl;
            return found;
        }

        // Leer e imprimir los enteros desde el archivo de metadatos
        int metaValue;
        while (meta.read(reinterpret_cast<char*>(&metaValue), sizeof(int))) {
            cout << "Valor en el archivo de metadatos: " << metaValue << endl;
        }

        // Cerrar el archivo de metadatos después de leer los datos
        meta.close();

        return found;
    }

    bool add(const Record& record) {
        // Obtener la cadena de bits hash para la clave del registro
        string hashBits = hashing(record.id);

        // Buscar la entrada de índice correspondiente
        if (posBuckets.find(hashBits) != posBuckets.end()) {
            int bucketPos = posBuckets[hashBits];

            // Abrir el archivo de datos en modo lectura/escritura
            fstream dataFileStream(datafile, ios::binary | ios::in | ios::out);

            // Ir a la posición del bucket en el archivo de datos
            dataFileStream.seekg(bucketPos);

            // Leer el bucket existente desde el archivo
            Bucket existingBucket(FB);
            existingBucket.deserialize(dataFileStream);

            // Comprobar si el bucket tiene espacio para más registros
            if (existingBucket.size < existingBucket.capacidad) {
                // Insertar el nuevo registro en el bucket
                existingBucket.records[existingBucket.size] = record;
                existingBucket.size++;

                // Actualizar el bucket en el archivo de datos
                dataFileStream.seekp(bucketPos);
                existingBucket.serialize(dataFileStream);

                // Cerrar el archivo de datos
                dataFileStream.close();

                // La inserción fue exitosa
                return true;
            }else {
                // El bucket está lleno, se requiere dividir

                // Aumentar la globalDepth
                globalDepth++;

                // Buscar las entradas de índice que apuntan al bucket lleno
                vector<IndexEntry *> entriesToUpdate;
                for (auto &entry: index) {
                    if (entry.local_index == bucketPos) {
                        entriesToUpdate.push_back(&entry);
                    }
                }

                // Crear los nuevos buckets
                Bucket newBucket1(FB), newBucket2(FB);
                for (int i = 0; i < existingBucket.size; ++i) {
                    // Poner la mitad de los datos en el nuevo bucket
                    if (i % 2 == 0) {
                        newBucket1.insert(existingBucket.records[i]);
                    } else {
                        newBucket2.insert(existingBucket.records[i]);
                    }
                }

                // Guardar el nuevo bucket en el archivo de datos
                int newBucketPos;
                fstream meta(metadatafile, ios::binary | ios::in | ios::out);
                int metadataPos;
                meta.seekp(-sizeof(int), std::ios::end); // Retrocede 1 int desde el final
                meta.read(reinterpret_cast<char *>(newBucketPos), sizeof(int));
                if (newBucketPos != -1) {
                    // Guardar el nuevo bucket en la posición calculada y el original en la posicion anterior
                    dataFileStream.seekp(bucketPos);
                    newBucket1.serialize(dataFileStream);
                    dataFileStream.seekp(newBucketPos);
                    newBucket2.serialize(dataFileStream);
                } else {
                    dataFileStream.seekp(0, std::ios::end);
                    dataFileStream.seekp(bucketPos);
                    newBucket1.serialize(dataFileStream);
                    dataFileStream.seekp(newBucketPos);
                    newBucket2.serialize(dataFileStream);
                }

                // Actualizar las entradas de índice para apuntar al nuevo bucket
                for (auto &entry: entriesToUpdate) {
                    entry->local_index = newBucketPos;
                }

                // Actualizar el map PosBuckets
                for (auto i: index) {
                    posBuckets[i.bits] = i.local_index;
                }

                // Actualizar el archivo de índice
                ofstream newIndexFile(indexfile, ios::binary | ios::out | ios::app);
                newIndexFile.seekp(sizeof(int) + sizeof(int));
                for (IndexEntry entry: index) {
                    entry.serialize(newIndexFile);
                }

                // Cerrar el archivo de datos
                dataFileStream.close();

                // Intentar insertar el registro nuevamente en los nuevos buckets
                return add(record);
            }
        } else{
            cout<<"posicion invalida"<<endl;
            return false;
        }

    }


    Record search(int key) {
        // Obtener la cadena de bits hash para la clave
        string hashBits = hashing(key);

        // Buscar la entrada de índice correspondiente
        if (posBuckets.find(hashBits) != posBuckets.end()) {
            int bucketPos = posBuckets[hashBits];

            // Abrir el archivo de datos en modo lectura
            ifstream dataFileStream(datafile, ios::binary);

            // Ir a la posición del bucket en el archivo de datos
            dataFileStream.seekg(bucketPos);

            // Leer el bucket desde el archivo
            Bucket existingBucket(FB);
            existingBucket.deserialize(dataFileStream);

            // Buscar el registro en el bucket
            for (const Record& record : existingBucket.records) {
                if (record.id == key) {
                    // Cerrar el archivo de datos
                    dataFileStream.close();

                    // Devolver el registro encontrado
                    return record;
                }
            }

            // Cerrar el archivo de datos
            dataFileStream.close();
        }

        // Si no se encuentra el registro, devuelve un registro con clave -1
        Record notFoundRecord;
        notFoundRecord.id = -1;
        return notFoundRecord;
    }

    Record remove(int key) {
        // Obtener la cadena de bits hash para la clave
        string hashBits = hashing(key);

        // Buscar la entrada de índice correspondiente
        if (posBuckets.find(hashBits) != posBuckets.end()) {
            int bucketPos = posBuckets[hashBits];

            // Abrir el archivo de datos en modo lectura/escritura
            fstream dataFileStream(datafile, ios::binary | ios::in | ios::out);

            // Ir a la posición del bucket en el archivo de datos
            dataFileStream.seekg(bucketPos);

            // Leer el bucket desde el archivo
            Bucket existingBucket(FB);
            existingBucket.deserialize(dataFileStream);

            // Buscar el registro en el bucket
            for (int i = 0; i < existingBucket.size; ++i) {
                if (existingBucket.records[i].id == key) {
                    // Marcar la clave como -1 y los datos como nulos
                    existingBucket.records[i].id = -1;
                    memset(existingBucket.records[i].district, 0, sizeof(existingBucket.records[i].district));
                    existingBucket.records[i].year = -1;
                    existingBucket.records[i].month = -1;
                    existingBucket.records[i].day = -1;
                    existingBucket.records[i].vol = -1;
                    memset(existingBucket.records[i].street, 0, sizeof(existingBucket.records[i].street));
                    existingBucket.size--;

                    // Actualizar el bucket en el archivo de datos
                    dataFileStream.seekp(bucketPos);
                    existingBucket.serialize(dataFileStream);

                    // Cerrar el archivo de datos
                    dataFileStream.close();

                    // Verificar si el bucket está vacío
                    if (existingBucket.size == 0) {
                        Bucket temp(FB);
                        int temp_ = temp.sizeInBits();
                        // Comprobar que no sean buckets raíz
                        if (bucketPos != 0 && bucketPos != temp_) {
                            // Agregar la posición del bucket eliminado a la freelist en el archivo de metadatos
                            fstream meta(metadatafile, ios::binary | ios::in | ios::out);
                            meta.seekp(0, std::ios::end);
                            meta.write(reinterpret_cast<char*>(&bucketPos), sizeof(int));
                            meta.close();

                            // Buscar otras entradas de índice que apunten al bucket eliminado
                            for (auto& entry : index) {
                                if (entry.local_index == bucketPos) {
                                    // Buscar el sufijo de la entrada de índice
                                    string suffix = entry.bits.substr(globalDepth - D);

                                    // Buscar una entrada de índice que tenga el mismo sufijo
                                    for (auto& otherEntry : index) {
                                        if (otherEntry.local_index != bucketPos &&
                                            otherEntry.bits.substr(globalDepth - D) == suffix) {
                                            // Redirigir la entrada de índice al bucket anterior
                                            entry.local_index = otherEntry.local_index;
                                            break;
                                        }
                                    }
                                }
                            }

                            // Actualizar el map PosBuckets
                            for (auto i : index){
                                posBuckets[i.bits] = i.local_index;
                            }

                            // Actualizar el archivo de índice
                            ofstream newIndexFile(indexfile, ios::binary | ios::out | ios::app);
                            newIndexFile.seekp(sizeof(int) + sizeof(int));
                            for (IndexEntry entry : index) {
                                entry.serialize(newIndexFile);
                            }
                        } else {
                            cout << "No se actualizó la freelist ya que es un bucket raíz" << endl;
                        }
                    }

                    return existingBucket.records[i];
                }
            }

            // Cerrar el archivo de datos
            dataFileStream.close();
        }
        // Si no se encuentra el registro, devuelve un registro con clave -1
        Record notFoundRecord;
        notFoundRecord.id = -1;
        return notFoundRecord;
    }

};


#endif // EXTENDIBLEHASH_H
