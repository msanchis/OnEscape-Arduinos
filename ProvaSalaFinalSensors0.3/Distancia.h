/*
  Distancia.h - Libreria para almacenar distancias y devolver la media.
  Created by Miquel Sanchis. 14/12/2019 para 
  ONESCAPE 2019, S.L.
*/
#ifndef Distancia_h
#define Distancia_h
 
#include "Arduino.h"

class Distancia {
  private:
  const static int NUM = 5; //Numero de medidas que se almacenan
  long var1[NUM]; //Array donde se almacenan las medidas
  int index=0;
  int tamano();  
  int numElementos=0; //Numero de inserciones realizadas inicialmente

  public:
  void insertar(long a);
  bool valida();
  long media();
  void inicializa();
};

#endif
