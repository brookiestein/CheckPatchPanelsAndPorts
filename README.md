# CheckPatchPanelsAndPorts
Este programa me permite verificar las evidencias de Patch Panel y puertos de los mismos de los equipos que están conectados a los Patch Panels. Así mismo, me exporta la información que maneja en un Excel.

# FAQ
1. ¿Por qué verificar eso? Para evitar que, por alguna u otra razón, algún puerto esté referenciado por 2 o más tomas de datos.
2. ¿La lógica? Físicamente es imposible que en un puerto hayan 2 o más equipos conectados. Como analogía sería que es imposible conectar dos cargadores al puerto de carga de tu celular.
3. ¿Por qué pueden estar repetidas las tomas? Por simple deducción puedo decir que en ocasiones es, quizás, porque son muchas y los técnicos pueden perder el control de algunas,
   pero eso lo sabrán más ellos que yo, a mí me toca asegurarme de que no son agregadas a los documentos de evidencias como _buena y válida_ sin que así sea.
5. ¿Por qué exportarlo en un Excel? Porque debe haber evidencia de que esos puertos se verificaron y todo está bien, además el programa distingue con colores si en X puerto hay conectado un AP, una cámara o es una simple toma de datos.
   Este archivo de Excel es, a su vez, dejado en la carpeta de evidencias del proyecto, junto con las demás evidencias.
6. ¿Cómo las verifico? Identifico cuántos Patch Panels tiene el proyecto, cuánto puertos tiene cada Patch Panel (esa información me la pide el programa) y, mirando los fotos de evidencias de las tomas de datos o los APs, voy marcando
   cada uno de los _check boxes_ que el programa generó para el proyecto.
   Si no me topo con ninguna colisión, y todos los puertos fueron marcados, puedo concluir que está todo bien y lo único que me queda es exportar esa información a Excel y subir el archivo a la carpeta de evidencias del proyecto.
7. ¿Por qué hacer un programa para eso si pudiste haber usado Excel directamente? **Porque programar es divertido** y, aunque sea ahorrándome sólo unos minutos de vida **cuando estoy trabajando**, me gusta optimizar (como sinónimo de automatizar)
   mi trabajo y hacerlo más rápido si me es posible.
   Si no lo es, creo que la primera razón es más que suficiente para mí.
   Además empecé a hacerlo en Excel directamente ya que era lo más lógico, pero _reinventar lo rueda_ sólo por diversión no me parece un pecado capital, siempre y cuando no comprometa mi productividad. :P

# Dependencias
```
Qt6
libxlsxwriter
C++17 compiler
```

# Capturas de Pantalla
![](Screenshots/MainPage.png)
|:--:|
Página Principal
![](Screenshots/PP1.png)
Patch Panel 1 de Ejemplo
![](Screenshots/PP2.png)
Patch Panel 2 de Ejemplo
![](Screenshots/PP3.png)
Patch Panel 3 de Ejemplo
![](Screenshots/ConfirmPage.png)
Página de Confirmación
![](Screenshots/ConfirmMessage.png)
Mensaje de Confirmación
![](Screenshots/ExcelQuestion.png)
Pregunta para guardar los datos en una hoja de cálculo de Excel
![](Screenshots/ExportedExcel.png)
Datos Exportados a Excel
