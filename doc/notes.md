# Notes

## Problemas
* Identificar el formato en el que se guardan los experimentos, fue necesario usar tambi�n una herramienta del tipo hex editor para una mejor inspección
* El experimento1 está corrupto
* Ha sido necesaria crear la clase frame. Han habido complicaciones con las matrices Eigen y los tipos de datos de los templates que se han solucionado creado esa clase
* Webengine resize slow [https://bugreports.qt.io/browse/QTBUG-58324](https://bugreports.qt.io/browse/QTBUG-58324)
* Mantener todas las imágenes puede requerir que la aplicación use +2GB, así que se ha compilado una app de 64bits


## Preocupaciones
* ¿Podría existir algún problema al leer los datos con little endian y big endian?


## Misc
* El tiempo de respuesta (latencia) desde que el js llama a una funci�n hasta que se devuelve el resultado es muy bajo (<1ms)
* Usar punteros en las variables de clase es mejor en detereminadas sutuaciones porque la memoria se asigna en el heap.
 * Si el objeto necesita existir por la duración de la función llamante, no usar punteros (aka es almacenado en el stack)
 * Si no, usar punteros (aka heap)
* My opinion is that the biggest advantage of the QTL is that it has the same implementation (including binary compatibility) on all OSes supported by Qt. Some STL implementations might be below par when it comes to performance or they might be missing functionality. Some platforms don’t even have an STL! On the other hand, the STL is more customizable and is available in its entirety in header files… Like I said, there is no clear winner. [4] [3]

## Bibliografía
* [1] Documentación QT
* [2] [https://stackoverflow.com/questions/18745839/when-do-i-use-pointers-in-qt](https://stackoverflow.com/questions/18745839/when-do-i-use-pointers-in-qt)
* [3] [https://stackoverflow.com/questions/5623218/why-use-qvectorqt-instead-of-stdvector](https://stackoverflow.com/questions/5623218/why-use-qvectorqt-instead-of-stdvector)
* [4] [https://web.archive.org/web/20160902015144/http://blog.codeimproved.net/posts/qtl-stl.html](https://web.archive.org/web/20160902015144/http://blog.codeimproved.net/posts/qtl-stl.html)
