#include <common.h>
#include <fcntl.h>
#include <unistd.h>
#include <elf.h>

void read_elf_header(int fd,ELF32_Ehdr *eh){
    lseek(fd,0,SEEK_SET);
    read(fd,eh,sizeof(ELF32_Ehdr));
    return ;
}
static void read_section(int fd, Elf32_Shdr sh, void *dst) {
	assert(dst != NULL);
	assert(lseek(fd, (off_t)sh.sh_offset, SEEK_SET) == (off_t)sh.sh_offset);
	assert(read(fd, dst, sh.sh_size) == sh.sh_size);
}

void read_section_header(int fd,ELF32_Ehdr eh,Elf32_Shdr * tb_sr){
    lseek(fd,eh.e_shoff,SEEK_SET);
    for(int i=0;i<eh.e_shnum;i++){
       read(fd,(void *)&tb_sr[i],eh.e_shentsize);
    }
    return;
}
void display_elf_header(ELF32_Ehdr eh){
    	/* Storage capacity class */
	printf("Storage class\t= ");
	switch(eh.e_ident[EI_CLASS])
	{
		case ELFCLASS32:
			printf("32-bit objects\n");
			break;

		case ELFCLASS64:
			printf("64-bit objects\n");
			break;

		default:
			printf("INVALID CLASS\n");
			break;
	}

	/* Data Format */
	printf("Data format\t= ");
	switch(eh.e_ident[EI_DATA])
	{
		case ELFDATA2LSB:
			printf("2's complement, little endian\n");
			break;

		case ELFDATA2MSB:
			printf("2's complement, big endian\n");
			break;

		default:
			printf("INVALID Format\n");
			break;
	}

	/* OS ABI */
	printf("OS ABI\t\t= ");
	switch(eh.e_ident[EI_OSABI])
	{
		case ELFOSABI_SYSV:
			printf("UNIX System V ABI\n");
			break;

		case ELFOSABI_HPUX:
			printf("HP-UX\n");
			break;

		case ELFOSABI_NETBSD:
			printf("NetBSD\n");
			break;

		case ELFOSABI_LINUX:
			printf("Linux\n");
			break;

		case ELFOSABI_SOLARIS:
			printf("Sun Solaris\n");
			break;

		case ELFOSABI_AIX:
			printf("IBM AIX\n");
			break;

		case ELFOSABI_IRIX:
			printf("SGI Irix\n");
			break;

		case ELFOSABI_FREEBSD:
			printf("FreeBSD\n");
			break;

		case ELFOSABI_TRU64:
			printf("Compaq TRU64 UNIX\n");
			break;

		case ELFOSABI_MODESTO:
			printf("Novell Modesto\n");
			break;

		case ELFOSABI_OPENBSD:
			printf("OpenBSD\n");
			break;

		case ELFOSABI_ARM_AEABI:
			printf("ARM EABI\n");
			break;

		case ELFOSABI_ARM:
			printf("ARM\n");
			break;

		case ELFOSABI_STANDALONE:
			printf("Standalone (embedded) app\n");
			break;

		default:
			printf("Unknown (0x%x)\n", eh.e_ident[EI_OSABI]);
			break;
	}

	/* ELF filetype */
	printf("Filetype \t= ");
	switch(eh.e_type)
	{
		case ET_NONE:
			printf("N/A (0x0)\n");
			break;

		case ET_REL:
			printf("Relocatable\n");
			break;

		case ET_EXEC:
			printf("Executable\n");
			break;

		case ET_DYN:
			printf("Shared Object\n");
			break;
		default:
			printf("Unknown (0x%x)\n", eh.e_type);
			break;
	}

	/* ELF Machine-id */
	printf("Machine\t\t= ");
	switch(eh.e_machine)
	{
		case EM_NONE:
			printf("None (0x0)\n");
			break;

		case EM_386:
			printf("INTEL x86 (0x%x)\n", EM_386);
			break;

		case EM_X86_64:
			printf("AMD x86_64 (0x%x)\n", EM_X86_64);
			break;

		case EM_AARCH64:
			printf("AARCH64 (0x%x)\n", EM_AARCH64);
			break;

		default:
			printf(" 0x%x\n", eh.e_machine);
			break;
	}

	/* Entry point */
	printf("Entry point\t= 0x%08lx\n", eh.e_entry);

	/* ELF header size in bytes */
	printf("ELF header size\t= 0x%08x\n", eh.e_ehsize);

	/* Program Header */
	printf("Program Header\t= ");
	printf("0x%08x\n", eh.e_phoff);		/* start */
	printf("\t\t  %d entries\n", eh.e_phnum);	/* num entry */
	printf("\t\t  %d bytes\n", eh.e_phentsize);	/* size/entry */

	/* Section header starts at */
	printf("Section Header\t= ");
	printf("0x%08x\n", eh.e_shoff);		/* start */
	printf("\t\t  %d entries\n", eh.e_shnum);	/* num entry */
	printf("\t\t  %d bytes\n", eh.e_shentsize);	/* size/entry */
	printf("\t\t  0x%08x (string table offset)\n", eh.e_shstrndx);

	/* File flags (Machine specific)*/
	printf("File flags \t= 0x%08x\n", eh.e_flags);

	/* ELF file flags are machine specific.
	 * INTEL implements NO flags.
	 * ARM implements a few.
	 * Add support below to parse ELF file flags on ARM
	 */
	int32_t ef = eh.e_flags;
	printf("\t\t  ");

	if(ef & EF_ARM_RELEXEC)
		printf(",RELEXEC ");

	if(ef & EF_ARM_HASENTRY)
		printf(",HASENTRY ");

	if(ef & EF_ARM_INTERWORK)
		printf(",INTERWORK ");

	if(ef & EF_ARM_APCS_26)
		printf(",APCS_26 ");

	if(ef & EF_ARM_APCS_FLOAT)
		printf(",APCS_FLOAT ");

	if(ef & EF_ARM_PIC)
		printf(",PIC ");

	if(ef & EF_ARM_ALIGN8)
		printf(",ALIGN8 ");

	if(ef & EF_ARM_NEW_ABI)
		printf(",NEW_ABI ");

	if(ef & EF_ARM_OLD_ABI)
		printf(",OLD_ABI ");

	if(ef & EF_ARM_SOFT_FLOAT)
		printf(",SOFT_FLOAT ");

	if(ef & EF_ARM_VFP_FLOAT)
		printf(",VFP_FLOAT ");

	if(ef & EF_ARM_MAVERICK_FLOAT)
		printf(",MAVERICK_FLOAT ");

	printf("\n");

	/* MSB of flags conatins ARM EABI version */
	printf("ARM EABI\t= Version %d\n", (ef & EF_ARM_EABIMASK)>>24);

	printf("\n");	/* End of ELF header */

}static void display_section_headers(int fd, Elf32_Ehdr eh, Elf32_Shdr sh_tbl[]) {
  // warn: C99
	char sh_str[sh_tbl[eh.e_shstrndx].sh_size];
  read_section(fd, sh_tbl[eh.e_shstrndx], sh_str);
  
	/* Read section-header string-table */

	printf("========================================");
	printf("========================================\n");
	printf(" idx offset     load-addr  size       algn"
			" flags      type       section\n");
	printf("========================================");
	printf("========================================\n");

	for(int i = 0; i < eh.e_shnum; i++) {
		printf(" %03d ", i);
		printf("0x%08x ", sh_tbl[i].sh_offset);
		printf("0x%08x ", sh_tbl[i].sh_addr);
		printf("0x%08x ", sh_tbl[i].sh_size);
		printf("%-4d ", sh_tbl[i].sh_addralign);
		printf("0x%08x ", sh_tbl[i].sh_flags);
		printf("0x%08x ", sh_tbl[i].sh_type);
		printf("%s\t", (sh_str + sh_tbl[i].sh_name));
		printf("\n");
	}
	printf("========================================");
	printf("========================================\n");
	printf("\n");	/* end of section header table */
}
void read_symbol_table(int fd,Elf32_Shdr * tb_sr,int sh_idx){
    Elf32_Sym sym_tbl[tb_sr[sh_idx].sh_size];
    read_section(fd,tb_sr[sh_idx],sym_tbl);

}
void read_symbols(int fd, Elf32_Ehdr eh, Elf32_Shdr* tb_sr) {
  for (int i = 0; i < eh.e_shnum; i++) {
		switch (tb_sr[i].sh_type) {
		case SHT_SYMTAB: case SHT_DYNSYM:
			read_symbol_table(fd, eh, tb_sr, i); break;
		}
  }
}

void parse_elf(const char * elf_file){ 
    int fd=open(elf_file, O_RDONLY|O_SYNC);
    ELF32_Ehdr eh;
    char * buf=NULL;
    read_elf_header(fd,eh);
    display_elf_header(eh);
    Elf32_Shdr tb_sr[eh.e_shentsize * eh.e_shnum];
    read_section_header(fd,eh,tb_sr);
    read_symbols();
}