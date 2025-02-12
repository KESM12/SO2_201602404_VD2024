# Configuración de kernel.

## Paso 1.
Instalar linux mint, ya sea de forma virtual o local, se recomienda fisica si la computadora no tiene demasiados nucleos.

## Paso 2.
Una vez instalado el sistema se comenzara con la configración del kernel a modificar ya que se busca que aun que se corropan un kernel el sistema no sea inutilizable, para esto debemos descargar un kernel nuevo:

```bash
wget https://www.kernel.org/pub/linux/kernel/v6.x/linux-6.8.tar.xz
```
ejecutando el comando anterior dentro de la terminal de mint obtendremos un .xz el cual es un archivo comprimodo donde se encuentra el kernel que modificaremos, para descomprimirlo hay varias formas pero en nuestro caso usaremos la terminal de mint.

```bash
tar -xf linux-6.8.tar.xz
```
Una vez descomprimido el archivo se nos genera una carpeta con el nombre `linux-6.8` teniendo ya esto en nuestra ubicación prefererida debemos realizar unas minimas modificaciones para distinguirla del kernel oficial, para esto iremos al archivo `Makefile` en el cual modificaremos la linea que tiene por encabezado `EXTRAVERSION` en nuestro caso agregamos `-49-usac1` con el objetivo de tener una distinción para el kernel que estaremos manejando, luego de realizar estos cambios debemos correr algunos comandos para actualizar el grub y que las configuraciones se efectuen sobre el sistema. 

Para que los cambios tengan efecto en el sistema operativo haremos lo siguiente:
1.- Instalaremos algunas dependecias/credenciales, ejecutando los siguientes comandos.

```bash
sudo apt-get install build-essential libncurses5-dev fakeroot wget bzip2 openssl
sudo apt-get install build-essential libncurses-dev bison flex libssl-dev libelf-dev
```

2.- Copiar el archivo de configuración desde el kernel principal.

```bash
cp -v /boot/config-$(uname -r) .config
```
3.- Actualizar el grub y reinicar.
```bash
sudo update-grub2
```

Realizando los pasos anterirores luego de que el sistema se haya reiniciado podemos validar si nuestra configuración surtio efecto ejecuatando el comando `uname -r` esto nos lanzara en consola un mensaje como este `linux-6.8-49-usac1`.

![Kernel Configuration](https://github.com/KESM12/SO2_201602404_VD2024/blob/main/Documentaci%C3%B3n/images/kernel.png)

## Paso 3.
Configuración de symlinks para evitar cargar todo el kernel al repositorio.
1.- Ubicar el archivo que se desea linkear. 
2.- Validar la ruta y crear todas las carpetas necesarias que hagan referencia dentro del kernel.
3.- En la terminal ejecutar el comando `ln -s rutadondeestaelarchivo rutadondeestaraelsymlink`

![Symlink Configuration](https://github.com/KESM12/SO2_201602404_VD2024/blob/main/Documentaci%C3%B3n/images/symlink.png)

## Paso 4.
En este paso ya sabemos como linkear archivo y carpetas (es el mismo proceso) desde nuestro repositorio a nuestro kernel entonces podemos avanzar con la modificación del kernel. 

Despues de todos los pasos anteriores cada vez que realicemos un cambio o que queramos compilar un cambio pues se recomienda utilizar el archivo ![Compilación](https://github.com/brianmatus/VD2024-so2-lab/blob/master/help%20scripts/compile_and_install.sh) para mayor facilidad, se recomienda leerlo para saber que opciones tomar pero en lo personal me salto el make clean unas 5 veces y luego le doy que si y enter a todas las demas opciones.
