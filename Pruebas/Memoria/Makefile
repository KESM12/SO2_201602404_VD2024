# Especifica el nombre del módulo
obj-m += memory_snapshot.o

# Ruta al código fuente del kernel
KDIR := /lib/modules/$(shell uname -r)/build

# Ruta del directorio actual
PWD := $(shell pwd)

# Regla para compilar el módulo
all:
	$(MAKE) -C $(KDIR) M=$(PWD) modules
	@rm -f *.o *.mod.c *.order *.symvers

# Limpieza de los archivos generados
clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean



