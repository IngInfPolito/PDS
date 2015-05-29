#include "Paragrafo.h"

Paragrafo::Paragrafo(std::string testo) : testo(testo) { }

const std::string& Paragrafo::getTesto() {
	return testo;
}

std::string Paragrafo::trova(std::string s) {
	size_t start = testo.find(s);
	if (start != std::string::npos) {
		return "/" + std::to_string(start + 1) + ":" + std::to_string(start + s.size());
	}
	return std::string("");
}
