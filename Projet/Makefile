
LIB_MODE=1

PROGRAM	:= game
UTIL := maputil

# Must be the first rule
.PHONY: game
game: $(PROGRAM)


SOURCES := $(wildcard src/*.c)
OBJECTS := $(SOURCES:src/%.c=obj/%.o)
DEPENDS := $(SOURCES:src/%.c=deps/%.d)

MAKEFILES := Makefile

CUSTOM_OBJ := obj/mapio.o
LIB	:= lib/libgame.a

#CC=gcc
CFLAGS := -O3 -g -std=c99 -Wall -Wno-unused-function
CFLAGS += -DPADAWAN
CFLAGS += -I./include
CFLAGS += $(shell pkg-config SDL2_image SDL2_mixer --cflags)
LDLIBS := $(shell pkg-config SDL2_image SDL2_mixer --libs)

$(OBJECTS): $(MAKEFILES)

ifndef LIB_MODE
$(LIB): $(filter-out $(CUSTOM_OBJ),$(OBJECTS))
	ar -crs $(LIB) $^
endif

$(PROGRAM): $(CUSTOM_OBJ) $(LIB)
	$(CC) -o $@ $^ $(LDFLAGS) $(LDLIBS)

$(OBJECTS): obj/%.o: src/%.c
	$(CC) -o $@ $(CFLAGS) -c $<

#Compilation of maputil
.PHONY: maputil
maputil: $(UTIL)


SOURCES_M := $(wildcard util/*.c)
OBJECTS_M := $(SOURCES_M:util/%.c=obj/%.o)
DEPENDS := $(SOURCES_M:util/%.c=deps/%.d)

MAKEFILES := Makefile

CUSTOM_OBJ_M := obj/maputil.o
LIB	:= lib/libgame.a

#CC=gcc
CFLAGS := -O3 -g -std=c99 -Wall -Wno-unused-function
CFLAGS += -DPADAWAN
CFLAGS += -I./include
CFLAGS += $(shell pkg-config SDL2_image SDL2_mixer --cflags)
LDLIBS := $(shell pkg-config SDL2_image SDL2_mixer --libs)

$(OBJECTS): $(MAKEFILES)

ifndef LIB_MODE
$(LIB): $(filter-out $(CUSTOM_OBJ_M),$(OBJECTS_M))
	ar -crs $(LIB_M) $^
endif

$(UTIL): $(CUSTOM_OBJ_M) $(LIB_M)
	$(CC) -o $@ $^ $(LDFLAGS) $(LDLIBS_M)

$(OBJECTS_M): obj/%.o: util/%.c
	$(CC) -o $@ $(CFLAGS) -c $<



.PHONY: depend
depend: $(DEPENDS)

$(DEPENDS): $(MAKEFILES)

$(DEPENDS): deps/%.d: src/%.c
	$(CC) $(CFLAGS) -MM $< | \
		sed -e 's|\(.*\)\.o:|deps/\1.d obj/\1.o:|g' > $@


ifneq ($(MAKECMDGOALS),clean)
-include $(DEPENDS)
endif

#Clean the project
.PHONY: clean
clean: 
	rm -f game obj/*.o deps/*.d maps/*.map
	rm -f maputil
	rm -f src/*~ util/*~
