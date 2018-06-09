# Notes

## Problemas
* Identificar el formato en el que se guardan los experimentos, fue necesario usar tambi�n una herramienta del tipo hex editor para una mejor inspección
* El experimento1 está corrupto
* Ha sido necesaria crear la clase frame. Han habido complicaciones con las matrices Eigen y los tipos de datos de los templates que se han solucionado creado esa clase
* Webengine resize slow [https://bugreports.qt.io/browse/QTBUG-58324](https://bugreports.qt.io/browse/QTBUG-58324)
* Mantener todas las imágenes puede requerir que la aplicación use +2GB, así que se ha compilado una app de 64bits


## Misc
* El tiempo de respuesta (latencia) desde que el js llama a una funci�n hasta que se devuelve el resultado es muy bajo (<1ms)
* Usar punteros en las variables de clase es mejor en detereminadas sutuaciones porque la memoria se asigna en el heap.
 * Si el objeto necesita existir por la duración de la función llamante, no usar punteros (aka es almacenado en el stack)
 * Si no, usar punteros (aka heap)

## Bibliografía
* Documentación QT
* [https://stackoverflow.com/questions/18745839/when-do-i-use-pointers-in-qt](https://stackoverflow.com/questions/18745839/when-do-i-use-pointers-in-qt)
