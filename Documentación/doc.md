# Manual de Usuario

## Introducción
Comprender y manipular las funcionalidades internas del kernel es clave para optimizar el rendimiento y saber como funcionan los SO. El objetivo de este proyecto fue desarrollar módulos del kernel Linux mint que proporcionen información del sistema, tales como estadísticas del cpu, el almacenamiento y la ram.

## Objetivos del Proyecto
- Desarrollar modulos del kernel de linux para recopilar estadísticas del sistema.
- Exponer la información del sistema de forma accesible.
- Documentar y validar el funcionamiento de los módulos.

## Configuración del Entorno
Para terminos del proyecto se utilizo un kernel (linux 6.8-1-usac1) modificado en base a un kernel base (linux 6.8), para validar esto se descargo el kernel desde el sitio oficial, se descomprimio y se siguieron los pasos proporcionados por el auxiliar para la instalación del nuevo kernel.

## Entorno configurado
![Configuración del Entorno](Documentación\images\kernel.png)
validación del kernel (configuración adjunta).

![Configuración de carpetas](Documentación\images\entorno1.png)
Estructura de las carpetas para el proyecto.

![Configuración de symlinks](Documentación\images\symlink.png)
Ejemplo de los symlinks para el manejo de la estructura del proyecto.

Entre los archivos que se utilizaron para el proyecto podemos mencionar:
    - syscall_64.tbl (Tabla donde se almacenan el registro de las syscalls)
    - main.c (Archivo donde se coloco el mensaje de bienvenida.)
    - open.c y read_write.c (Archivo donde se coloco el contador para la syscall2)
    - syscalls.h (Archivo donde se declararon las syscalls para sus structs)
    - uts.h (Archivo donde se modifico el nombre de quien modifico el kernel)
    - Kernel, usac, Makefile y sys.c (todos estos componentes fueron modificados para que las syscalls cumplieran su objetivo)

Cada uno de los archivos anteriores tuvieron su modificación la cual se detalla más adelante.

## Descripción de Modificaciones en el Kernel


## Detalles de las Nuevas Llamadas al Sistema
Descripción de las nuevas llamadas al sistema implementadas.

## Pruebas Realizadas
Explicación de las pruebas realizadas y sus resultados.

## Reflexión Personal
Reflexión sobre el desarrollo del proyecto y aprendizajes obtenidos.