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

double Buscador::CalcularScoreDocumento(int idDoc, const map<string, double>& pesosPregunta,
                                        const map<string, InformacionTermino>& indiceTemporal) {
    double scoreTotal = 0.0;
    for (const auto& terminoQuery : pesosPregunta) {
        auto it = indiceTemporal.find(terminoQuery.first);
        if (it == indiceTemporal.end()) continue;

        const auto& infoTerm = it->second;
        if (infoTerm.getL_docs().count(idDoc) == 0) continue;

        double score = (formSimilitud == 0) ?
            CalcularDFR(terminoQuery.first, idDoc, pesosPregunta) :
            CalcularBM25(terminoQuery.first, idDoc, pesosPregunta);

        scoreTotal += score;
    }
    return scoreTotal;
}

bool Buscador::Buscar(const int& numDocumentos) {
    docsOrdenados = priority_queue<ResultadoRI>();

    string pregunta;
    if (!DevuelvePregunta(pregunta)) {
        cerr << "ERROR: No hay ninguna pregunta indexada." << endl;
        return false;
    }

    map<string, double> pesosPregunta;
    if (!CalcularPesosPregunta(pesosPregunta) || pesosPregunta.empty()) {
        cerr << "ERROR: Pregunta inválida o sin términos significativos." << endl;
        return false;
    }

    unordered_set<int> docsProcesados;
    map<string, InformacionTermino> indiceTemporal;

    for (const auto& termino : pesosPregunta) {
        InformacionTermino info;
        if (Devuelve(termino.first, info)) {
            indiceTemporal[termino.first] = info;
        }
    }

    for (const auto& par : indiceTemporal) {
        const string& termino = par.first;
        const InformacionTermino& infoTerm = par.second;

        for (const auto& docPar : infoTerm.getL_docs()) {
            int idDoc = docPar.first;
            if (!docsProcesados.insert(idDoc).second) continue;

            double scoreTotal = CalcularScoreDocumento(idDoc, pesosPregunta, indiceTemporal);

            if (docsOrdenados.size() < numDocumentos) {
                docsOrdenados.push(ResultadoRI(scoreTotal, idDoc, 0));
            } else if (scoreTotal > docsOrdenados.top().VSimilitud()) {
                docsOrdenados.pop();
                docsOrdenados.push(ResultadoRI(scoreTotal, idDoc, 0));
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
            cerr << "ERROR: No se puede abrir el archivo: " << ss.str() << endl;
            continue;
        }

        string preguntaTexto((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
        IndexarPregunta(preguntaTexto);

        map<string, double> pesosPregunta;
        if (!CalcularPesosPregunta(pesosPregunta) || pesosPregunta.empty()) continue;

        unordered_set<int> docsProcesados;
        map<string, InformacionTermino> indiceTemporal;

        for (const auto& termino : pesosPregunta) {
            InformacionTermino info;
            if (Devuelve(termino.first, info)) {
                indiceTemporal[termino.first] = info;
            }
        }

        for (const auto& par : indiceTemporal) {
            const string& termino = par.first;
            const InformacionTermino& infoTerm = par.second;

            for (const auto& docPar : infoTerm.getL_docs()) {
                int idDoc = docPar.first;
                if (!docsProcesados.insert(idDoc).second) continue;

                double score = CalcularScoreDocumento(idDoc, pesosPregunta, indiceTemporal);

                if (docsOrdenados.size() < numDocumentos) {
                    docsOrdenados.push(ResultadoRI(score, idDoc, i));
                } else if (score > docsOrdenados.top().VSimilitud()) {
                    docsOrdenados.pop();
                    docsOrdenados.push(ResultadoRI(score, idDoc, i));
                }
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

    int k = 0;
    // Número de términos en la pregunta
    for (const auto& par : indicePregunta) {
        k += par.second.getFT();
    }

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
    int nt = infTerm.getL_docs().size();
    double lambda = static_cast<double>(ft) / N;
    int ld = it->second.getNumPalSinParada();
    double avgdl = static_cast<double>(informacionColeccionDocs.getNumTotalPalSinParada()) / N;
    int ftd = docInfo->second.getFT();

    double ftd_star = ftd * log2(1 + ((c * avgdl) / ld));

    double wtd = (log2(1 + lambda) + ftd_star * log2((1+lambda) / lambda)) * ((ft + 1) / (nt * (ftd_star + 1)));

    double pesoPregunta = pesosPregunta.at(termino);

    /*
    cout << "[DFR] Documento: " << idDoc
         << " | Termino: " << termino 
         << " | ftd: " << ftd 
         << " | ft: " << ft 
         << " | N: " << N 
         << " | lambda: " << lambda 
         << " | ld: " << ld 
         << " | avgdl: " << avgdl 
         << " | pesoPregunta: " << pesoPregunta 
         << " | Resultado: " << pesoPregunta * wtd << endl;
    */
    return pesoPregunta * wtd;
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

    int N = informacionColeccionDocs.getNumDocs();
    int nq = infTerm.getL_docs().size();
    // Cálculo de IDF
    double idf = log2((N - nq + 0.5) / (nq + 0.5));

    int ftd = docInfo->second.getFT();
    int d = it->second.getNumPalSinParada();
    double avgdl = static_cast<double>(informacionColeccionDocs.getNumTotalPalSinParada()) / N;
    
    // Fórmula BM25
    double numerador = ftd * (k1 + 1);
    double denominador = ftd + k1 * (1 - b + b * (d / avgdl));
    double frac = numerador / denominador;

    /*
    cout << "[BM25] Documento: " << idDoc
         << " | Termino: " << termino
         << " | ftd: " << ftd 
         << " | nq: " << nq 
         << " | N: " << N 
         << " | idf: " << idf 
         << " | frac: " << frac 
         << " | Resultado: " << idf * frac << endl;

    */
    return idf * frac;
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
