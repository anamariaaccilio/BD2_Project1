# Organización de Archivos

## Grupo 5
* Ana Maria Accilio Villanueva
* Diego Sebastián Pacheco Ferrel
* Juan Pedro Vásquez Vílchez
* Luis Enrique Cortijo Gonzales
* Marcelo Mario Zuloeta Salazar

# Introducción
## Objetivo del proyecto
La organización física de archivos en memoria secundaria es la base para la creación y utilización de los diferentes sistemas de gestión de bases de datos (DBMS). En un entorno cada vez más orientado hacia la información, la estructuración eficiente de los archivos en memoria secundaria es esencial para garantizar la integridad, disponibilidad y rendimiento de los datos almacenados. Nuestro proyecto esta orientado a aplicar diferentes técnicas de organización de archivos como son las de **Sequential File Organization**, **AVL File Organization** y **Extendible Hashing** para la gestion de registros en memoria secundaria y la consulta, inserción y eliminación eficiente de la información de los registros a través de una GUI para hacer consultas con sentencias SQL.

## Dominio de datos
- NYC Traffic Volume
- Twitter Bitcoin

## Resultados esperados
Se busca obtener una optimización significativa de los tiempos de inserción y consulta de registros en nuestro sistema. Con Sequential File, anticipamos una aceleración en la inserción ordenada de registros, lo que mejorará la eficiencia en la consulta secuencial y búsqueda binaria. Por otro lado, la estructura de árbol AVL permitirá búsquedas en tiempo logarítmico en base a la cantidad de registros en el archivo, beneficiando las consultas por llave primaria. Finalmente, el uso de extendible hashing agilizará la inserción y consulta de registros distribuyendo eficientemente los datos en bloques de almacenamiento en base a una funcion hash. En conjunto, esperamos reducir significativamente los tiempos de respuesta para diferentes tipos de consultas en base a las diferentes técnicas de indexación usadas.

# Técnicas utilizadas

## Sequential File Organization
La técnica de organización de archivos llamada Sequential File es una forma de organizar y almacenar datos en un archivo, donde los registros se almacenan en secuencia, uno tras otro. Es como una lista de elementos, donde cada elemento tiene un número de posición. En este tipo de archivo, los registros están en un orden específico y se pueden acceder uno tras otro, desde el principio hasta el final, sin saltos.

![SequentialFile]/sequential1.png

![SequentialFile]/sequential2.png

En este proyecto, utilizamos esta técnica para guardar datos de manera simple y eficiente. Es útil cuando no se necesita un acceso aleatorio a los datos y cuando la prioridad es la simplicidad y la eficiencia en la lectura y escritura de registros en orden.

- *Métodos importantes*
  1. add(Record record): Agrega un nuevo registro al archivo de datos siguiendo una lógica específica. Dependiendo de ciertas condiciones, los registros se pueden agregar al archivo auxiliar o al archivo de datos principal.
     ```cpp
      void add(Record record){
        //Ubicamos el elemento anterior a record
        int X_pos = binarySearchPosition(record) - 1; //binarySearchPOS me devuelve la pos donde debo insertar el elemento, por eso el -1
        Record X = readRecord(X_pos,this->data_file);

        //Si X apunta a un elemento de la misma Data
        /*
            add(Manuel)
            ======================
            Data:
            "     " -> 0.1
            Asucena -> 0.2  <- X
            Ortega  -> 0
            ======================
            Pedro   -> 0.3
        */
        if(analyzeFloat(X.Puntero)=="Data"){
            //Agregamos el record al aux y hacemos el cambiaso de punteros entre X y record
            record.Puntero = X.Puntero; //record -> X.Puntero
            writeRecordEND(record, this->aux_file);
            updatePunteroAtPosition(X_pos, aux_size, this->data_file); // X -> pos(record)

            //No olvidarse de aumentar la cantidad de elementos que hay en el auxiliar.bin
            aux_size += 1;
        }

        //Si X apunta a un elemento perteneciente a Auxiliar
        /*
            add(Manuel)
            ======================
            Data:
            "     " -> 0.1
            Asucena -> 0   <- X
            Ortega  -> 0.3
            ======================
            Auxiliar:
            Belen   ->  0.2 <- Y
        */
        if(analyzeFloat(X.Puntero)=="Auxiliar"){
            //Si apunta a un auxiliar primero me tengo que ubicar en auxiliarfile[X.Puntero]

            //Y_pos = X.Puntero;
            Record Y = readRecord(X.Puntero,this->aux_file);

            //Caso particular (Todavia no nos hemos adentrado al 100% en aux_file, estamos en el limbo)
            /*
                add(Baldor)
                ======================
                Data:
                "     " -> 0.1
                Asucena -> 0   <- X      (X todavia esta en data file, mientras que Y ya esta exclusivamente en el auxiliar file)
                Ortega  -> 0.3
                ======================
                Auxiliar:
                Belen   ->  0.2 <- Y    (Baldor va antes que que Belen)
             */
            int cmp = record.compare(Y);
            if (cmp <= 0){ //Si record va antes que Y o es igual a Y
                //Agregamos el record al aux y hacemos el cambiaso de punteros entre X y record
                record.Puntero = X.Puntero; //record -> X.Puntero
                writeRecordEND(record, this->aux_file);
                updatePunteroAtPosition(X_pos, aux_size, this->data_file); // X -> pos(record)

                //No olvidarse de aumentar la cantidad de elementos que hay en el auxiliar.bin
                aux_size += 1;
            }

            //Si record va despues que Y
            if (cmp > 0){

                //Caso particular (Todavia no nos hemos adentrado al 100% en aux_file, estamos en el limbo)
                /*
                    add(Boris)
                    ======================
                    Data:
                    "     " -> 0.1
                    Asucena -> 0   <- X      (X todavia esta en data file, mientras que Y ya esta exclusivamente en el auxiliar file)
                    Ortega  -> 0.3
                    ======================
                    Auxiliar:
                    Belen   ->  0.2 <- Y    (Boris va despues que que Belen, pero despues de Belen no hay nada mas)
                 */
                if (analyzeFloat(Y.Puntero) == "Data"){
                    record.Puntero = Y.Puntero;
                    writeRecordEND(record, this->aux_file);
                    updatePunteroAtPosition(X.Puntero, aux_size, this->aux_file); // X -> pos(record)
                    //No olvidarse de aumentar la cantidad de elementos que hay en el auxiliar.bin
                    aux_size += 1;
                }

                //Ahora si tanto X como Y pertenecen a auxiliar file
                else {
                    bool posicionado = false;
                    int cmp2;
                    while(!posicionado){

                        X_pos = X.Puntero; //X_pos = Y_pos
                        X = readRecord(X_pos,this->aux_file);

                        //Y_pos = X.Puntero
                        Y = readRecord(X.Puntero,this->aux_file);

                        /*
                            add(Conchita)
                            ======================
                            Data:
                            "     " -> 0.1
                            Asucena -> 0   <- X
                            Ortega  -> 0.3
                            ======================
                            Auxiliar:
                            Belen   ->  1   <- Y
                            Boris   ->  0.2

                            >>>>>>>>>>>>>>>>>>>>>>

                            add(Conchita)
                            ======================
                            Data:
                            "     " -> 0.1
                            Asucena -> 0
                            Ortega  -> 0.3
                            ======================
                            Auxiliar:
                            Belen   ->  1   <- X
                            Boris   ->  0.2 <- Y

                         */
                        cmp2 = record.compare(Y);

                        //Si record va antes que Y
                        if (cmp2 <= 0){
                            //Agregamos el record al aux y hacemos el cambiaso de punteros entre X y record
                            record.Puntero = X.Puntero; //record -> X.Puntero
                            writeRecordEND(record, this->aux_file);
                            updatePunteroAtPosition(X_pos, aux_size, this->aux_file); // X -> pos(record)

                            //No olvidarse de aumentar la cantidad de elementos que hay en el auxiliar.bin
                            aux_size += 1;

                            posicionado = true;
                        }

                        //Si record va despues que Y, y este Y apunta a un elemento perteneciente a data.file (osea que ya no apunta a ningun otro elemento)
                        if (cmp2 > 0 && analyzeFloat(Y.Puntero)=="Data"){

                            record.Puntero = Y.Puntero;
                            writeRecordEND(record, this->aux_file);
                            updatePunteroAtPosition(X.Puntero/*Y_pos*/, aux_size, this->aux_file);

                            //No olvidarse de aumentar la cantidad de elementos que hay en el auxiliar.bin
                            aux_size += 1;

                            posicionado = true;
                        }

                        /*

                         En caso record vaya despues que Y pero Y apunta a otro elemento perteneciente a auxixiliar.bin
                         (osea no se ha llegao al final)

                         Se repite el while y con ello
                         -  X pasa a ser Y(1)
                         -  Y pasa a ser el Y(1).Puntero
                         */
                    }
                }
            }
        }

        /*
        Hacemos rebuild en caso que auxiliar.size() supere el log2(data.size())
        */

        if (aux_size > log2(data_size)){
            rebuild();
        }

    }
      ```
  2. remove_(const string& key): Elimina un registro del archivo de datos. Realiza una búsqueda binaria para encontrar la posición del registro y marca su puntero como -1 para indicar que está eliminado. Luego, realiza una reconstrucción del archivo de datos para eliminar los registros marcados como eliminados.
     ```cpp
     bool remove_(const string& key){

        //DATA FILE
        rebuild();

        bool modified = false;

        //Busco con busqueda binaria la posicion del elemento
        Record encontrar;
        strcpy(encontrar.Nombre, key.c_str());

        // Busco con búsqueda binaria el key_pos
        int key_pos = binarySearchPosition(encontrar);
        if (key_pos == 0){
            throw runtime_error("No se encontró la key");
        }
        else{
            key_pos -= 1; //Pos real del key encontrado
            Record current = readRecord(key_pos, data_file);
            if (current.Nombre != key){
                throw runtime_error("No se encontró la key");
            }

            //Actualizamos el primer elemento encontrado con binary search
            updatePunteroAtPosition(key_pos, -1, this->data_file);

            // Recorrer los elementos antes del key_pos para ver si se repiten y agregarlos
            for (int i = key_pos - 1; i >= 0; i--) {
                current = readRecord(i, data_file);
                Record prev = readRecord(i + 1, data_file);
                if (current.compare(prev) == 0) {
                    if (getPunteroAtPosition(i, data_file) != -1) {
                        updatePunteroAtPosition(i, -1, this->data_file);
                    }
                } else {
                    break; // Si ya no se repiten, detener la búsqueda
                }
            }

            // Recorrer los elementos después del key_pos para ver si se repiten y agregarlos
            for (int i = key_pos + 1; i < size(data_file); i++) {
                current = readRecord(i, data_file);
                Record next = readRecord(i - 1, data_file);
                if (current.compare(next) == 0) {
                    if (getPunteroAtPosition(i, data_file) != -1) {
                        updatePunteroAtPosition(i, -1, this->data_file);
                    }
                } else {
                    break; // Si ya no se repiten, detener la búsqueda
                }
            }
        }

        return modified;

        /*
         *Para el eliminar
         - Se hace un rebuild ()
         - Solo buscar el elemento por nombre, y cambiar su puntero a -1

         - Tomar en cuenta que se tiene que modificar el rebuild para que si lee un -1 en un registro_datafile,
           que no lo agregue al nuevo datafile
         */
    };

     ```
  3. search(const string& key): Busca registros por su nombre en el archivo de datos y el archivo auxiliar y devuelve una lista de registros que coinciden con el nombre proporcionado.
     ```cpp
      vector<Record> search(const string& key){

        //DATAFILE

        //Busco con busqueda binaria la posicion del elemento
        Record encontrar;
        strcpy(encontrar.Nombre, key.c_str());

        // Busco con búsqueda binaria el key_pos
        int key_pos = binarySearchPosition(encontrar);
        if (key_pos == 0){
            throw runtime_error("No se encontró la key");
        }
        else {
            key_pos -= 1; //Pos real del key encontrado
            Record current = readRecord(key_pos, data_file);
            if (current.Nombre != key.c_str()){
                throw runtime_error("No se encontró la key");
            }

            vector<Record> result;

            //Agregamos el key_pos encontrado

            result.push_back(current);

            // Recorrer los elementos antes del key_pos para ver si se repiten y agregarlos
            for (int i = key_pos - 1; i >= 0; i--) {
                current = readRecord(i, data_file);
                Record prev = readRecord(i + 1, data_file);
                if (current.compare(prev) == 0) {
                    if (getPunteroAtPosition(i, data_file) != -1) {
                        result.push_back(current);
                    }
                } else {
                    break; // Si ya no se repiten, detener la búsqueda
                }
            }

            // Recorrer los elementos después del key_pos para ver si se repiten y agregarlos
            for (int i = key_pos + 1; i < size(data_file); i++) {
                current = readRecord(i, data_file);
                Record next = readRecord(i - 1, data_file);
                if (current.compare(next) == 0) {
                    if (getPunteroAtPosition(i, data_file) != -1) {
                        result.push_back(current);
                    }
                } else {
                    break; // Si ya no se repiten, detener la búsqueda
                }
            }


            //AUXILIAR FILE
            ifstream file2(this->aux_file, ios::binary);
            if (!file2.is_open()) {
                throw ("No se pudo abrir el archivo de datos.");
            }

            int totalRecords = size(this->aux_file);

            for (int i = 0; i < totalRecords; i++) {
                Record record = readRecord(i, this->aux_file);

                // Comparar el nombre del registro con la clave proporcionada (insensible a mayúsculas y minúsculas)
                if (strcasecmp(record.Nombre, key.c_str()) == 0 && getPunteroAtPosition(i, this->aux_file) != -1) {
                    result.push_back(record);
                }
            }

            file2.close();

            return result;

        }


    }
      ```
  4. rangeSearch(const string& begin, const string& end): Realiza una búsqueda de rango en el archivo de datos y devuelve registros cuyos nombres estén en el rango especificado.
     ```cpp
     vector<Record> rangeSearch(const string& begin, const string& end) {
        rebuild();

        //Busco con busqueda binaria la posicion del begin
        Record encontrar_begin;
        strcpy(encontrar_begin.Nombre, begin.c_str());
        int limite_inferior = binarySearchPosition(encontrar_begin);

        if (limite_inferior == 0){
            throw runtime_error("No se encontró el límite inferior");
        }
        else {
            limite_inferior -= 1;
            Record inf = readRecord(limite_inferior, this->data_file);
            if (inf.Nombre != begin){
                throw runtime_error("No se encontró la key inferior");
            }
        }

        //Busco con busqueda binaria la posicion del end
        Record encontrar_end;
        strcpy(encontrar_end.Nombre, end.c_str());
        int limite_superior = binarySearchPosition(encontrar_end);
        if (limite_superior == 0){
            throw runtime_error("No se encontró el límite superior");
        }
        else {
            limite_superior -= 1;
            Record sup = readRecord(limite_superior, this->data_file);
            if (sup.Nombre != end){
                throw runtime_error("No se encontró la key superior");
            }
        }
     vector<Record> result;

        /*// Agregar los elementos que están entre el límite inferior y el superior (Si apuntan a -1 no se agregan)
        for (int i = limite_inferior; i <= limite_superior; i++) {
            Record record = readRecord(i, data_file);
            if (getPunteroAtPosition(i, data_file) != -1) {
                result.push_back(record);
            }
        }
        */

        // Agregar los elementos que están entre el límite inferior y el superior (Si apuntan a -1 no se agregan)
        if (limite_inferior <= limite_superior);
        else swap(limite_inferior, limite_superior);// En caso de que begin sea mayor que end, intercambiamos los límites

        for (int i = limite_inferior; i <= limite_superior; i++) {
            Record record = readRecord(i, data_file);
            if (getPunteroAtPosition(i, data_file) != -1) {
                result.push_back(record);
            }
        }


        // Recorrer los elementos antes del límite inferior para ver si se repiten y agregarlos (si apuntan a -1 no se agregan)
        for (int i = limite_inferior - 1; i >= 0; i--) {
            Record current = readRecord(i, data_file);
            Record prev = readRecord(i + 1, data_file);
            if (current.compare(prev) == 0 && getPunteroAtPosition(i, data_file) != -1) {
                result.push_back(current);
            } else if (current.compare(prev) != 0){
                break; // Si ya no se repiten o apuntan a -1, detener la búsqueda
            }
        }

        // Recorrer los elementos después del límite superior para ver si se repiten y agregarlos (si apuntan a -1 no se agregan)
        for (int i = limite_superior + 1; i < size(data_file); i++) {
            Record current = readRecord(i, data_file);
            Record next = readRecord(i - 1, data_file);
            if (current.compare(next) == 0 && getPunteroAtPosition(i, data_file) != -1) {
                result.push_back(current);
            } else if (current.compare(next) != 0){
                break; // Si ya no se repiten o apuntan a -1, detener la búsqueda
            }
        }

        return result;

    }

     ```
  5. rebuild(): Este método se utiliza para reconstruir el archivo de datos principal. Elimina registros marcados como eliminados y reorganiza los registros válidos.
      ```cpp
      void rebuild() {

        ifstream dataFileStream(data_file, ios::binary);
        if (!dataFileStream.is_open()) {
            cerr << "No se pudo abrir el archivo de datos: " << data_file << endl;
            return;
        }

        ofstream newFileStream("nuevo_data_file.bin", ios::binary);
        if (!newFileStream.is_open()) {
            cerr << "No se pudo crear el nuevo archivo de datos." << endl;
            return;
        }

        Record X;
        float puntero = -0.9;
        float puntero_temporal;

        int cantidad_eliminados = 0;

        // Iterar a través de los registros en el archivo de datos
        while (dataFileStream.read(reinterpret_cast<char*>(&X), sizeof(Record))) {

            if (X.Puntero == -1){
                cantidad_eliminados += 1;
            }

            //Nos saltamos los records que apunten a -1, ya que estos estan eliminados
            if (X.Puntero != -1){
                //Agregamos X al nuevo data_file
                puntero += 1;
                puntero_temporal = X.Puntero;
                X.Puntero = puntero;
                writeRecordEND(X,"nuevo_data_file.bin");

                //En caso X apunte a un registro perteneciente al espacio auxiliar
                if (analyzeFloat(puntero_temporal ) == "Auxiliar"){
                    //Sabemos que X apunta a Y
                    //Nos ubicamos en Y
                    Record Y;
                    while (analyzeFloat(puntero_temporal) != "Data" ){
                        puntero += 1;
                        Y = readRecord(puntero_temporal,this->aux_file);
                        puntero_temporal = Y.Puntero;
                        Y.Puntero = puntero;
                        writeRecordEND(Y,"nuevo_data_file.bin");
                    };
                }
            }
        }

        dataFileStream.close();
        newFileStream.close();

        // Reemplazar el archivo original con el nuevo archivo
        if (remove(data_file.c_str()) != 0) {
            cerr << "Error al eliminar el archivo original." << endl;
        }

        if (rename("nuevo_data_file.bin", data_file.c_str()) != 0) {
            cerr << "Error al renombrar el nuevo archivo." << endl;
        }

        clearFile(this->aux_file);

        //Actualizamos la cantidad de elementos que hay en ambos archivos
        data_size += (aux_size - cantidad_eliminados);
        aux_size = 0;

    }

      ```
  6. size(string _file): Calcula y devuelve la cantidad de registros en un archivo específico.
      ```cpp
      int size(string _file){
        ifstream file(_file, ios::binary);
        if(!file.is_open()) throw ("No se pudo abrir el archivo");

        file.seekg(0, ios::end);//ubicar cursos al final del archivo
        long total_bytes = file.tellg();//cantidad de bytes del archivo
        file.close();
        return total_bytes / sizeof(Record);
    }
      
      ```
  7. analyzeFloat(float number): Determina si un número flotante corresponde a un registro almacenado en el archivo auxiliar o en el archivo de datos principal.
      ```cpp
      string analyzeFloat(float number) {
        if (std::floor(number) == number) {
            return "Auxiliar";
        } else {
            return "Data";
        }
    }

      ```
  8. binarySearchPosition(const Record& nuevoRecord): Realiza una búsqueda binaria en el archivo de datos para encontrar la posición en la que debería insertarse un nuevo registro en orden alfabético. Si el registro ya existe, devuelve la posición actual del registro encontrado.
      ```cpp
      int binarySearchPosition(const Record& nuevoRecord) {

        /*
        En este código, el método binarySearchPosition utiliza la búsqueda binaria para encontrar
        la posición de inserción del nuevo registro nuevoRecord en orden alfabético. Devolverá la
        posición en la que debería insertarse el nuevo registro en función del nombre. Si el
        registro ya existe en la lista, devolverá la posición actual del registro encontrado.
        */
        ifstream file(this->data_file, ios::binary);
        if (!file.is_open()) throw ("No se pudo abrir el archivo");

        int left = 0;
        int right = size(this->data_file) - 1;

        while (left <= right) {
            int middle = left + (right - left) / 2;
            Record record = readRecord(middle, this->data_file);

            // Comparar el nombre del registro con el nombre del nuevo record
            int cmp = record.compare(nuevoRecord);

            if (cmp == 0) {

                file.close();
                middle += 1; //Si encuentra un nombre igual que me el nuevo registro se ponga despues

                //Para la eliminacion si B se inserta despues que A, tenemos que verificar que este no apunte a un -1
                //En caso a apunte a un menos -1, vamos a ir retrocediendo de pos en data_file hasta encontrar el mas registro superior alfabeticamente a B que no este eliminado.
                while (getPunteroAtPosition(middle-1,this->data_file) == -1){
                    middle -= 1;
                    //Si esta A y quiero insertar B. Entonces el metodo me retornada left, ya que A estaba en left -1
                }

                return middle; // Encontrado, devuelve la posición actual
            } else if (cmp < 0) {
                left = middle + 1; // El nombre está en la mitad derecha
            } else {
                right = middle - 1; // El nombre está en la mitad izquierda
            }
        }

        file.close();

        // Si no se encuentra un registro igual, devolver la posición en la que debería insertarse

        //Para la eliminacion si B se inserta despues que A, tenemos que verificar que este no apunte a un -1
        //En caso a apunte a un menos -1, vamos a ir retrocediendo de pos en data_file hasta encontrar el mas registro superior alfabeticamente a B que no este eliminado.
        while (getPunteroAtPosition(left-1,this->data_file) == -1){
            left -= 1;

            //Si esta A y quiero insertar B. Entonces el metodo me retornada left, ya que A estaba en left -1
        }



        return left;
    }

      ```
  9. scanAll(string _file): Lee todos los registros de un archivo específico y los devuelve en un vector de registros.
      ```cpp
      vector<Record> scanAll(string _file){
        ifstream file(_file, ios::binary);
        if(!file.is_open()) throw ("No se pudo abrir el archivo");

        vector<Record> records;
        Record record;

        while(file.peek() != EOF){
            record = Record();
            file.read((char*) &record, sizeof(Record));
            records.push_back(record);
        }
        file.close();

        return records;
    }

      ```

## AVL File Organization
La técnica de organización de archivos AVL File se basa en la utilización de árboles AVL para almacenar registros de manera ordenada y balanceada en un archivo, basada en la estructura de árboles AVL (árbol binario de búsqueda) en la que se garantiza que la diferencia de alturas entre los subárboles izquierdo y derecho de cada nodo (conocida como el factor de equilibrio) no excede más de uno. Esto asegura que el árbol esté siempre balanceado, lo que, a su vez, garantiza que las operaciones de búsqueda, inserción y eliminación sean eficientes con un tiempo de ejecución en el peor caso de O(log n), donde "n" es el número de nodos en el árbol.

La técnica de organización de archivos AVL File se utiliza en situaciones en las que es necesario realizar operaciones de búsqueda y manipulación eficientes en un archivo de registros. Cada nodo en el árbol AVL representa un registro en el archivo y contiene información sobre el registro, así como punteros a los nodos hijos izquierdo y derecho. La estructura del árbol garantiza que los registros se almacenan en un orden específico que facilita la búsqueda binaria, lo que resulta en tiempos de búsqueda muy eficientes.

En las operaciones de eliminación en un archivo AVLFile, se localiza el nodo que contiene el registro a eliminar y se efectúan rotaciones para mantener el equilibrio y la altura adecuada en el árbol. La eliminación física del elemento en el archivo se pospone hasta que se realice un rebuild.

![avl]/avl.png

- *Métodos importantes*
  1. Record find: Busca un registro en el árbol AVL según una clave dada.
     ```cpp
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
    
     ```
  2. void insert(Record record): Inserta un nuevo registro en el árbol AVL.
     ```cpp
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
     ```
  3. void remove(T key): Elimina un registro del árbol AVL según una clave dada.
     ```cpp
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
     ```
  4. vector<Record> searchRange(int start, int end, int i, vector<struct Record> vector): Busca registros en un rango específico de claves en el árbol AVL.
     ```cpp
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
     ```

## Extendible Hashing

## Parser SQL

La implementación del parser para consultas SQL se ha llevado a cabo siguiendo un enfoque modular y escalable. En primer lugar, hemos desarrollado un scanner que se encarga de tokenizar los lexemas de una cadena de consulta SQL. Este scanner analiza la entrada de texto y divide la consulta en tokens significativos, como palabras clave **(SELECT, INSERT, DELETE, CREATE)**, identificadores **(Campos o ID's)**, operadores y valores. Una vez que tenemos estos tokens el parser se encarga de analizar secuencialmente estos tokens, siguiendo las reglas sintácticas del lenguaje SQL en base a la diferentes tipos de consultas que se quieren hacer. A medida que avanza, verifica la estructura y la coherencia de las consultas, asegurándose de que cumplan con la sintaxis requerida, por ejemplo, si se quiere hacer un **SELECT**, se le va pidiendo al scanner los siguientes tokens y con una funcion verificamos si es el token esperado para poder continuar, si todo es correcto se ejecutan las consultas.

