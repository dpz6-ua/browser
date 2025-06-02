#include "../include/indexadorInformacion.h"

/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
///// CLASE INFORMACION TERMINO DOCUMENTO ///////////
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////

InfTermDoc::InfTermDoc (const InfTermDoc &p) {
    ft = p.ft;
    posTerm = p.posTerm;
}

InfTermDoc::InfTermDoc () {
    ft = 0;
}

InfTermDoc::~InfTermDoc () {
    ft = 0;
}

InfTermDoc& InfTermDoc::operator= (const InfTermDoc &p) {
    if (this != &p) {
        ft = p.ft;
        posTerm = p.posTerm;
    }
    return *this;
}

void InfTermDoc::incrementarFT(){
    ft++;
}

void InfTermDoc::anadirPosTerm(int pos){
    posTerm.push_back(pos);
}

ostream& operator<<(ostream& s, const InfTermDoc& p) {
    s << "ft: " << p.ft;
    // A continuación se mostrarían todos los elementos de p.posTerm (?posicion
    //TAB posicion TAB ... posicion, es decir nunca finalizará en un TAB?): 
    for (const auto& posicion : p.posTerm) {
        s << "\t" << posicion;
    }
    return s;
}

istream& operator>>(istream& s, InfTermDoc& p) {
    string term;
    s >> term;    // Leer "ft:"
    s >> p.ft;    // Leer la frecuencia del término

    p.posTerm.clear();

    // Intentamos leer la primera posición (tentativamente)
    int pos;
    if (s >> pos) {
        // Si se leyó correctamente un número, lo añadimos
        p.posTerm.push_back(pos);

        // Leemos el resto de posiciones (si ft > 1)
        for (int i = 1; i < p.ft; ++i) {
            if (s >> pos)
                p.posTerm.push_back(pos);
            else
                break;  // Por seguridad
        }
    } else {
        // Si no se leyó número, limpiamos el error y dejamos el stream listo
        s.clear();
    }

    return s;
}

/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
///////////  CLASE INFORMACION TERMINO //////////////
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////

InformacionTermino::InformacionTermino (const InformacionTermino &p) {
    ftc = p.ftc;
    l_docs = p.l_docs;
}

InformacionTermino::InformacionTermino () {
    ftc = 0;
}

InformacionTermino::~InformacionTermino () {
    ftc = 0;
    l_docs.clear();
}

InformacionTermino& InformacionTermino::operator= (const InformacionTermino &p) {
    if (this != &p) {
        ftc = p.ftc;
        l_docs = p.l_docs;
    }
    return *this;
}

void InformacionTermino::incrementarFTC(){
    ftc++;
}

ostream& operator<<(ostream& s, const InformacionTermino& p) {
    s << "Frecuencia total: " << p.ftc << "\tfd: " << p.l_docs.size();
    // A continuación se mostrarían todos los elementos de p.l_docs: 
    for (const auto& par : p.l_docs) {  // par es un std::pair<const int, InfTermDoc>
        s << "\tId.Doc: " << par.first << "\t" << par.second;
    }
    return s;
}

istream& operator>>(istream& s, InformacionTermino& p) {
    int numDocs;
    string term;
    s >> term;  // "Frecuencia
    s >> term;  // "total:"
    s >> p.ftc; // Leer la frecuencia total del término en la colección
    s >> term;  // "fd:"
    s >> numDocs;  // p.l_docs.size();

    p.l_docs.clear();
    for (int i = 0; i < numDocs; i++) {
        s >> term; // "Id.Doc:"
        InfTermDoc info;
        int idDoc;
        s >> idDoc; // Leer id del documento y la información del término en el doc
        //cout << "INFORMACION TERMINO Id.Doc: " << idDoc << endl;
        s >> info; // Leer la información del término en el documento
        p.l_docs[idDoc] = info;
    }
    
    return s;
}

/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
///////////  CLASE INFORMACION DOCUMENTO ////////////
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////

InfDoc::InfDoc (const InfDoc &p) {
    idDoc = p.idDoc;
    numPal = p.numPal;
    numPalSinParada = p.numPalSinParada;
    numPalDiferentes = p.numPalDiferentes;
    tamBytes = p.tamBytes;
}

InfDoc::InfDoc () {
    idDoc = 0;
    numPal = 0;
    numPalSinParada = 0;
    numPalDiferentes = 0;
    tamBytes = 0;
}

InfDoc::~InfDoc () {
    idDoc = 0;
    numPal = 0;
    numPalSinParada = 0;
    numPalDiferentes = 0;
    tamBytes = 0;
}

InfDoc& InfDoc::operator= (const InfDoc &p) {
    if (this != &p) {
        idDoc = p.idDoc;
        numPal = p.numPal;
        numPalSinParada = p.numPalSinParada;
        numPalDiferentes = p.numPalDiferentes;
        tamBytes = p.tamBytes;
    }
    return *this;
}

Fecha InfDoc::obtenerFechaModificacion() const{
    return fechaModificacion;
}

void InfDoc::setIDoc(int id) {
    idDoc = id;
}

void InfDoc::incrementarNumPal(){
    numPal++;
}

void InfDoc::incrementarNumPalSinParada(){
    numPalSinParada++;
}

void InfDoc::incrementarNumPalDiferentes(){
    numPalDiferentes++;
}

void InfDoc::incrementarTamBytes(int bytes){
    tamBytes += bytes;
}

void InfDoc::setFechaModificacion(const Fecha& fecha) {
    fechaModificacion = fecha;
}

ostream& operator<<(ostream& s, const InfDoc& p) {
    s << "idDoc: " << p.idDoc << "\tnumPal: " << p.numPal <<
    "\tnumPalSinParada: " << p.numPalSinParada << "\tnumPalDiferentes: " <<
    p.numPalDiferentes << "\ttamBytes: " << p.tamBytes;
    return s;
}

istream& operator>>(istream& s, InfDoc& p) {
    string term;
    s >> term;  // "idDoc:"
    s >> p.idDoc;
    s >> term;  // "numPal:"
    s >> p.numPal;
    s >> term;  // "numPalSinParada:"
    s >> p.numPalSinParada;
    s >> term;  // "numPalDiferentes:"
    s >> p.numPalDiferentes;
    s >> term;  // "tamBytes:"
    s >> p.tamBytes;
    s >> p.fechaModificacion;
    //cout << "FECHA MODIFICACION: " << p.fechaModificacion << endl;
    
    return s;
}

/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//// CLASE INFORMACION COLECCION DOCUMENTOS /////////
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////

InfColeccionDocs::InfColeccionDocs (const InfColeccionDocs &p) {
    numDocs = p.numDocs;
    numTotalPal = p.numTotalPal;
    numTotalPalSinParada = p.numTotalPalSinParada;
    numTotalPalDiferentes = p.numTotalPalDiferentes;
    tamBytes = p.tamBytes;
}

InfColeccionDocs::InfColeccionDocs () {
    numDocs = 0;
    numTotalPal = 0;
    numTotalPalSinParada = 0;
    numTotalPalDiferentes = 0;
    tamBytes = 0;
}

InfColeccionDocs::~InfColeccionDocs () {
    numDocs = 0;
    numTotalPal = 0;
    numTotalPalSinParada = 0;
    numTotalPalDiferentes = 0;
    tamBytes = 0;
}

InfColeccionDocs& InfColeccionDocs::operator= (const InfColeccionDocs &p) {
    if (this != &p) {
        numDocs = p.numDocs;
        numTotalPal = p.numTotalPal;
        numTotalPalSinParada = p.numTotalPalSinParada;
        numTotalPalDiferentes = p.numTotalPalDiferentes;
        tamBytes = p.tamBytes;
    }
    return *this;
}

void InfColeccionDocs::actualizar(InfDoc& doc) {
    numDocs++;
    numTotalPal += doc.getNumPal();
    numTotalPalSinParada += doc.getNumPalSinParada();
    tamBytes += doc.getTamBytes();
}

void InfColeccionDocs::setNumPalDiferentes(int numPalDiferentes) {
    this->numTotalPalDiferentes = numPalDiferentes;
}

ostream& operator<<(ostream& s, const InfColeccionDocs& p){
    s << "numDocs: " << p.numDocs << "\tnumTotalPal: " << p.numTotalPal <<
    "\tnumTotalPalSinParada: " << p.numTotalPalSinParada <<
    "\tnumTotalPalDiferentes: " << p.numTotalPalDiferentes << "\ttamBytes: " << p.tamBytes;
    return s;
}

istream& operator>>(istream& s, InfColeccionDocs& p) {
    string term;
    s >> term;  // "numDocs:"
    s >> p.numDocs;
    s >> term;  // "numTotalPal:"
    s >> p.numTotalPal;
    s >> term;  // "numTotalPalSinParada:"
    s >> p.numTotalPalSinParada;
    s >> term;  // "numTotalPalDiferentes:"
    s >> p.numTotalPalDiferentes;
    s >> term;  // "tamBytes:"
    s >> p.tamBytes;
    
    return s;
}

/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
/////// CLASE INFORMACION TERMINO PREGUNTA //////////
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////

InformacionTerminoPregunta::InformacionTerminoPregunta (const InformacionTerminoPregunta &p) {
    ft = p.ft;
    posTerm = p.posTerm;
}

InformacionTerminoPregunta::InformacionTerminoPregunta () {
    ft = 0;
}

InformacionTerminoPregunta::~InformacionTerminoPregunta () {
    ft = 0;
}

InformacionTerminoPregunta& InformacionTerminoPregunta::operator= (const InformacionTerminoPregunta &p) {
    if (this != &p) {
        ft = p.ft;
        posTerm = p.posTerm;
    }
    return *this;
}

void InformacionTerminoPregunta::incrementarFT(){
    ft++;
}

void InformacionTerminoPregunta::agregarPosTerm(int pos){
    posTerm.push_back(pos);
}


ostream& operator<<(ostream& s, const InformacionTerminoPregunta& p) {
    s << "ft: " << p.ft;
    // A continuación se mostrarían todos los elementos de p.posTerm (?posicion
    // TAB posicion TAB ... posicion, es decir nunca finalizará en un TAB?): 
    for (const auto& posicion : p.posTerm){
        s << "\t" << posicion;
    }
    return s;
}

istream& operator>>(istream& s, InformacionTerminoPregunta&p){
    int numPos;
    string term;
    s >> term;  // "ft:"
    s >> p.ft;  // Leer la frecuencia del término en la pregunta
    // cout << "INFORMACION TERMINO PREGUNTA ft: " << p.ft << endl;
    
    p.posTerm.clear();
    for (int i = 0; i < p.ft; i++) {
        s >> numPos;
        p.posTerm.push_back(numPos);
    }
    
    return s;
}

/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
///////////  CLASE INFORMACION PREGUNTA /////////////
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////

InformacionPregunta::InformacionPregunta (const InformacionPregunta &p) {
    numTotalPal = p.numTotalPal;
    numTotalPalSinParada = p.numTotalPalSinParada;
    numTotalPalDiferentes = p.numTotalPalDiferentes;
}

InformacionPregunta::InformacionPregunta () {
    numTotalPal = 0;
    numTotalPalSinParada = 0;
    numTotalPalDiferentes = 0;
}

InformacionPregunta::~InformacionPregunta () {
    numTotalPal = 0;
    numTotalPalSinParada = 0;
    numTotalPalDiferentes = 0;
}

InformacionPregunta& InformacionPregunta::operator= (const InformacionPregunta &p) {
    if (this != &p) {
        numTotalPal = p.numTotalPal;
        numTotalPalSinParada = p.numTotalPalSinParada;
        numTotalPalDiferentes = p.numTotalPalDiferentes;
    }
    return *this;
}

void InformacionPregunta::incrementarNumPal(){
    numTotalPal++;
}

void InformacionPregunta::incrementarNumPalSinParada(){
    numTotalPalSinParada++;
}

void InformacionPregunta::incrementarNumPalDiferentes(){
    numTotalPalDiferentes++;
}

ostream& operator<<(ostream& s, const InformacionPregunta& p){
    s << "numTotalPal: " << p.numTotalPal << "\tnumTotalPalSinParada: "<<
    p.numTotalPalSinParada << "\tnumTotalPalDiferentes: " << p.numTotalPalDiferentes;
    return s;
}

istream& operator>>(istream& s, InformacionPregunta& p) {
    string term;
    s >> term;  // "numTotalPal:"
    s >> p.numTotalPal;
    s >> term;  // "numTotalPalSinParada:"
    s >> p.numTotalPalSinParada;
    s >> term;  // "numTotalPalDiferentes:"
    s >> p.numTotalPalDiferentes;
    
    return s;
}

/////////////////////////////////////////////////////
/////////////////////////////////////////////////////
//////////////  CLASE FECHA /////////////////////////
/////////////////////////////////////////////////////
/////////////////////////////////////////////////////

Fecha::Fecha(int d, int m, int a, int h, int mm, int s)
    : dia(d), mes(m), anio(a), hora(h), minuto(mm), segundo(s) {}

Fecha::Fecha() : dia(1), mes(1), anio(1970), hora(0), minuto(0), segundo(0) {}

Fecha::Fecha(const Fecha&) = default;
Fecha::~Fecha() = default;
Fecha& Fecha::operator=(const Fecha&) = default;

bool Fecha::operator==(const Fecha& other) const {
    return std::tie(dia, mes, anio, hora, minuto, segundo) ==
           std::tie(other.dia, other.mes, other.anio, other.hora, other.minuto, other.segundo);
}

bool Fecha::operator!=(const Fecha& other) const {
    return !(*this == other);
}

bool Fecha::operator<(const Fecha& other) const {
    return std::tie(anio, mes, dia, hora, minuto, segundo) <
           std::tie(other.anio, other.mes, other.dia, other.hora, other.minuto, other.segundo);
}

bool Fecha::operator>(const Fecha& other) const {
    return other < *this;
}

bool Fecha::operator<=(const Fecha& other) const {
    return !(*this > other);
}

bool Fecha::operator>=(Fecha& other) const {
    return !(*this < other);
}

ostream& operator<<(ostream& os, const Fecha& f) {
    os << f.dia << "/" << f.mes << "/" << f.anio << " "
       << f.hora << ":" << f.minuto << ":" << f.segundo;
    return os;
}

istream& operator>>(istream& is, Fecha& f) {
    char sep;
    is >> f.dia >> sep >> f.mes >> sep >> f.anio >> f.hora >> sep >> f.minuto >> sep >> f.segundo;
    return is;
}