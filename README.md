# Inteligencia Artificial Embebida

## 4ta Cohorte 2021

Autor

* Pablo C. Zizzutti

Docentes

* Juan Esteban Carrique
* Matías Gerard
* Leonardo Giovanini

### Descripcion

## Actividad : LoopBack
Se desarrolló un loopback entre script de python y la placa Nucleo F746ZG.

### Caracteristicas tecnicas utilizadas

* Hardware: Nucleo F746ZG

* Nombre del SO : Ubuntu 20.04.5 LTS
* Tipo de OS: 64 bits

* Python -> Visual Studio Code 
* C -> Visual Studio Code 

## Actividad : Puerta de Ruido

* Realizar un script de python que cargue un archivo de sonido y genere como salida varios otros archivos de sonido aplicando una funcion compuerta de ruido[1] con los siguientes parametros configurables:
    *  Umbral de apertura: Es el valor que debe superarse para que se abra la compuerta.
    * Umbral de cierre: Es el valor que debe alcanzarse para volver a cerrarse
    * “hold”: Es el tiempo que permanece abierta la compuerta una vez alcanzado el umbral de cierre (si durante este periodo la señal quedara entre ambos umbrales, la compuerta permanecera abierta).
* Implementar la compuerta de ruido en C como embebido en el lazo.