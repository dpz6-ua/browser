#include "../include/indexadorHash.h"

IndexadorHash::IndexadorHash(const string& fichStopWords, const string& delimitadores,
    const bool& detectComp, const bool& minuscSinAcentos, const string&
    dirIndice, const int& tStemmer, const bool& almPosTerm) :
    ficheroStopWords(fichStopWords), tok(delimitadores, detectComp, minuscSinAcentos),
    directorioIndice(dirIndice), tipoStemmer(tStemmer), almacenarPosTerm(almPosTerm) {
    // Inicializa el índice y la información de la colección de documentos
    indice.clear();
    indiceDocs.clear();
    // Cargar las palabras de parada desde el fichero
    ifstream stopWordsFile(fichStopWords);
    if (stopWordsFile.is_open()) {
        string word;
        stemmerPorter stemmer;

        while (getline(stopWordsFile, word)) {
            // Elimina espacios iniciales y finales
            word.erase(0, word.find_first_not_of(" \t\r\n"));
            word.erase(word.find_last_not_of(" \t\r\n") + 1);
            stopWords_sinStemming.insert(word);

            // Convierte a minúsculas
            transform(word.begin(), word.end(), word.begin(), ::tolower);

            // Aplica stemming si corresponde
            if (tipoStemmer != 0) {
                stemmer.stemmer(word, tipoStemmer);
            }

            // Inserta la palabra ya transformada al set de stopwords
            stopWords.insert(word);
        }

        stopWordsFile.close();
    }
    
    informacionColeccionDocs = InfColeccionDocs();
    // Inicializar el índice de preguntas
    indicePregunta.clear();
    pregunta = "";
    infPregunta = InformacionPregunta();
}

IndexadorHash::IndexadorHash(const string& directorioIndexacion) {

    if(!RecuperarIndexacion(directorioIndexacion)){
        //cerr << "ERROR: No se pudo recuperar la indexación desde el directorio: " << directorioIndexacion << endl;
        ficheroStopWords = "";
        directorioIndice = "";
        tipoStemmer = 0;
        almacenarPosTerm = false;
        indice.clear();
        indiceDocs.clear();
        stopWords.clear();
        informacionColeccionDocs = InfColeccionDocs();
        pregunta = "";
        indicePregunta.clear();
        infPregunta = InformacionPregunta();
    }
}

IndexadorHash::IndexadorHash(const IndexadorHash &ih){
    this->ficheroStopWords = ih.ficheroStopWords;
    this->directorioIndice = ih.directorioIndice;
    this->tipoStemmer = ih.tipoStemmer;
    this->almacenarPosTerm = ih.almacenarPosTerm;
    this->indice = ih.indice;
    this->indiceDocs = ih.indiceDocs;
    this->informacionColeccionDocs = ih.informacionColeccionDocs;
    this->pregunta = ih.pregunta;
    this->indicePregunta = ih.indicePregunta;
    this->infPregunta = ih.infPregunta;
    this->stopWords = ih.stopWords;
    this->tok = ih.tok;
}

IndexadorHash::~IndexadorHash() {
    // Destructor: liberar recursos si es necesario
}

IndexadorHash& IndexadorHash::operator= (const IndexadorHash&) {
    // Asignación: implementar la lógica de copia profunda si es necesario
    return *this;
}

Fecha IndexadorHash::obtenerFechaModificacion(const string& rutaArchivo) {
    struct stat atributos;
    
    if (stat(rutaArchivo.c_str(), &atributos) != 0) {
        //perror("Error en stat()");
        return Fecha();
    }

    tm* tm = localtime(&atributos.st_mtime);
    //cout << "Fecha de modificación: " << tm->tm_mday << "/" << (tm->tm_mon + 1) << "/" << (tm->tm_year + 1900) << endl;
    
    return Fecha(
        tm->tm_mday,        // Día (1-31)
        tm->tm_mon + 1,     // Mes (0-11 ? ajustamos a 1-12)
        tm->tm_year + 1900, // Año (desde 1900 ? ajustamos)
        tm->tm_hour,        // Horas (0-23)
        tm->tm_min,         // Minutos (0-59)
        tm->tm_sec          // Segundos (0-59)
    );
}

bool IndexadorHash::IndexarDocumento(const string& documento) {
    // Verificar acceso al documento
    ifstream testFile(documento);
    if (!testFile.is_open()) {
        //cerr << "ERROR: No se pudo abrir el documento: " << documento << endl;
        return false;
    }
    testFile.close();

    // Verificar si el documento necesita reindexación
    Fecha fechaActual = obtenerFechaModificacion(documento);
    auto itDoc = indiceDocs.find(documento);
    if (itDoc != indiceDocs.end()) {
        if (itDoc->second.obtenerFechaModificacion() >= fechaActual) {
            return true; // Documento actualizado, no necesita reindexar
        }
        // Eliminar versión obsoleta
        if(!BorraDoc(documento)){
            //cerr << "ERROR: No se pudo eliminar el documento del índice: " << documento << endl;
            return false;
        }
    }

    // Tokenización
    if (!tok.Tokenizar(documento)) {
        //cerr << "ERROR: Falló la tokenización del documento: " << documento << endl;
        return false;
    }

    string doc_tokenizado = documento + ".tk";
    // Procesamiento de tokens
    ifstream tokenFile(doc_tokenizado);
    InfDoc infoDoc;
    int nuevoId = indiceDocs.size() + 1;
    infoDoc.setIDoc(nuevoId);
    infoDoc.setFechaModificacion(fechaActual);
    //cout << infoDoc.getFechaModificacion() << endl;

    string token;
    int posicion = 0;
    while (tokenFile >> token) {
        infoDoc.incrementarNumPal();

        // Stemming
        string termino = token;
        transform(termino.begin(), termino.end(), termino.begin(), ::tolower);
        if (tipoStemmer != 0) {
            stemmerPorter stemmer;
            stemmer.stemmer(termino, tipoStemmer);
        }
        
        if (stopWords.find(termino) == stopWords.end()) {
            infoDoc.incrementarNumPalSinParada();

            // Verificar si es un término nuevo antes de acceder
            bool esTerminoNuevo = (indice.find(termino) == indice.end());
            
            // Actualización del índice
            InformacionTermino& infoTermino = indice[termino];
            bool primeraAparicion = !infoTermino.existeEnDocumento(infoDoc.getIDoc());
            infoTermino.incrementarFTC();

            if (primeraAparicion) {
                infoDoc.incrementarNumPalDiferentes();
            }
            
            // Acceso a l_docs de InformacionTermino
            InfTermDoc& infoTermDoc = infoTermino.obtenerInfTermDoc(nuevoId);
            infoTermDoc.incrementarFT();
            
            if (almacenarPosTerm) {
                infoTermDoc.anadirPosTerm(posicion);
            }
        }
        posicion++;
    }

    tokenFile.close();

    // tamaño del documento
    ifstream docFile(documento, ios::binary | ios::ate);
    infoDoc.incrementarTamBytes(docFile.tellg());
    docFile.close();

    // Actualizar indice de documentos
    indiceDocs[documento] = infoDoc;
    indiceDocs[documento].setFechaModificacion(fechaActual);
    informacionColeccionDocs.actualizar(infoDoc);
    informacionColeccionDocs.setNumPalDiferentes(indice.size());

    return true;
}

bool IndexadorHash::Indexar(const string& ficheroDocumentos) {
    ifstream file(ficheroDocumentos);
    if (!file.is_open()) {
        //cerr << "ERROR: No se pudo abrir el fichero de documentos: " << ficheroDocumentos << endl;
        return false;
    }
    string ficheroLinea;
    while (getline(file, ficheroLinea)) {
        if (ficheroLinea.empty()) {
            continue; // Ignorar líneas vacías
        }
        if(!IndexarDocumento(ficheroLinea)) {
            //cerr << "ERROR: No se pudo completar la indexación"<< endl;
            return false;
        }
    }
    return true;
}

bool IndexadorHash::IndexarDirectorio(const std::string& dirAIndexar) {
    return RecorrerDirectorio(dirAIndexar);
}

bool IndexadorHash::RecorrerDirectorio(const std::string& ruta) {
    DIR* dir = opendir(ruta.c_str());
    if (!dir) {
        //perror(("ERROR: No se pudo abrir el directorio: " + ruta).c_str());
        return false;
    }

    struct dirent* entrada;
    while ((entrada = readdir(dir)) != nullptr) {
        std::string nombre = entrada->d_name;

        if (nombre == "." || nombre == "..") continue;

        std::string rutaCompleta = ruta + "/" + nombre;

        struct stat info;
        if (stat(rutaCompleta.c_str(), &info) != 0) {
            //perror(("ERROR: No se pudo acceder a " + rutaCompleta).c_str());
            closedir(dir);
            return false;
        }

        if (S_ISDIR(info.st_mode)) {
            // Recursivo para subdirectorios
            if (!RecorrerDirectorio(rutaCompleta)) {
                closedir(dir);
                return false;
            }
        } else if (S_ISREG(info.st_mode)) {
            // Verificar que no sea un archivo .tk
            if (nombre.size() >= 3 && nombre.substr(nombre.size() - 3) == ".tk") {
                continue; // Ignorar archivos temporales .tk
            }

            // Es un archivo regular, intentamos indexarlo
            if (!IndexarDocumento(rutaCompleta)) {
                //cerr << "ERROR: No se pudo indexar el documento: " << rutaCompleta << endl;
                closedir(dir);
                return false;
            }
        }
    }

    closedir(dir);
    return true;
}

bool IndexadorHash::GuardarIndexacion() const {
    ofstream outFile(directorioIndice);
    if (!outFile) return false;

    // Guardar índice de términos
    outFile << indice.size() << '\n';
    for (const auto& entry : indice)
        outFile << entry.first << '\t' << entry.second << '\n';
    
    // Índice de documentos
    outFile << indiceDocs.size() << '\n';
    for (const auto& entry : indiceDocs)
        outFile << entry.first << '\t' << entry.second << '\t' << entry.second.obtenerFechaModificacion() << '\n';
    
    // Info colección
    outFile << informacionColeccionDocs << '\n';
    outFile << pregunta << '\n';
    
    // Índice de términos de la pregunta
    outFile << indicePregunta.size() << '\n';
    for (const auto& entry : indicePregunta)
        outFile << entry.first << '\t' << entry.second << '\n';

    // Info extendida + config
    outFile << infPregunta << '\n'
            << tok.DelimitadoresPalabra() << '\n'
            << (tok.CasosEspeciales() ? "true" : "false") << '\n'
            << (tok.PasarAminuscSinAcentos() ? "true" : "false") << '\n'
            << (almacenarPosTerm ? "true" : "false") << '\n'
            << tipoStemmer << '\n'
            << directorioIndice << '\n'
            << ficheroStopWords << '\n';

    outFile.close();
    return true;
}

bool IndexadorHash::RecuperarIndexacion(const string &directorioIndexacion) {
    ifstream inFile(directorioIndexacion);
    if (!inFile) return false;

    string palabra, line;
    int cantidad;

    // Índice términos
    inFile >> cantidad;
    inFile.ignore();  // Limpia el \n
    for (int i = 0; i < cantidad; ++i) {
        getline(inFile, line);
        stringstream ss(line);
        InformacionTermino info;
        ss >> palabra >> info;
        indice[palabra] = info;
    }

    // Índice documentos
    inFile >> cantidad;
    inFile.ignore();
    for (int i = 0; i < cantidad; ++i) {
        getline(inFile, line);
        stringstream ss(line);
        string docId, fecha;
        InfDoc infoDoc;
        ss >> docId >> infoDoc;
        indiceDocs[docId] = infoDoc;
    }

    // Info colección
    inFile >> informacionColeccionDocs;
    inFile.ignore();

    // Pregunta
    getline(inFile, pregunta);
    //cout << pregunta << endl;

    // Índice pregunta
    inFile >> cantidad;
    //cout << cantidad << endl;
    inFile.ignore();
    for (int i = 0; i < cantidad; ++i) {
        getline(inFile, line);
        stringstream ss(line);
        InformacionTerminoPregunta info;
        ss >> palabra >> info;
        indicePregunta[palabra] = info;
    }

    // Info extendida
    inFile >> infPregunta;
    inFile.ignore();

    // Configuración
    string delimitadores, casosEsp, minusculas;

    // Leer configuraciones del tokenizador
    getline(inFile, delimitadores);
    getline(inFile, casosEsp);
    getline(inFile, minusculas);
    
    bool tieneCasosEspeciales = (casosEsp == "true");
    bool convertirMinusculasSinAcentos = (minusculas == "true");
    
    // Crear tokenizador
    tok = Tokenizador(delimitadores, tieneCasosEspeciales, convertirMinusculasSinAcentos);

    string almacenarPosTermStr;
    getline(inFile, almacenarPosTermStr);
    almacenarPosTerm = (almacenarPosTermStr == "true");

    inFile >> tipoStemmer;
    inFile >> directorioIndice;
    inFile >> ficheroStopWords;
    //cout << ficheroStopWords << endl;

    inFile.close();
    return true;
}


void IndexadorHash::ImprimirIndexacion() const {
    cout << "Términos indexados: " << endl;
    for (const auto& term : indice) {
        cout << term.first << "\t" << term.second << endl;
    }
    cout << "Documentos indexados: " << endl;
    for (const auto& doc : indiceDocs) {
        cout << doc.first << "\t" << doc.second << endl;
    }
}

bool IndexadorHash::IndexarPregunta(const string &preg){
    int index=0;
    list<string> lt1;
    list<string>::iterator itS;
    
    indicePregunta.clear();
    infPregunta.~InformacionPregunta();
    pregunta=preg;

    tok.Tokenizar(pregunta, lt1);
    string term;
    string termino;

    for(list<string>::const_iterator it= lt1.begin();it!=lt1.end();it++){
        term=(*it);

        termino = term;
        transform(termino.begin(), termino.end(), termino.begin(), ::tolower);
        if (tipoStemmer != 0) {
            stemmerPorter stemmer;
            stemmer.stemmer(termino, tipoStemmer);
        }
        
        if(stopWords.find(termino) == stopWords.end()){
            //cout << "entrado en stopWords" << endl;
	    	infPregunta.incrementarNumPalSinParada();
	    	unordered_map<string, InformacionTerminoPregunta>::iterator termino_it = indicePregunta.find(termino);

	    	if(termino_it != indicePregunta.end()) {
	    	  termino_it->second.incrementarFT();
	    	  termino_it->second.agregarPosTerm(index);
	    	}
	    	else {
	    	  InformacionTerminoPregunta itp;
	    	  itp.incrementarFT();
	    	  itp.agregarPosTerm(index);
	    	  indicePregunta.insert({termino, itp});
              infPregunta.incrementarNumPalDiferentes();
	    	}
	    }
        index++;
        infPregunta.incrementarNumPal();
    }
    return true;
}

bool IndexadorHash::DevuelvePregunta(string &preg) const{
    if (pregunta.empty()) {
        return false;
    }
    preg = pregunta;
    return true;
}

bool IndexadorHash::DevuelvePregunta(const string &word, InformacionTerminoPregunta &inf) const{
    string termino = word;
    transform(termino.begin(), termino.end(), termino.begin(), ::tolower);
    if (tipoStemmer != 0) {
        stemmerPorter stemmer;
        stemmer.stemmer(termino, tipoStemmer);
    }

    auto it = indicePregunta.find(termino);
    if (it != indicePregunta.end()) {
        inf = it->second;
        return true;
    }
    return false;
}

bool IndexadorHash::DevuelvePregunta(InformacionPregunta &inf) const{
    if (pregunta.empty()) {
        return false;
    }
    inf = infPregunta;
    return true;
}

void IndexadorHash::ImprimirIndexacionPregunta() {
    cout << "Términos indexados en la pregunta: " << endl;
    for (const auto& term : indicePregunta) {
        cout << term.first << "\t" << term.second << endl;
    }
    cout << "Información de la pregunta: " << endl;
    cout << infPregunta << endl;
}

void IndexadorHash::ImprimirPregunta() {
    cout << "Pregunta indexada: " << pregunta << endl;
    cout << "Información de la pregunta: " << infPregunta << endl;
}

bool IndexadorHash::Devuelve(const string &word, InformacionTermino &inf) const {
    string termino = word;
    if (tipoStemmer != 0) {
        stemmerPorter stemmer;
        stemmer.stemmer(termino, tipoStemmer);
    }

    auto it = indice.find(termino);
    if (it != indice.end()) {
        inf = it->second;
        return true;
    }
    return false;
}

bool IndexadorHash::Devuelve(const string &word, const string &nomDoc, InfTermDoc &InfDoc) const{
    string termino = word;
    if (tipoStemmer != 0) {
        stemmerPorter stemmer;
        stemmer.stemmer(termino, tipoStemmer);
    }

    auto it = indice.find(termino);
    if (it != indice.end()) {
        auto itDoc = indiceDocs.find(nomDoc);
        if (itDoc != indiceDocs.end()) {
            auto& infoTermino = it->second;
            auto& infoTermDoc = infoTermino.obtenerInfTermDoc(itDoc->second.getIDoc());
            InfDoc = infoTermDoc;
            return true;
        }
    }
    return false;
}

bool IndexadorHash::Existe(const string &word) const{
    string termino = word;
    if (tipoStemmer != 0) {
        stemmerPorter stemmer;
        stemmer.stemmer(termino, tipoStemmer);
    }
    auto it = indice.find(termino);
    return it != indice.end();
}

bool IndexadorHash::BorraDoc(const string &nomDoc){
    auto itDoc = indiceDocs.find(nomDoc);
    if (itDoc != indiceDocs.end()) {
        // Eliminar los términos asociados al documento del índice
        for (auto it = indice.begin(); it != indice.end();) {
            it->second.eliminarDocumento(itDoc->second.getIDoc());
            if (it->second.getFTC() == 0) {
                it = indice.erase(it);  // Eliminar términos con frecuencia total 0
            }
            else {
                ++it;
            }
        }

        // Actualizar la información de la colección de documentos
        informacionColeccionDocs.borrarDoc(itDoc->second);
        informacionColeccionDocs.setNumPalDiferentes(indice.size());
        
        // Eliminar el documento del índice de documentos
        indiceDocs.erase(itDoc);

        return true;
    }
    return false;
}

void IndexadorHash::VaciarIndiceDocs(){
    indiceDocs.clear();
    indice.clear();
    informacionColeccionDocs = InfColeccionDocs();
}

void IndexadorHash::VaciarIndicePreg() {
    indicePregunta.clear();
    indice.clear();
    pregunta = "";
    infPregunta = InformacionPregunta();
}

int IndexadorHash::NumPalIndexadas() const {
    return indice.size();
}

string IndexadorHash::DevolverFichPalParada() const {
    return ficheroStopWords;
}

void IndexadorHash::ListarPalParada() const {
    for (const auto& word : stopWords_sinStemming) {
        cout << word << endl;
    }
}

int IndexadorHash::NumPalParada() const {
    return stopWords.size();
}

string IndexadorHash::DevolverDelimitadores() const {
    return tok.DelimitadoresPalabra();
}

bool IndexadorHash::DevolverCasosEspeciales() const {
    return tok.CasosEspeciales();
}

bool IndexadorHash::DevolverPasarAminuscSinAcentos() const {
    return tok.PasarAminuscSinAcentos();
}

bool IndexadorHash::DevolverAlmacenarPosTerm() const {
    return almacenarPosTerm;
}

string IndexadorHash::DevolverDirIndice() const {
    return directorioIndice;
}

int IndexadorHash::DevolverTipoStemming() const {
    return tipoStemmer;
}

void IndexadorHash::ListarInfColeccDocs() const
{
    cout << informacionColeccionDocs << endl;
}

void IndexadorHash::ListarTerminos() const {
    cout << "Términos indexados: " << endl;
    for (const auto& term : indice) {
        cout << term.first << "\t" << term.second << endl;
    }
}

bool IndexadorHash::ListarTerminos(const string& nomDoc) const {
    auto it = indiceDocs.find(nomDoc);
    if (it != indiceDocs.end()) {
        cout << "Términos indexados en el documento " << nomDoc << ": " << endl;
        for (const auto& term : indice) {
            auto itTerm = term.second.obtenerInfTermDoc(it->second.getIDoc());
            if (itTerm.getFT() > 0) {
                cout << term.first << "\t" << itTerm << endl;
            }
        }
        return true;
    }
    return false;
}

void IndexadorHash::ListarDocs() const {
    cout << "Documentos indexados: " << endl;
    for (const auto& doc : indiceDocs) {
        cout << doc.first << "\t" << doc.second << endl;
    }
}

bool IndexadorHash::ListarDocs(const string& nomDoc) const {
    auto it = indiceDocs.find(nomDoc);
    if (it != indiceDocs.end()) {
        cout << nomDoc << "\t" << it->second << endl;
        return true;
    }
    return false;
}
