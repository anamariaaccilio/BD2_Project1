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

## AVL File Organization

## Extendible Hashing

## Parser SQL

La implementación del parser para consultas SQL se ha llevado a cabo siguiendo un enfoque modular y escalable. En primer lugar, hemos desarrollado un scanner que se encarga de tokenizar los lexemas de una cadena de consulta SQL. Este scanner analiza la entrada de texto y divide la consulta en tokens significativos, como palabras clave (SELECT, INSERT, DELETE, CREATE), identificadores(Campos o ID's), operadores y valores. Una vez que tenemos estos tokens el parser se encarga de analizar secuencialmente estos tokens, siguiendo las reglas sintácticas del lenguaje SQL en base a la diferentes tipos de consultas que se quieren hacer. A medida que avanza, verifica la estructura y la coherencia de las consultas, asegurándose de que cumplan con la sintaxis requerida, por ejemplo, si se quiere hacer un SELECT, se le va pidiendo al scanner los siguientes tokens y con una funcion verificamos si es el token esperado para poder continuar, si todo es correcto se ejecutan las consultas.

