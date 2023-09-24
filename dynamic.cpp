#include <string>
#include <iostream>
#include <vector>
#include <ostream>
#include <fstream>
#include <cstring>
#include <cmath> // Para log2
#include <unordered_map>

using namespace std;

struct Record {
    int key;
    char data[10];

    // Función de serialización para Record
    void serialize(ofstream& file) const {
        file.write(reinterpret_cast<const char*>(&key), sizeof(int));
        file.write(data, sizeof(data));
    }

    // Función de deserialización para Record
    void deserialize(ifstream& file) {
        file.read(reinterpret_cast<char*>(&key), sizeof(int));
        file.read(data, sizeof(data));
    }
};


struct Bucket {
    vector<Record> records;
    int capacidad;
    int size;
    int borrado;

    Bucket(){

    }

    Bucket(int fb) : capacidad(fb), size(0), borrado(0) {
        records = vector<Record>(fb);
        for (int i = 0; i < fb; ++i) {
            records[i].key = -1; // Inicializa todos los registros con -1
            memset(records[i].data, 0, sizeof(records[i].data)); // Inicializa los datos con 0
        }
    }
    Bucket(const Bucket& other) : capacidad(other.capacidad), size(other.size), borrado(other.borrado), records(other.records) {
    }

// Operador de asignación de copia
    Bucket& operator=(const Bucket& other) {
        if (this != &other) {
            capacidad = other.capacidad;
            size = other.size;
            borrado = other.borrado;
            records = other.records;
        }
        return *this;
    }
    // Serializa el Bucket en un flujo de salida binario
    void serialize(ofstream& file) {
        file.write(reinterpret_cast<const char*>(&capacidad), sizeof(int));
        file.write(reinterpret_cast<const char*>(&size), sizeof(int));
        file.write(reinterpret_cast<const char*>(&borrado), sizeof(int));

        // Utiliza la función de serialización de Record para escribir cada registro en el vector
        for (const Record& record : records) {
            record.serialize(file);
        }
    }

    // Deserializa el Bucket desde un flujo de entrada binario
    void deserialize(ifstream& file) {
        file.read(reinterpret_cast<char*>(&capacidad), sizeof(int));
        file.read(reinterpret_cast<char*>(&size), sizeof(int));
        file.read(reinterpret_cast<char*>(&borrado), sizeof(int));

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
};

struct IndexEntry {
    int bits;
    int local_index;

    int size_of(){
        return (sizeof(int) + sizeof(int));
    }

    // Serializa el IndexEntry en un flujo de salida binario
    void serialize(ofstream& file) {
        file.write(reinterpret_cast<const char*>(&bits), sizeof(int));
        file.write(reinterpret_cast<const char*>(&local_index), sizeof(int));
    }

    // Deserializa el IndexEntry desde un flujo de entrada binario
    void deserialize(ifstream& file) {
        file.read(reinterpret_cast<char*>(&bits), sizeof(int));
        file.read(reinterpret_cast<char*>(&local_index), sizeof(int));
    }
};

class DynamicHash {
private:
    unordered_map<int ,int> posBuckets; // archivo de datos
    string datafile;
    string indexfile;
    int D, FB; //D = para construir el index, fb es  el factor de bloque
    int globalDepth; // Profundidad global
    vector<IndexEntry> index; // Agregar un vector de entradas de índice
public:
    virtual ~DynamicHash() {
        // Liberar la memoria utilizada por el vector 'index'
        index.clear(); // Esto liberará automáticamente la memoria de las entradas del índice

        // Cerrar los archivos de datos e índice si están abiertos
        // Asegúrate de que los archivos estén cerrados antes de que el objeto se destruya
        ifstream dataFile(datafile);
        ifstream indexFile(indexfile);
        if (dataFile.is_open()) {
            dataFile.close();
        }
        if (indexFile.is_open()) {
            indexFile.close();
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

    int hashing(int key){
        return (key % D);
    }

    DynamicHash(string datafile, string indexfile, int D, int FB) : datafile(datafile),
    indexfile(indexfile), D(D),FB(FB) {
        initialize();
    }
    void verifyData(const string& datafile, const string& indexfile, int FB) {
        ifstream dataFile(datafile, ios::binary);
        ifstream indexFile(indexfile, ios::binary);

        if (!dataFile.is_open() || !indexFile.is_open()) {
            cerr << "No se pueden abrir los archivos para verificar los datos." << endl;
            return;
        }

        // Leer la cantidad de entradas de índice desde el archivo de índice
        int numEntries;
        indexFile.read(reinterpret_cast<char*>(&numEntries), sizeof(int));

        // Leer la profundidad global desde el archivo de índice
        int globalDepth;
        indexFile.read(reinterpret_cast<char*>(&globalDepth), sizeof(int));

        cout << "Profundidad global: " << globalDepth << endl;

        // Verificar las entradas de índice
        vector<IndexEntry> indexEntries;
        for (int i = 0; i < numEntries; ++i) {
            IndexEntry entry;
            entry.deserialize(indexFile);
            indexEntries.push_back(entry);
        }

        for (const IndexEntry& entry : indexEntries) {
            cout << "Posicion: " << entry.local_index << " Bits: " << entry.bits << endl;
        }

        // Leer y verificar los datos en el archivo de datos
        int bucketPos = 0;
        dataFile.seekg(sizeof(int));
        while (true) {
            Bucket bucket(FB);
            bucket.deserialize(dataFile);

            // Verificar si se alcanzó el final del archivo
            if (dataFile.eof()) {
                break;
            }

            cout << "Bucket #" << bucketPos << " Capacidad: " << bucket.capacidad << " Size: " << bucket.size << " Borrado: " << bucket.borrado << endl;

            if (bucket.size == 0) {
                cout << "  Bucket vacio" << endl;
            } else {
                for (const Record& record : bucket.records) {
                    cout << "  Key: " << record.key << " Data: " << record.data << endl;
                }
            }

            bucketPos++;
        }

        dataFile.close();
        indexFile.close();
    }



    void initialize() {
        ifstream dataFile(datafile, ios::binary);
        ifstream indexFile(indexfile, ios::binary);

        if (!dataFile.is_open() || !indexFile.is_open()) {
            globalDepth = 1; // Inicialmente, solo hay un bit de profundidad global
            // Los archivos no existen, así que crea los archivos y buckets iniciales
            ofstream newDataFile(datafile, ios::binary | ios::out | ios::app);
            ofstream newIndexFile(indexfile, ios::binary | ios::out | ios::app);

            // Inicializa la estructura de índice con todas las combinaciones posibles de bits
            vector<string> binarios;
            generarBinarios(D, "", binarios);

            for (int i = 0; i < binarios.size(); ++i) {
                IndexEntry entry;
                entry.bits = stoi(binarios[i], nullptr, 2);

                // Calcular la profundidad de bits requerida en función de la profundidad global
                int requiredBits = globalDepth;
                if (requiredBits > D) {
                    requiredBits = D;
                }
                //guardar el posbuckets

                // Ajustar la posición en consecuencia
                entry.local_index = (i % 2) * sizeof(Bucket) + sizeof(int);
                posBuckets[i] = entry.local_index;

                index.push_back(entry);
            }

            // Escribe el número de entradas de índice en el archivo de índice (metadatos)
            int numEntries = binarios.size();
            newIndexFile.write(reinterpret_cast<const char*>(&numEntries), sizeof(int));

            // Escribe la profundidad global en el archivo índice (metadatos)
            newIndexFile.write(reinterpret_cast<const char*>(&globalDepth), sizeof(int));

            // Escribe la estructura de índice en el archivo de índice
            for (IndexEntry entry : index) {
                newIndexFile.write(reinterpret_cast<const char*>(&entry), entry.size_of());
            }
            //Escribe la metadata para la freelist
            int freelist = -1;
            newDataFile.write(reinterpret_cast<char*>(&freelist), sizeof(int)); // Pasa un puntero al valor y usa sizeof para especificar la cantidad de bytes a escribir
            newDataFile.seekp(sizeof(int)); // Mueve el puntero de escritura al siguiente lugar si es necesario
            // Crea y escribe los buckets iniciales en el archivo de datos
            for (int i = 0; i < 2; ++i) {
                Bucket bucket(FB);
                bucket.borrado = 0; // Marcar el bucket como no utilizado
                // Serializa el bucket y escríbelo en el archivo de datos
                bucket.serialize(newDataFile);
            }

            // Cierra los archivos recién creados
            newDataFile.close();
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
                IndexEntry entry;
                indexFile.read(reinterpret_cast<char*>(&entry), sizeof(IndexEntry));
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

    bool insert(Record record) {
        // Hacer el hash del dato con la key
        int hash = hashing(record.key);

        // Obtener la posición del bucket correspondiente al hash
        int localIndex = posBuckets[hash];

        // Leer el bucket correspondiente desde el archivo de datos
        ifstream dataFile(datafile, ios::binary);
        dataFile.seekg(localIndex);  // Mover el puntero al inicio del bucket
        Bucket bucket(FB);
        bucket.deserialize(dataFile);
        dataFile.close();

        // Verificar si el bucket está lleno o no
        if (bucket.size < bucket.capacidad) {
            // Buscar la primera posición vacía en el vector 'records'
            int emptyPosition = -1;
            for (int i = 0; i < bucket.capacidad; ++i) {
                if (bucket.records[i].key == -1) {  //valor de los vacios
                    emptyPosition = i;
                    break;
                }
            }

            if (emptyPosition != -1) {
                bucket.records[emptyPosition] = record;
                bucket.size++;

                // Guarda el bucket actualizado en el archivo de datos
                ofstream newDataFile(datafile, ios::binary | ios::out | ios::in);
                newDataFile.seekp(localIndex);  // Mover el puntero al inicio del bucket
                bucket.serialize(newDataFile);

                // La inserción fue exitosa
                newDataFile.close();
                return true;
            }
        } else {
            // Si el bucket está lleno, aumentamos la profundidad global y creamos nuevos buckets
            globalDepth++;

            // Buscar la entrada de índice correspondiente al hash
            IndexEntry* indexEntry = nullptr;
            for (IndexEntry& entry : index) {
                if (entry.bits == hash) {
                    indexEntry = &entry;
                    break;
                }
            }

            if (indexEntry == nullptr) {
                cerr << "No se pudo encontrar la entrada de índice correspondiente al hash." << endl;
                return false;
            }

            // Calcular cuántos índices apuntan al bucket lleno
            int numIndicesApuntan = 0;
            for (const auto& entry : index) {
                if (entry.local_index == localIndex) {
                    numIndicesApuntan++;
                }
            }

            // Crear nuevos buckets para la división
            vector<Bucket> newBuckets(numIndicesApuntan);
            vector<int> newLocalIndexes(numIndicesApuntan);

            // Dividir los registros del bucket lleno entre los nuevos buckets
            for (int i = 0; i < bucket.size; ++i) {
                int newHash = hashing(bucket.records[i].key);  // Calcular nuevo hash
                int newIndex = newHash % (1 << globalDepth);  // Calcular nuevo índice

                // Asignar el registro al nuevo bucket correspondiente
                newBuckets[newIndex].records.push_back(bucket.records[i]);
                newBuckets[newIndex].size++;
            }

            // Destruir el bucket original asignando capacidad 0
            bucket.capacidad = 0;
            bucket.size = 0;

            // Actualizar la freelist
            manageFreeList(localIndex);

            // Guardar el bucket de división en la posición de free list o al final del archivo de datos
            ofstream newDataFile(datafile, ios::binary | ios::out | ios::in);
            newDataFile.seekp(localIndex);  // Mover el puntero al inicio del bucket
            newBuckets[0].serialize(newDataFile);

            // Escribir los otros buckets de división al final del archivo de datos si no se usó la freelist
            ifstream dataFile(datafile, ios::binary);
            int freelistPointer;
            dataFile.read(reinterpret_cast<char*>(freelistPointer), sizeof(int));
            if (freelistPointer == -1) {
                for (int i = 1; i < numIndicesApuntan; ++i) {
                    newBuckets[i].serialize(newDataFile);
                }
            }

            // Actualizar el índice para que apunte a los nuevos buckets
            for (int i = 0; i < numIndicesApuntan; ++i) {
                IndexEntry newEntry;
                // Actualizar bits en función de la nueva profundidad global
                newEntry.bits = (hash << (32 - globalDepth)) | i;
                // Posición del nuevo bucket en el archivo
                newEntry.local_index = localIndex + i * sizeof(Bucket);
                index.push_back(newEntry);
                posBuckets[newEntry.bits] = newEntry.local_index;
            }

            // Actualizar la posición del bucket en posBuckets
            posBuckets[hash] = localIndex;

            // Actualizar el índice
            updateIndex(hash, localIndex);

            // La inserción fue exitosa
            dataFile.close();
            return true;
        }

        // Si llegamos aquí, la inserción falló, por lo que retornamos false
        dataFile.close();
        return false;
    }

    Record remove(int key) {
        // Hacer el hash de la clave para encontrar el bucket
        int hash = hashing(key);

        // Obtener la entrada de índice correspondiente al hash
        int localIndex = posBuckets[hash];

        // Leer el bucket correspondiente desde el archivo de datos
        ifstream dataFile(datafile, ios::binary);
        dataFile.seekg(localIndex + sizeof(int));  // Mover el puntero al inicio del bucket
        Bucket bucket(FB);
        dataFile.read(reinterpret_cast<char*>(&bucket), bucket.size_of());
        dataFile.close();

        // Buscar el registro en el bucket
        for (int i = 0; i < bucket.size; ++i) {
            if (bucket.records[i].key == key) {
                // Se encontró el registro, eliminarlo
                Record removedRecord = bucket.remove_(i);
                // Reducir el tamaño del bucket
                bucket.size--;

                // Si el bucket está vacío, manejar la lógica de la freelist
                if (bucket.size == 0) {
                    manageFreeList(localIndex);
                }

                // Guardar el bucket actualizado en el archivo de datos
                ofstream newDataFile(datafile, ios::binary | ios::in | ios::out);
                newDataFile.seekp(localIndex + sizeof(int));  // Mover el puntero al inicio del bucket
                newDataFile.write(reinterpret_cast<const char*>(&bucket), bucket.size_of());
                newDataFile.close();
                return removedRecord;
            }
        }
        // Si no se encuentra el registro, devolver un registro vacío o un indicador de que no se encontró
        Record emptyRecord;
        emptyRecord.key = -1;  // Puedes usar un valor especial para indicar que no se encontró el registro
        return emptyRecord;
    }

    Record search(int key) {
        // Hacer el hash de la clave para encontrar el bucket
        int hash = hashing(key);

        // Obtener la entrada de índice correspondiente al hash
        int localIndex = posBuckets[hash];

        // Leer el bucket correspondiente desde el archivo de datos
        ifstream dataFile(datafile, ios::binary);
        dataFile.seekg(localIndex);  // Mover el puntero al inicio del bucket
        Bucket bucket(FB);
        dataFile.read(reinterpret_cast<char*>(&bucket), bucket.size_of());
        dataFile.close();

        // Buscar el registro en el bucket
        for (int i = 0; i < bucket.size; ++i) {
            if (bucket.records[i].key == key) {
                // Se encontró el registro, devolverlo
                return bucket.records[i];
            }
        }

        // Si no se encuentra el registro, devolver un registro vacío o un indicador de que no se encontró
        Record emptyRecord;
        emptyRecord.key = -1;  // Puedes usar un valor especial para indicar que no se encontró el registro
        return emptyRecord;
    }

    void manageFreeList(int localIndex) {
        // Leer el puntero al siguiente bucket libre desde la metadata
        int freelistPointer;
        ifstream metadataFile(datafile, ios::binary);
        metadataFile.read(reinterpret_cast<char*>(&freelistPointer), sizeof(int));
        metadataFile.close();

        // Si la freelist está vacía, establecer el puntero al bucket actual
        if (freelistPointer == -1) {
            freelistPointer = localIndex;
        } else {
            // Enlazar el bucket actual con el último bucket en la freelist
            fstream metadataFile(datafile, ios::binary | ios::in | ios::out);
            while (true) {
                Bucket lastBucket(FB);
                metadataFile.seekg(freelistPointer);
                metadataFile.read(reinterpret_cast<char*>(&lastBucket), lastBucket.size_of());
                if (lastBucket.borrado == -1) {
                    // Este es el último bucket en la freelist, actualiza el puntero
                    lastBucket.borrado = localIndex;
                    metadataFile.seekp(freelistPointer);
                    metadataFile.write(reinterpret_cast<const char*>(&lastBucket), lastBucket.size_of());
                    metadataFile.close();
                    break;
                } else {
                    // Avanzar al siguiente bucket en la freelist
                    freelistPointer = lastBucket.borrado;
                }
            }
        }
    }
    void updateIndex(int oldHash, int localIndex) {
        // Verificar si la profundidad global ha cambiado
        if (globalDepth > (1 << index[oldHash].bits)) {
            // La profundidad global ha aumentado, duplicar las entradas de índice
            int numEntries = index.size();
            for (int i = 0; i < numEntries; ++i) {
                if (index[i].bits == oldHash) {
                    // Duplicar la entrada de índice con el nuevo bit
                    IndexEntry newEntry;
                    newEntry.bits = index[i].bits ^ (1 << (32 - globalDepth));
                    newEntry.local_index = localIndex + (i % 2) * sizeof(Bucket);

                    // Actualizar la posición del bucket en posBuckets
                    posBuckets[newEntry.bits] = newEntry.local_index;

                    // Agregar la nueva entrada de índice al final del vector
                    index.push_back(newEntry);
                }
            }
        } else {
            // La profundidad global no ha cambiado, simplemente actualizar la posición del bucket en posBuckets
            posBuckets[oldHash] = localIndex;
        }
    }



};