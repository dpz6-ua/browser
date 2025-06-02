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
    // A�adir cuantos m�todos se consideren necesarios para manejar la parte
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
    int ft; // Frecuencia del t�rmino en el documento
    vector<int> posTerm;
    // Solo se almacenar� esta informaci�n si el campo privado del indexador almacenarPosTerm == true
    // Lista de n�meros de palabra en los que aparece el t�rmino en el
    // documento. Los n�meros de palabra comenzar�n desde cero (la primera
    // palabra del documento). Se numerar�n las palabras de parada. Estar�
    // ordenada de menor a mayor posici�n.
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
    ~InformacionTermino (); // Pone ftc = 0 y vac�a l_docs
    InformacionTermino & operator= (const InformacionTermino &);
    // A�adir cuantos m�todos se consideren necesarios para manejar la parte
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
    int ftc; // Frecuencia total del t�rmino en la colecci�n
    unordered_map<int, InfTermDoc> l_docs;
    // Tabla Hash que se acceder� por el id del documento, devolviendo un
    // objeto de la clase InfTermDoc que contiene toda la informaci�n de
    // aparici�n del t�rmino en el documento
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
    // A�adir cuantos m�todos se consideren necesarios para manejar la parte
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
    // colecci�n ser� el identificador 1
    int numPal;             // N� total de palabras del documento
    int numPalSinParada;    // N� total de palabras sin stop-words del documento
    int numPalDiferentes;   // N� total de palabras diferentes que no sean stop-words 
                            //(sin acumular la frecuencia de cada una de ellas)
    int tamBytes;           // Tama�o en bytes del documento
    Fecha fechaModificacion;
    // Atributo correspondiente a la fecha y hora (completa) de modificaci�n
    // del documento. El tipo ?Fecha/hora? lo elegir�/implementar� el alumno
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
    // A�adir cuantos m�todos se consideren necesarios para manejar la parte
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
    int numDocs; // N� total de documentos en la colecci�n
    int numTotalPal; // N� total de palabras en la colecci�n
    int numTotalPalSinParada; // N� total de palabras sin stop-words en la colecci�n
    int numTotalPalDiferentes;
    // N� total de palabras diferentes en la colecci�n que no sean stop-
    // words (sin acumular la frecuencia de cada una de ellas)
    int tamBytes; // Tama�o total en bytes de la colecci�n
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
    // A�adir cuantos m�todos se consideren necesarios para manejar la parte 
    // privada de la clase

    void incrementarFT();
    void agregarPosTerm(int pos);
    int getFT() const {
        return ft;
    }
    
    private:
    int ft; // Frecuencia total del t�rmino en la pregunta
    vector<int> posTerm;
    // Solo se almacenar� esta informaci�n si el campo privado del indexador almacenarPosTerm == true
    // Lista de n�meros de palabra en los que aparece el t�rmino en la
    // pregunta. Los n�meros de palabra comenzar�n desde cero (la primera
    // palabra de la pregunta). Se numerar�n las palabras de parada. Estar�
    // ordenada de menor a mayor posici�n.
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
    // A�adir cuantos m�todos se consideren necesarios para manejar la parte
    // privada de la clase
    void incrementarNumPal();
    void incrementarNumPalSinParada();
    void incrementarNumPalDiferentes();
    
    private:
    int numTotalPal;
    // N� total de palabras en la pregunta
    int numTotalPalSinParada;
    // N� total de palabras sin stop-words en la pregunta
    int numTotalPalDiferentes;
    // N� total de palabras diferentes en la pregunta que no sean stop-words
    // (sin acumular la frecuencia de cada una de ellas)
};

#endif