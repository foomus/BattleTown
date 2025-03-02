CC=gcc
CFLAGS=-g -Wall
SDLPATH= -I./src/SDL2/include/ -L./src/SDL2/lib/
LFLAGS= -lmingw32 -lSDL2main -lSDL2_image -lSDL2_ttf -lSDL2_mixer -lSDL2 -mwindows 

BINDIR=./out/bin/
OBJDIR=./out/obj/
WRAPPER=./src/wrapper/

ifdef OS
	OUTPUT=$(BINDIR)main.exe
	CLEAN=del /q out\bin\main.exe out\obj\*.o
else 
	OUTPUT=$(BINDIR)main
	CLEAN=rm -f $(OUTPUT) $(OBJDIR)*.o
endif

OBJS=$(OBJDIR)main.o $(OBJDIR)SDL_Controls.o $(OBJDIR)AssetControls.o $(OBJDIR)errHandler.o $(OBJDIR)timer.o $(OBJDIR)LinkedList.o $(OBJDIR)ui.o $(OBJDIR)tank.o $(OBJDIR)blocks.o $(OBJDIR)bullet.o $(OBJDIR)stageReader.o $(OBJDIR)explosions.o $(OBJDIR)powerup.o

all: $(OUTPUT)

$(OUTPUT): $(OBJS)
	$(CC) $(SDLPATH) $^ $(LFLAGS) -o $@

$(OBJDIR)%.o: src/%.c
	$(CC) $(SDLPATH) $(CFLAGS) -c $^ -o $@

$(OBJDIR)%.o: $(WRAPPER)%.c
	$(CC) $(SDLPATH) $(CFLAGS) -c $^ -o $@

clean: 
	$(CLEAN)