/*
  Distancia.cpp - Libreria para almacenar distancias y devolver la media.
  Created by Miquel Sanchis. 14/12/2019 para 
  ONESCAPE 2019, S.L.
*/

#include "Arduino.h"
#include "Distancia.h"

void Distancia::insertar(long a) {
  var1[index] = a;
  index++;
  if ( index == NUM ) index=0;
  if ( numElementos < NUM ) numElementos++;
}

int Distancia::tamano(){  
  return sizeof(var1) / sizeof(var1[0]);
}

bool Distancia::valida() {
  if ( numElementos == NUM ) return true;
  else return false;
}

long Distancia::media() {
  if (valida()) {
    long suma=0;
    for (int i =0; i < NUM ; i++){
      suma+=var1[i];
    }
    return suma/NUM;
  }else return 0;
  
}

void Distancia::inicializa(){
  for (int i=0; i < NUM ; i++){
      var1[i]=-1000;
  }
}
