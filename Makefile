TARGETS = cpuinfo_$(TARGET_CPU).exe

CC = cl.exe
CFLAGS = $(CFLAGS) /Zi
LIBS =
RM = DEL /Q
CLEANFILES = $(TARGETS) *.exe *.dll *.exp *.obj *.pdb *.ilk

all: $(TARGETS)

cpuinfo_$(TARGET_CPU).exe: cpuinfo.obj ll$(TARGET_CPU).obj
	$(CC) /Fe$@ $** $(LIBS)

cpuinfo.obj: cpuinfo.c
	$(CC) /c $** $(CFLAGS)

ll$(TARGET_CPU).obj: ll$(TARGET_CPU).asm
	ml64 /c $**

clean:
	$(RM) $(CLEANFILES) > NUL 2>&1
