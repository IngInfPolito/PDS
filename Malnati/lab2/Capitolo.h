#ifndef __CAPITOLO_H
#define __CAPITOLO_H

#include "Contenitore.h"

class Capitolo : public Contenitore {
public:

	virtual Contenitore* aggiungi(Testo* t);

};

#endif
