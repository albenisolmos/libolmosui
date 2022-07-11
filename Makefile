VERSION = 0.5
CC = gcc
CFLAGS = -fPIC -Wall
LIBS = -lolmos `pkg-config --cflags --libs gtk+-3.0`
OBJPATH = ./build
SRCPATH = ./src
TARGET = olmosui
LIBTARGET = libolmosui.so
SOURCES = ol_app_list.c ol_window.c ol_box_expander.c
OBJECTS = ${SOURCES:.c=.o}
OBJFILES = $(addprefix ${OBJPATH}/,${OBJECTS})
HEADERS = $(addprefix ${SRCPATH}/,${SOURCES:.c=.h})

.SILENT:
.PHONY: clean prebuild

install: ${OBJPATH}/${LIBTARGET} ${HEADERS}
	cp -va $< /usr/lib/${LIBTARGET}
	mkdir -p /usr/include/${TARGET}
	cp -v ${HEADERS} /usr/include/${TARGET}

uninstall:
	rm -fv /usr/lib/${LIBTARGET}
	rm -frv /usr/include/olmosui

build: prebuild ${OBJECTS}
	@echo "Archiving library: ${LIBTARGET}"
	${CC} -shared -o ${OBJPATH}/${LIBTARGET} ${OBJFILES}
#	mkdir -p ${OBJPATH}/lib ${OBJPATH}/lib/libolmos
#	ar -x ./lib/libolmos.a && mv *.o ${OBJPATH}/lib/libolmos
#	ar rcs ${OBJPATH}/${LIBTARGET} ${OBJFILES} ${OBJPATH}/lib/libolmos/*.o

%.o: ${SRCPATH}/%.c
	@echo "Compiling: $@"
	${CC} ${CFLAGS} -c -o ${OBJPATH}/$@ $< ${LIBS}

clean:
	rm -frv ${OBJPATH}

showlib:
	ar -t ${LIBTARGET}

prebuild:
	mkdir -p ${OBJPATH}
