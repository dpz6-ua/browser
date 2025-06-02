#ifndef INDEXADOR_INFORMACION_H
#define INDEXADOR_INFORMACION_H

#include <iostream>
#include <unordered_map>
#include <list>
#include <vector>
using namespace std;

/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//////////////  CLASE FECHA /////////////////////////
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////

class Fecha {
    friend ostream& operator<<(ostream& s, const Fecha& p);
    friend istream& operator>>(istream& s, Fecha& p);
    
public:
    Fecha(int d, int m, int a, int h, int mm, int s);
    Fecha(const Fecha&);
    Fecha();
    ~Fecha();
    Fecha& operator=(const Fecha&);
    bool operator==(const Fecha&) const;
    bool operator!=(const Fecha&) const;
    bool operator<(const Fecha&) const;
    bool operator>(const Fecha&) const;
    bool operator<=(const Fecha&) const;
    bool operator>=(Fecha&) const;

private:
    int dia, mes, anio;
    int hora, minuto, segundo;
};

/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
///// CLASE INFORMACION TERMINO DOCUMENTO ///////////
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////

class InfTermDoc {
    friend ostream& operator<<(ostream& s, const InfTermDoc& p);
    friend istream& operator>>(istream& s, InfTermDoc& p);
    
    public:
    InfTermDoc (const InfTermDoc &);
    InfTermDoc (); // Inicializa ft = 0
    ~InfTermDoc (); // Pone ft = 0
    InfTermDoc & operator= (const InfTermDoc &);
    // Añadir cuantos métodos se consideren necesarios para manejar la parte
    // privada de la clase
    void incrementarFT();
    void anadirPosTerm(int pos);
    
    int getFT() {
        return ft;
    }

    int getFT() const {
        return ft;
    }
    
    private:
    int ft; // Frecuencia del término en el documento
    vector<int> posTerm;
    // Solo se almacenará esta información si el campo privado del indexador almacenarPosTerm == true
    // Lista de números de palabra en los que aparece el término en el
    // documento. Los números de palabra comenzarán desde cero (la primera
    // palabra del documento). Se numerarán las palabras de parada. Estará
    // ordenada de menor a mayor posición.
};

/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
///////////  CLASE INFORMACION TERMINO //////////////
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////

class InformacionTermino {
    friend ostream& operator<<(ostream& s, const InformacionTermino& p);
    friend istream& operator>>(istream& s, InformacionTermino& p);
    
    public:
    InformacionTermino (const InformacionTermino &);
    InformacionTermino (); // Inicializa ftc = 0
    ~InformacionTermino (); // Pone ftc = 0 y vacía l_docs
    InformacionTermino & operator= (const InformacionTermino &);
    // Añadir cuantos métodos se consideren necesarios para manejar la parte
    // privada de la clase

    void incrementarFTC();

    void decrementarFTC(int ft) {
        ftc -= ft;
    }

    int getFTC() const {
        return ftc;
    }

    InfTermDoc& obtenerInfTermDoc(int idDoc) {
        return l_docs[idDoc];
    }

    const InfTermDoc& obtenerInfTermDoc(int idDoc) const {
        return l_docs.at(idDoc);
    }

    bool existeEnDocumento(int idDoc) const {
        return l_docs.find(idDoc) != l_docs.end();
    }

    void eliminarDocumento(int idDoc) {
        auto it = l_docs.find(idDoc);
        if (it != l_docs.end()) {
            decrementarFTC(it->second.getFT());
        }
        l_docs.erase(idDoc);
    }

    const unordered_map<int, InfTermDoc>& getL_docs() const {
        return l_docs;
    }
    
    private:
    int ftc; // Frecuencia total del término en la colección
    unordered_map<int, InfTermDoc> l_docs;
    // Tabla Hash que se accederá por el id del documento, devolviendo un
    // objeto de la clase InfTermDoc que contiene toda la información de
    // aparición del término en el documento
};

/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
///////////  CLASE INFORMACION DOCUMENTO ////////////
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////

class InfDoc {
    friend ostream& operator<<(ostream& s, const InfDoc& p);
    friend istream& operator>>(istream& s, InfDoc& p);

    public:
    InfDoc (const InfDoc &);
    InfDoc ();
    ~InfDoc ();
    InfDoc & operator= (const InfDoc &);
    // Añadir cuantos métodos se consideren necesarios para manejar la parte
    // privada de la clase
    Fecha obtenerFechaModificacion() const;
    void setIDoc(int id);
    void incrementarNumPal();
    void incrementarNumPalSinParada();
    void incrementarNumPalDiferentes();
    void incrementarTamBytes(int bytes);
    void setFechaModificacion(const Fecha& fecha);
    int getIDoc() const {
        return idDoc;
    }
    int getNumPal() const {
        return numPal;
    }
    int getNumPalSinParada() const {
        return numPalSinParada;
    }
    int getNumPalDiferentes() const {
        return numPalDiferentes;
    }
    int getTamBytes() const {
        return tamBytes;
    }

    Fecha getFechaModificacion() const {
        return fechaModificacion;
    }
    
    private:
    int idDoc;
    // Identificador del documento. El primer documento indexado en la
    // colección será el identificador 1
    int numPal;             // Nº total de palabras del documento
    int numPalSinParada;    // Nº total de palabras sin stop-words del documento
    int numPalDiferentes;   // Nº total de palabras diferentes que no sean stop-words 
                            //(sin acumular la frecuencia de cada una de ellas)
    int tamBytes;           // Tamaño en bytes del documento
    Fecha fechaModificacion;
    // Atributo correspondiente a la fecha y hora (completa) de modificación
    // del documento. El tipo ?Fecha/hora? lo elegirá/implementará el alumno
};

/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//// CLASE INFORMACION COLECCION DOCUMENTOS /////////
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////

class InfColeccionDocs {
    friend ostream& operator<<(ostream& s, const InfColeccionDocs& p);
    friend istream& operator>>(istream& s, InfColeccionDocs& p);

    public:
    InfColeccionDocs (const InfColeccionDocs &);
    InfColeccionDocs ();
    ~InfColeccionDocs ();
    InfColeccionDocs & operator= (const InfColeccionDocs &);
    // Añadir cuantos métodos se consideren necesarios para manejar la parte
    // privada de la clase

    void actualizar(InfDoc& doc);
    void setNumPalDiferentes(int numPalDiferentes);
    void borrarDoc(InfDoc& doc) {
        numDocs--;
        numTotalPal -= doc.getNumPal();
        numTotalPalSinParada -= doc.getNumPalSinParada();
        tamBytes -= doc.getTamBytes();
    }

    int getNumDocs() const {
        return numDocs;
    }

    int getNumTotalPal() const {
        return numTotalPal;
    }

    int getNumTotalPalSinParada() const {
        return numTotalPalSinParada;
    }


private:
    int numDocs; // Nº total de documentos en la colección
    int numTotalPal; // Nº total de palabras en la colección
    int numTotalPalSinParada; // Nº total de palabras sin stop-words en la colección
    int numTotalPalDiferentes;
    // Nº total de palabras diferentes en la colección que no sean stop-
    // words (sin acumular la frecuencia de cada una de ellas)
    int tamBytes; // Tamaño total en bytes de la colección
};

/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
/////// CLASE INFORMACION TERMINO PREGUNTA //////////
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////

class InformacionTerminoPregunta {
    friend ostream& operator<<(ostream& s, const InformacionTerminoPregunta&p);
    friend istream& operator>>(istream& s, InformacionTerminoPregunta&p);

    public:
    InformacionTerminoPregunta (const InformacionTerminoPregunta &);
    InformacionTerminoPregunta ();
    ~InformacionTerminoPregunta ();
    InformacionTerminoPregunta & operator= (const InformacionTerminoPregunta &);
    // Añadir cuantos métodos se consideren necesarios para manejar la parte 
    // privada de la clase

    void incrementarFT();
    void agregarPosTerm(int pos);
    int getFT() const {
        return ft;
    }
    
    private:
    int ft; // Frecuencia total del término en la pregunta
    vector<int> posTerm;
    // Solo se almacenará esta información si el campo privado del indexador almacenarPosTerm == true
    // Lista de números de palabra en los que aparece el término en la
    // pregunta. Los números de palabra comenzarán desde cero (la primera
    // palabra de la pregunta). Se numerarán las palabras de parada. Estará
    // ordenada de menor a mayor posición.
};

/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
///////////  CLASE INFORMACION PREGUNTA /////////////
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////

class InformacionPregunta {
    friend ostream& operator<<(ostream& s, const InformacionPregunta& p);
    friend istream& operator>>(istream& s, InformacionPregunta& p);
    
    public:
    InformacionPregunta (const InformacionPregunta &);
    InformacionPregunta ();
    ~InformacionPregunta ();
    InformacionPregunta & operator= (const InformacionPregunta &);
    // Añadir cuantos métodos se consideren necesarios para manejar la parte
    // privada de la clase
    void incrementarNumPal();
    void incrementarNumPalSinParada();
    void incrementarNumPalDiferentes();
    
    private:
    int numTotalPal;
    // Nº total de palabras en la pregunta
    int numTotalPalSinParada;
    // Nº total de palabras sin stop-words en la pregunta
    int numTotalPalDiferentes;
    // Nº total de palabras diferentes en la pregunta que no sean stop-words
    // (sin acumular la frecuencia de cada una de ellas)
};

#endif