#include "../include/buscador.h"
#include <cmath>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <algorithm>

using namespace std;

Buscador::Buscador(const string& directorioIndexacion, const int& f) : IndexadorHash(directorioIndexacion) {
    formSimilitud = f;
    c = 2;
    k1 = 1.2;
    b = 0.75;
}

Buscador::Buscador(const Buscador& busc) : IndexadorHash(busc) {
    formSimilitud = busc.formSimilitud;
    c = busc.c;
    k1 = busc.k1;
    b = busc.b;
}

Buscador::~Buscador() {}

Buscador& Buscador::operator=(const Buscador& busc) {
    if (this != &busc) {
        IndexadorHash::operator=(busc);
        formSimilitud = busc.formSimilitud;
        c = busc.c;
        k1 = busc.k1;
        b = busc.b;
    }
    return *this;
}

bool Buscador::Buscar(const int& numDocumentos) {
    // Limpiar la cola de resultados previos
    docsOrdenados = priority_queue<ResultadoRI>();

    // Paso 1: Verificar si hay una pregunta indexada con términos válidos
    string pregunta;
    if (!DevuelvePregunta(pregunta)) {
        cerr << "ERROR: No hay ninguna pregunta indexada." << endl;
        return false;
    }

    // Paso 2: Calcular los pesos de los términos de la pregunta
    map<string, double> pesosPregunta;
    if (!CalcularPesosPregunta(pesosPregunta)) {
        cerr << "ERROR: No se pudieron calcular los pesos de la pregunta." << endl;
        return false;
    }

    if (pesosPregunta.empty()) {
        cerr << "ERROR: La pregunta no contiene términos válidos (no de parada)." << endl;
        return false;
    }

    // Paso 3: Procesar documentos que comparten términos con la pregunta
    set<int> docsProcesados; // Para evitar procesar un documento más de una vez

    for (const auto& terminoPregunta : pesosPregunta) {
        const string& termino = terminoPregunta.first;
        InformacionTermino infoTermino;

        // Si el término no está en el índice, continuar con el siguiente
        if (!Devuelve(termino, infoTermino)) {
            cerr << "El término " << termino << " no está en el índice." << endl;
            continue;
        }

        // Procesar cada documento que contiene el término
        for (const auto& parDoc : infoTermino.getL_docs()) {
            int idDoc = parDoc.first;

            // Si ya procesamos este documento, saltar
            if (docsProcesados.count(idDoc) > 0) {
                continue;
            }
            docsProcesados.insert(idDoc);

            // Paso 4: Calcular similitud para el documento actual
            double scoreTotal = 0.0;

            for (const auto& terminoQuery : pesosPregunta) {
                const string& terminoDoc = terminoQuery.first;
                InformacionTermino infoTerminoDoc;

                if (!Devuelve(terminoDoc, infoTerminoDoc)) {
                    continue; // Término no encontrado en el índice
                }

                // Verificar si el documento contiene el término
                if (infoTerminoDoc.getL_docs().count(idDoc) == 0) {
                    continue;
                }

                // Calcular similitud según la fórmula especificada
                double scoreTermino = 0.0;
                scoreTermino = (formSimilitud == 0) ? CalcularDFR(terminoDoc, idDoc, pesosPregunta) : CalcularBM25(terminoDoc, idDoc, pesosPregunta);
                scoreTotal += scoreTermino;
            }

            // Paso 5: Almacenar solo los 'numDocumentos' más relevantes
            if (docsOrdenados.size() < numDocumentos) {
                docsOrdenados.push(ResultadoRI(scoreTotal, idDoc, 0));
            } else {
                // Si la cola está llena, reemplazar el peor score si el actual es mejor
                if (scoreTotal > docsOrdenados.top().VSimilitud()) {
                    docsOrdenados.pop();
                    docsOrdenados.push(ResultadoRI(scoreTotal, idDoc, 0));
                }
            }
        }
    }

    return true;
}

bool Buscador::Buscar(const string& dirPreguntas, const int& numDocumentos, const int& numPregInicio, const int& numPregFin) {
    docsOrdenados = priority_queue<ResultadoRI>();

    for (int i = numPregInicio; i <= numPregFin; ++i) {
        stringstream ss;
        ss << dirPreguntas << "/" << i << ".txt";
        ifstream file(ss.str());
        if (!file) {
            cerr << "ERROR: No se puede abrir el archivo de la pregunta: " << ss.str() << endl;
            continue;
        }

        string preguntaTexto((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
        IndexarPregunta(preguntaTexto);

        map<string, double> pesosPregunta;
        if (!CalcularPesosPregunta(pesosPregunta)) continue;

        set<int> docsProcesados;
        for (const auto& par : pesosPregunta) {
            InformacionTermino info;
            if (!Devuelve(par.first, info)) continue;

            for (const auto& doc : info.getL_docs()) {
                int idDoc = doc.first;
                if (docsProcesados.count(idDoc)) continue;
                docsProcesados.insert(idDoc);

                double score = 0.0;
                for (const auto& term : pesosPregunta) {
                    // Comprobar si el término está en el documento
                    InformacionTermino infoTerm;
                    if (!Devuelve(term.first, infoTerm)) continue;

                    if (infoTerm.getL_docs().count(idDoc) == 0) continue;

                    double s = (formSimilitud == 0) ? CalcularDFR(term.first, idDoc, pesosPregunta) : CalcularBM25(term.first, idDoc, pesosPregunta);
                    score += s;
                }
                docsOrdenados.push(ResultadoRI(score, idDoc, i));
            }
        }
    }
    return true;
}


void Buscador::ImprimirResultadoBusqueda(const int& numDocumentos) const {
    priority_queue<ResultadoRI> copia = docsOrdenados;
    int pos = 0;
    string preg;
    DevuelvePregunta(preg);

    while (!copia.empty() && pos < numDocumentos) {
        ResultadoRI res = copia.top();
        copia.pop();
        cout << res.NumPregunta() << " "
             << (formSimilitud == 0 ? "DFR" : "BM25") << " "
             << ExtraerNombreDoc(res.IdDoc()) << " "
             << pos << " "
             << fixed << setprecision(6) << res.VSimilitud() << " "
             << (res.NumPregunta() == 0 ? preg : "ConjuntoDePreguntas") << endl;
        pos++;
    }
}

bool Buscador::ImprimirResultadoBusqueda(const int& numDocumentos, const string& nombreFichero) const {
    ofstream out(nombreFichero);
    if (!out) return false; 

    priority_queue<ResultadoRI> copia = docsOrdenados;
    int pos = 0;
    string preg;
    DevuelvePregunta(preg);

    while (!copia.empty() && pos < numDocumentos) {
        ResultadoRI res = copia.top();
        copia.pop();
        out << res.NumPregunta() << " "
            << (formSimilitud == 0 ? "DFR" : "BM25") << " "
            << ExtraerNombreDoc(res.IdDoc()) << " "
            << pos << " "
            << fixed << setprecision(10) << res.VSimilitud() << " "
            << (res.NumPregunta() == 0 ? preg : "ConjuntoDePreguntas") << endl;
        pos++;
    }
    return true;
}

int Buscador::DevolverFormulaSimilitud() const {
    return formSimilitud;
}

bool Buscador::CambiarFormulaSimilitud(const int& f) {
    if (f == 0 || f == 1) {
        formSimilitud = f;
        return true;
    }
    return false;
}

void Buscador::CambiarParametrosDFR(const double& kc) {
    c = kc;
}

double Buscador::DevolverParametrosDFR() const {
    return c;
}

void Buscador::CambiarParametrosBM25(const double& kk1, const double& kb) {
    k1 = kk1;
    b = kb;
}

void Buscador::DevolverParametrosBM25(double& kk1, double& kb) const {
    kk1 = k1;
    kb = b;
}

bool Buscador::CalcularPesosPregunta(map<string, double>& pesos) const {
    pesos.clear();
    if (indicePregunta.empty()) return false;

    // Número de términos únicos (no de parada) en la pregunta
    int k = indicePregunta.size(); 

    for (const auto& par : indicePregunta) {
        double fiq = par.second.getFT(); // Frecuencia del término en la pregunta
        pesos[par.first] = fiq / k;     // w_iq = fiq / k (sin logaritmo)
    }

    return true;
}

double Buscador::CalcularDFR(const string& termino, int idDoc, const map<string, double>& pesosPregunta) const {
    InformacionTermino infTerm;
    if (!Devuelve(termino, infTerm)) {
        cout << "[ERROR DFR] No se encontró información para el término: " << termino << endl;
        return 0.0;
    }

    string nombreDoc = DevolverNombreDoc(idDoc);
    if (nombreDoc.empty()) {
        cout << "[ERROR DFR] No se encontró nombre para idDoc: " << idDoc << endl;
        return 0.0;
    }

    auto it = indiceDocs.find(nombreDoc);
    if (it == indiceDocs.end()) {
        cout << "[ERROR DFR] No se encontró el documento en indiceDocs para: " << nombreDoc << endl;
        return 0.0;
    }

    auto docInfo = infTerm.getL_docs().find(idDoc);
    if (docInfo == infTerm.getL_docs().end()) {
        cout << "[ERROR DFR] No existe frecuencia para término '" << termino << "' en idDoc " << idDoc << endl;
        cout << "Documentos con el término: ";
        for (const auto& d : infTerm.getL_docs()) cout << d.first << " ";
        cout << endl;
        return 0.0;
    }

    int ft = infTerm.getFTC();
    int N = informacionColeccionDocs.getNumDocs();
    double lambda = static_cast<double>(ft) / N;
    int ld = it->second.getNumPalSinParada();
    double avgdl = static_cast<double>(informacionColeccionDocs.getNumTotalPalSinParada()) / N;

    int ftd = docInfo->second.getFT();
    // Fórmula DFR corregida:
    double wd = ftd * log2(1 + (c * avgdl) / ld) * (ftd * log2(1 + lambda) + 0.5 * log2(2 * M_PI * ftd)) / (ftd + 1.0);

    double pesoPregunta = pesosPregunta.at(termino);

    cout << "[DFR] Termino: " << termino 
         << " | ftd: " << ftd 
         << " | ft: " << ft 
         << " | N: " << N 
         << " | lambda: " << lambda 
         << " | ld: " << ld 
         << " | avgdl: " << avgdl 
         << " | pesoPregunta: " << pesoPregunta 
         << " | Resultado: " << pesoPregunta * wd << endl;

    return pesoPregunta * wd;
}

double Buscador::CalcularBM25(const string& termino, int idDoc, const map<string, double>& pesosPregunta) const {
    InformacionTermino infTerm;
    if (!Devuelve(termino, infTerm)) {
        cout << "[ERROR BM25] No se encontró información para el término: " << termino << endl;
        return 0.0;
    }

    string nombreDoc = DevolverNombreDoc(idDoc);
    if (nombreDoc.empty()) {
        cout << "[ERROR BM25] No se encontró nombre para idDoc: " << idDoc << endl;
        return 0.0;
    }

    auto it = indiceDocs.find(nombreDoc);
    if (it == indiceDocs.end()) {
        cout << "[ERROR BM25] No se encontró el documento en indiceDocs para: " << nombreDoc << endl;
        return 0.0;
    }

    auto docInfo = infTerm.getL_docs().find(idDoc);
    if (docInfo == infTerm.getL_docs().end()) {
        cout << "[ERROR BM25] No existe frecuencia para término '" << termino << "' en idDoc " << idDoc << endl;
        cout << "Documentos con el término: ";
        for (const auto& d : infTerm.getL_docs()) cout << d.first << " ";
        cout << endl;
        return 0.0;
    }

    int ftd = docInfo->second.getFT();
    int N = informacionColeccionDocs.getNumDocs();
    int nq = infTerm.getL_docs().size();
    int dl = it->second.getNumPalSinParada();
    double avgdl = static_cast<double>(informacionColeccionDocs.getNumTotalPalSinParada()) / N;

    // Cálculo de IDF corregido (usando log natural como en la fórmula estándar)
    double idf = log((N - nq + 0.5) / (nq + 0.5) + 1.0);  // +1 para evitar negativos
    
    // Fórmula BM25 corregida
    double numerador = ftd * (k1 + 1);
    double denominador = ftd + k1 * (1 - b + b * (dl / avgdl));
    double frac = numerador / denominador;

    double pesoPregunta = pesosPregunta.at(termino);

    /*
    cout << "[BM25] Termino: " << termino 
         << " | ftd: " << ftd 
         << " | nq: " << nq 
         << " | N: " << N 
         << " | idf: " << idf 
         << " | frac: " << frac 
         << " | pesoPregunta: " << pesoPregunta 
         << " | Resultado: " << pesoPregunta * idf * frac << endl;

    */
    return pesoPregunta * idf * frac;
}


string Buscador::DevolverNombreDoc(int idDoc) const {
    for (const auto& par : indiceDocs) {
        if (par.second.getIDoc() == idDoc) return par.first;
    }
    return "";
}

string Buscador::ExtraerNombreDoc(int idDoc) const {
    string fullPath = DevolverNombreDoc(idDoc);
    size_t slash = fullPath.find_last_of("/");
    size_t dot = fullPath.find_last_of(".");
    if (slash == string::npos) slash = -1;
    if (dot == string::npos || dot < slash) dot = fullPath.length();
    return fullPath.substr(slash + 1, dot - slash - 1);
}

///////////////////////////////////
//////// ResultadoRI //////////////
///////////////////////////////////

ResultadoRI::ResultadoRI(const double& kvSimilitud, const long int& kidDoc, const int& np) {
    vSimilitud = kvSimilitud;
    idDoc = kidDoc;
    numPregunta = np;
}

double ResultadoRI::VSimilitud() const {
    return vSimilitud;
}

long int ResultadoRI::IdDoc() const {
    return idDoc;
}

int ResultadoRI::NumPregunta() const {
    return numPregunta;
}

bool ResultadoRI::operator< (const ResultadoRI& lhs) const {
    if (numPregunta == lhs.numPregunta)
        return vSimilitud < lhs.vSimilitud;
    else
        return numPregunta > lhs.numPregunta;
}

ostream& operator<<(ostream& os, const ResultadoRI& res) {
    os << res.vSimilitud << "\t\t" << res.idDoc << "\t" << res.numPregunta << endl;
    return os;
}
