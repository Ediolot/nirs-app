# Notes

## Problemas
* Identificar el formato en el que se guardan los experimentos, fue necesario usar tambi�n una herramienta del tipo hex editor para una mejor inspección (¿Qué programa fue?)
* El experimento1 está corrupto
* Ha sido necesaria crear la clase frame. Han habido complicaciones con las matrices Eigen y los tipos de datos de los templates que se han solucionado creado esa clase
* Webengine resize slow [https://bugreports.qt.io/browse/QTBUG-58324](https://bugreports.qt.io/browse/QTBUG-58324)
* Mantener todas las imágenes puede requerir que la aplicación use +2GB, así que se ha compilado una app de 64bits
* La función toIndexed8 tiene que devolver el array en base64 para que poder leero en el javascript. Debe de haber una forma de hacerlo sin necesidad de convertir a base64. Además, se pierde la información.
* Siguen habiendo problemas con el tema de las unidades del frame
* QVariantList es la solución para enviar números arbitrarios a través del canal

## Preocupaciones
* ¿Podría existir algún problema al leer los datos con little endian y big endian?
* ¿Las excepciones heredan de std::exception o std::runtime mejor?
* Hay que hacer un wait() de los TaskLauncher antes de acabar el programa

## Misc
* El tiempo de respuesta (latencia) desde que el js llama a una funci�n hasta que se devuelve el resultado es muy bajo (<1ms)
* Usar punteros en las variables de clase es mejor en detereminadas sutuaciones porque la memoria se asigna en el heap.
 * Si el objeto necesita existir por la duración de la función llamante, no usar punteros (aka es almacenado en el stack)
 * Si no, usar punteros (aka heap)
* My opinion is that the biggest advantage of the QTL is that it has the same implementation (including binary compatibility) on all OSes supported by Qt. Some STL implementations might be below par when it comes to performance or they might be missing functionality. Some platforms don’t even have an STL! On the other hand, the STL is more customizable and is available in its entirety in header files… Like I said, there is no clear winner. [4] [3]
* https://cryptii.com
* https://renenyffenegger.ch/notes/development/Base64/Encoding-and-decoding-base-64-with-cpp
* Los datos en memoria están guardados como ColMajor. Formato que debe coincidir con DataMatrix en frame.h
* Hablar sobre poder hacer debugging en una pestaña de Chrome (localhost:8080)
* MTuner
* Color picker
* https://www.cssmatic.com/es/box-shadow
* https://github.com/vinniefalco/DSPFiltersDemo
* http://paletton.com/#uid=35y0H0kyAuFkVCvqBz7AnolGfi+
* https://ionicons.com/
* https://www.canva.com/color-palette/
* http://dygraphs.com/options.html
* https://www.flaticon.com/free-icon/creativity_908920#term=brain&page=4&position=30

* Mendeley => Gestor de referencias
* Refworks => Gestor de referencias
* Google academico => Citas

## TODOs

* Indicador de color en las imágenes
* Settings
* Exportar datos de gráfica
* Cargar basal de archivo
* Guardar imágenes
* Funcionalidad de las checkbox
* HP LP BP BS interfaces
* ¿HSat no se usa en nada para calcular las medias?
* ¿Cuál es HbR y HbO?
* ¿Cuál se puede invertir? HbR
* ROI
* Smooth images
* Hhb => HbR
* OxHb => HbO

* (NO) Selector idioma
* (NO) DRIFTER

* ¿No usar Mapper?
* Frame superclass
* Mutex para el read experiment (trylock en otras funcoines) (o flag booleana)
* Problema con las fuentes de iconos desde archivo local
* (DONE) Poder elegir la imagen de saturación con el número
* (DONE) Tamaño de imágenes dinámico
* (DONE) Generar basal en una tarea separada
* (DONE) Mapa de color ¿correcto?
* (DONE) Animación cambio pestañas
* (DONE) Enviar datos de imagen con QVariantList en vez de en base64
* (DONE) Multitask al generar el gráfico

## Bibliografía
* [1] Documentación QT
* Documentación Bootstrap
* Documentación C++
* Documentación JQuery y JS/HTML/CSS
* [2] [https://stackoverflow.com/questions/18745839/when-do-i-use-pointers-in-qt](https://stackoverflow.com/questions/18745839/when-do-i-use-pointers-in-qt)
* [3] [https://stackoverflow.com/questions/5623218/why-use-qvectorqt-instead-of-stdvector](https://stackoverflow.com/questions/5623218/why-use-qvectorqt-instead-of-stdvector)
* [4] [https://web.archive.org/web/20160902015144/http://blog.codeimproved.net/posts/qtl-stl.html](https://web.archive.org/web/20160902015144/http://blog.codeimproved.net/posts/qtl-stl.html)
* [5] [https://stackoverflow.com/questions/27670401/using-jquery-this-with-es6-arrow-functions-lexical-this-binding](https://stackoverflow.com/questions/27670401/using-jquery-this-with-es6-arrow-functions-lexical-this-binding)
* https://www.kdab.com/new-qt-5-10-qthreadcreate/
* https://medium.com/genymobile/how-c-lambda-expressions-can-improve-your-qt-code-8cd524f4ed9f
* https://stackoverflow.com/questions/41965425/qt-display-qimage-or-pixel-map-in-qtwebengine-view-from-c-to-html?rq=1
