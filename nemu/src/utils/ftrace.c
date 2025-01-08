#include<stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <elf.h>
#include <assert.h>
#include <stdlib.h>
#include<string.h>
#include<common.h>
typedef struct {
	char name[32]; // func name, 32 should be enough
	uint32_t addr;
	unsigned char info;
	uint32_t size;
} SymEntry;

SymEntry *symbol_tbl = NULL; // dynamic allocated
int symbol_tbl_size = 0;
int call_depth = 0;
typedef struct tail_rec_node {
	paddr_t pc;
	int depth;
	struct tail_rec_node *next;
} TailRecNode;
TailRecNode *tail_rec_head = NULL; // linklist with head, dynamic allocated
void read_elf_header(int fd,Elf32_Ehdr *eh){
   assert(lseek(fd, 0, SEEK_SET) == 0);
  assert(read(fd, (void *)eh, sizeof(Elf32_Ehdr)) == sizeof(Elf32_Ehdr));
    return ;
}
void read_section_header(int fd,Elf32_Ehdr eh,Elf32_Shdr * tb_sr){
    lseek(fd,eh.e_shoff,SEEK_SET);
    for(int i=0;i<eh.e_shnum;i++){
       assert(read(fd,(void *)&tb_sr[i],eh.e_shentsize)==eh.e_shentsize);
    }
    return;
}
static void read_section(int fd, Elf32_Shdr sh, void *dst) {
	assert(dst != NULL);
	assert(lseek(fd, (off_t)sh.sh_offset, SEEK_SET) == (off_t)sh.sh_offset);
    printf("0x%08x\n ", sh.sh_size);
	assert(read(fd, dst, sh.sh_size) == sh.sh_size);
}
void display_elf_header(Elf32_Ehdr eh){
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
	printf("Entry point\t= 0x%08x\n", eh.e_entry);

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

}
static void display_section_headers(int fd, Elf32_Ehdr eh, Elf32_Shdr sh_tbl[]) {
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

void read_symbol_table(int fd,Elf32_Ehdr eh,Elf32_Shdr * tb_sr,int sh_idx){
    Elf32_Sym sym_tbl[tb_sr[sh_idx].sh_size];
    read_section(fd,tb_sr[sh_idx],sym_tbl);
     int str_idx = tb_sr[sh_idx].sh_link;
    char str_tbl[tb_sr[str_idx].sh_size];
    read_section(fd, tb_sr[str_idx], str_tbl);
    int sym_count = (tb_sr[sh_idx].sh_size / sizeof(Elf32_Sym));
    for(int i=0;i<sym_count;i++){
        printf("%d %s\n",i,str_tbl+sym_tbl[i].st_name);
    }
	printf("====================================================\n\n");

	// read
	symbol_tbl_size = sym_count;
	symbol_tbl = malloc(sizeof(SymEntry) * sym_count);
  for (int i = 0; i < sym_count; i++) {
    symbol_tbl[i].addr = sym_tbl[i].st_value;
		symbol_tbl[i].info = sym_tbl[i].st_info;
		symbol_tbl[i].size = sym_tbl[i].st_size;
		memset(symbol_tbl[i].name, 0, 32);
		strncpy(symbol_tbl[i].name, str_tbl + sym_tbl[i].st_name, 31);
		printf("%d %s ",i,str_tbl+sym_tbl[i].st_name);
		printf("st_info :%u\n",sym_tbl[i].st_info);
  }

}
void read_symbols(int fd, Elf32_Ehdr eh, Elf32_Shdr* tb_sr) {
  for (int i = 0; i < eh.e_shnum; i++) {
		switch (tb_sr[i].sh_type) {
		case SHT_SYMTAB: 
		//case SHT_DYNSYM:
			read_symbol_table(fd, eh, tb_sr, i); 
            break;
		}
  }
}
static void init_tail_rec_list() {
	tail_rec_head = (TailRecNode *)malloc(sizeof(TailRecNode));
	tail_rec_head->pc = 0;
	tail_rec_head->next = NULL;
}
int parse_elf(const char * elf_file){ 
    int fd=open(elf_file, O_RDONLY|O_SYNC);
    Elf32_Ehdr eh;
    read_elf_header(fd,&eh);
    display_elf_header(eh);
    //Elf32_Shdr tb_sr[eh.e_shentsize * eh.e_shnum];
	Elf32_Shdr tb_sr[ eh.e_shnum+1];
    read_section_header(fd,eh,tb_sr);
    display_section_headers(fd,eh,tb_sr);
    read_symbols(fd,eh,tb_sr);
	init_tail_rec_list();
	close(fd);
    return 0;
}
int find_symbol_function(uint32_t target,bool is_call){
	for(int i=0;i<symbol_tbl_size;i++){
		if(ELF32_ST_TYPE(symbol_tbl[i].info)==STT_FUNC){
			if(is_call){
				if(symbol_tbl[i].addr==target){
				return i;
			 }
			}
			else{
				if (symbol_tbl[i].addr <= target && target < symbol_tbl[i].addr + symbol_tbl[i].size) return i;
			}
		}
	}
	return -1;
}
void insert_tail(paddr_t pc,int depth){
	TailRecNode *node = (TailRecNode *)malloc(sizeof(TailRecNode));
	node->next=tail_rec_head->next;
	node->pc=pc;
	node->depth=depth;
	tail_rec_head->next=node;
}
void remove_tail(){
	TailRecNode *node=tail_rec_head->next;
	tail_rec_head->next=node->next;
	free(node);
}
void trace_func_call(uint32_t pc,uint32_t target,bool is_tail){
	if(symbol_tbl == NULL) return;
	call_depth++;
	int i=find_symbol_function(target,true);
	printf("%d ",call_depth);
	printf(FMT_PADDR ": call [%s@" FMT_PADDR "]\n",
		pc,
		i>=0?symbol_tbl[i].name:"???",
		target
	);
	if(is_tail) insert_tail(pc,call_depth-1);
	//find_symbol_function(target);
	
}
void trace_func_ret(paddr_t pc){
	int i=find_symbol_function(pc,false);
	printf("%d ",call_depth);
	printf(FMT_PADDR ": ret [%s]\n",
		pc,
		i>=0?symbol_tbl[i].name:"???"
	);
	call_depth--;
	TailRecNode *node = tail_rec_head->next;
	if(node!=NULL){
		if(node->depth==call_depth){
			paddr_t ret_target = node->pc;
			remove_tail();
			trace_func_ret(ret_target);
		}
	}
}

