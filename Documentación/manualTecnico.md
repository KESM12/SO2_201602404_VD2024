# Manual Técnico.

## Error 1.
```bash
root@kevin-VirtualBox:/home/kevin/Desktop/linux-6.8# make clean
/home/kevin/Desktop/linux-6.8/Makefile:1019: scripts/Makefile.extrawarn: No such file or directory
make[1]: *** No rule to make target 'scripts/Makefile.extrawarn'.  Stop.
make: *** [Makefile:240: __sub-make] Error 2
```

Dicho error indica que no se reconoce el archivo Makefile.extrawarn para solucionarlo se reviso el symlink hacia la carpeta correcta y efectivamente ese era el error.

## Error 2.

```bash
make[2]: *** No rule to make target 'arch/x86/entry/syscalls/syscall_64.tbl', needed by 'arch/x86/include/generated/uapi/asm/unistd_64.h'.  Stop.
make[1]: *** [arch/x86/Makefile:249: archheaders] Error 2
make: *** [Makefile:240: __sub-make] Error 2
```

Este error indica que no se encuentra el archivo syscall_64.tbl necesario para generar unistd_64.h. Para solucionarlo, se debe verificar la existencia del archivo y su ruta correcta se volvio a validar la configuración symlink.

## Error 3.
```bash
make -j$(nproc)
mkdir -p /home/taro/Desktop/linux-6.8/tools/objtool && make O=/home/taro/Desktop/linux-6.8 subdir=tools/objtool --no-print-directory -C objtool 
    INSTALL libsubcmd_headers
    CALL    scripts/checksyscalls.sh
    UPD     include/generated/utsversion.h
    CC      init/version-timestamp.o
    LD      .tmp_vmlinux.kallsyms1
ld: arch/x86/entry/syscall_64.o:(.rodata+0x1120): undefined reference to __x64_sys_capture_memory_snapshot'
make[2]: *** [scripts/Makefile.vmlinux:37: vmlinux] Error 1
make[1]: *** [/home/taro/Desktop/linux-6.8/Makefile:1162: vmlinux] Error 2
make: *** [Makefile:240: __sub-make] Error 2
```
Este error indica que hay una referencia indefinida a `__x64_sys_capture_memory_snapshot`. Para solucionarlo, se debe verificar la implementación de la función `__x64_sys_capture_memory_snapshot` y asegurarse de que esté correctamente definida y enlazada en el archivo correspondiente. Se corrigio agregando como command la `sys_capture_memory_snapshot` al archivo de syscalls_64.tbl.

## Error 4.

```bash
make[1]: *** [/home/taro/Desktop/linux-6.8/Makefile:1921: .] Error 2
make: *** [Makefile:240: __sub-make] Error 2
```

Este error lo tuve muchas veces, pero era por la importación para una de las syscalls a desarrollar.

## Error 5.
```bash
make[3]: *** No rule to make target 'debian/canonical-certs.pem', needed by 'certs/x509_certificate_list'.  Stop.
make[3]: *** Waiting for unfinished jobs....
make[2]: *** [scripts/Makefile.build:481: certs] Error 2
make[2]: *** Waiting for unfinished jobs....
make[1]: *** [/home/taro/Desktop/linux-6.8/Makefile:1921: .] Error 2
make: *** [Makefile:240: __sub-make] Error 2
```
Este error indica que no se encuentra el archivo `canonical-certs.pem` necesario para generar `x509_certificate_list`. Para solucionarlo, se debe verificar la existencia del archivo y su ruta correcta, o bien, generar el archivo `canonical-certs.pem` si no existe, esto sucedido debido a que borramos unos certificados y debian de volverse a generar pero estabamos compilando de forma incorrecta.


# Cronograma/Bitacora.

### Dia 1: Compilación y modificación básica del kernel.
08/12/2024 Configuración de kernel, errores con symlinks y consulta al auxiliar del curso, lectura de documentación.

### Dia 2: Desarrollo de módulos personalizados del kernel.
09/12/2024 y 10/12/2024 Desarrollo y compilación del modulo que devuelve información de la ram.

### Dia 3-5: Creación de las syscalls en el kernel
11/12/2024 y 13/12/24 Lectura de documentación, afinación de detalles para el desarrollo 
de la syscall que devolviera los valores solicitados para la ram.

### Dia 6: Pruebas y ajustes de la solución.
15/12/2024 Desarrollo de modulos y terminación de detalles externos.

### Dia 7: Documentación final y presentación del proyecto
16/12/2024 Escritura de manuales técnico y de usuario.
