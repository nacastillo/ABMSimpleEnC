# Ejemplo ABM en C

Ejemplo simple tipo ABM para la materia Estructura y Base de Datos, del Instituto Nacional Superior del Profesorado Técnico - Universidad Tecnológica Nacional.  
El mismo está construido en el lenguaje C, para ser compilado para Win32 más que nada por algunos llamados a _system_. Para armar el ejecutable se utilizó Codeblocks.
El mismo toma un archivo .csv de una tabla de MySQL (en este caso, como ejemplo, la tabla _Ingresantes.csv_), y lo transforma en un archivo binario para poder hacer altas, bajas y modificaciones a cada registro en dicho archivo binario. 

## Pendientes
- Modularizar y refactorizar funciones para reutilizar código (como las de ordenamiento o las de ABM)
- Combinar las funcionalidades de _baja_ y _modificación_ en una funcionalidad _buscar_