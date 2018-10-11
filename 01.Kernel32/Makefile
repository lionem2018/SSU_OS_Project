# file		makefile
# date		2008/12/14
# author	kkamagui 
# brief		��ȣ ��� Ŀ���� ����ϱ� ���� make ����

################################################################################
# ��� ȯ�� �� ��Ģ ���� 
################################################################################
# �����Ϸ� �� ��Ŀ ����
NASM32 = nasm
GCC32 = gcc -c -m32 -ffreestanding
LD32 = ld -melf_i386 -T ../elf_i386.x -nostdlib -e Main -Ttext 0x10200

# ���̳ʸ� �̹��� ������ ���� OBJCOPY �ɼ� ����
OBJCOPY32 = objcopy -j .text -j .data -j .rodata -j .bss -S -O binary

# ���͸� ����
OBJECTDIRECTORY = Temp
SOURCEDIRECTORY	= Source

################################################################################
# ��� �׸� �� ��� ��� ����
################################################################################
# �⺻������ ��带 ������ ���
all: prepare Kernel32.bin

# ������Ʈ ������ ��ġ�� ���͸��� ����
prepare:
	mkdir -p $(OBJECTDIRECTORY)

# Ŀ�� ��Ʈ�� ����Ʈ ���
$(OBJECTDIRECTORY)/EntryPoint.bin: $(SOURCEDIRECTORY)/EntryPoint.s
	$(NASM32) -o $@ $<

# Ŀ���� C �ҽ� ���Ͽ� ���� ������ ���� ����
dep:
	@echo === Make Dependancy File ===
	make -C $(OBJECTDIRECTORY) -f ../Makefile InternalDependency
	@echo === Dependancy Search Complete ===

# ���͸��� ������Ʈ ���� ���͸��� �̵��ؼ� ������ ���� �� ���� ������ ����
ExecuteInternalBuild: dep
	make -C $(OBJECTDIRECTORY) -f ../Makefile Kernel32.elf

# Ŀ�� �̹����� ���̳ʸ� ���Ϸ� ��ȯ
$(OBJECTDIRECTORY)/Kernel32.elf.bin: ExecuteInternalBuild
	$(OBJCOPY32) $(OBJECTDIRECTORY)/Kernel32.elf $@

# ��Ʈ�� ����Ʈ�� Ŀ���� ���ļ� ��ȣ ��� ���̳ʸ� ����
Kernel32.bin: $(OBJECTDIRECTORY)/EntryPoint.bin $(OBJECTDIRECTORY)/Kernel32.elf.bin
	cat $^ > $@
		
# �ҽ� ������ ������ ������ ���� ����	
clean:
	rm -f *.bin
	rm -f $(OBJECTDIRECTORY)/*.*

################################################################################
# Make�� ���� �ٽ� ȣ��Ǵ� �κ�, Temp ���͸��� �������� �����
################################################################################
# ����� C �ҽ� ���� ����, Temp ���͸��� �������� ����
CENTRYPOINTOBJECTFILE = Main.o
CSOURCEFILES = $(wildcard ../$(SOURCEDIRECTORY)/*.c)
ASSEMBLYSOURCEFILES = $(wildcard ../$(SOURCEDIRECTORY)/*.asm)
COBJECTFILES = $(subst Main.o, , $(notdir $(patsubst %.c,%.o,$(CSOURCEFILES))))
ASSEMBLYOBJECTFILES = $(notdir $(patsubst %.asm,%.o,$(ASSEMBLYSOURCEFILES)))

# .c ������ .o ���Ϸ� �ٲٴ� ��Ģ ����
%.o: ../$(SOURCEDIRECTORY)/%.c
	$(GCC32) -c $<

# .asm ������ .o ���Ϸ� �ٲٴ� ��Ģ ����
%.o: ../$(SOURCEDIRECTORY)/%.asm
	$(NASM32) -f elf32 -o $@ $<

# ���� �������� ���õ� ������ ����
InternalDependency:
	$(GCC32) -MM $(CSOURCEFILES) > Dependency.dep

# ���� Ŀ�� �̹����� ���
Kernel32.elf: $(CENTRYPOINTOBJECTFILE) $(COBJECTFILES) $(ASSEMBLYOBJECTFILES)
	$(LD32) -o $@ $^

# ���� ���丮�� ���� ��, dependency ������ ������ make�� ����	
ifeq (Dependency.dep, $(wildcard Dependency.dep))
include Dependency.dep
endif
