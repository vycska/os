#ifndef __FILE_SYSTEM_H__
#define __FILE_SYSTEM_H__

#define PAGE_SIZE 256 //cia naudojamas kaip maziausias i flasha irasomas duomenu kiekis ir kaip metaduomenu [dir, fat] dydis

#define FS_START_ADDRESS (0x38000) //21 proco sektorius
#define FS_END_ADDRESS (0x7ffff) //29 proco sektoriaus pabaiga
#define FS_TOTAL_SIZE (FS_END_ADDRESS-FS_START_ADDRESS+1) //0x48000 = 288 * 1024

//sekanciu triju define'u reiksmes turi susisumuoti i 256
#define DIRECTORY_SIZE 244
#define FAT_SIZE 8
#define FS_COPY_COUNT 128 //kiek bus failu sistemos duomenu [direktorijos ir fat lenteles] kopiju, cia ju turetu buti tiek, kad uzimtu visa sektoriu

#define DIRECTORY_ENTRIES (DIRECTORY_SIZE/sizeof(struct DirectoryEntry)) //max failu gali buti 244/61 = 4

#define SECTORS 9 //vienas sektorius bus skirtas direktorijai ir fat lentelei [struct FS]
#define SECTOR_SIZE FS_TOTAL_SIZE/SECTORS //32,768

#define MAX_FILE_NAME_SIZE 16
#define MAX_HEADER_SIZE 33

#define STATUS_OK 0
#define STATUS_ERROR -1

#define ERROR_FLASH 0
#define ERROR_FILEHANDLE 1
#define ERROR_DATALEN 2
#define ERROR_ADRLEN 3

#pragma pack(1)
struct DirectoryEntry { //sizeof = 61
	int data_size;
	short max_sectors, //kiek daugiausiai sektoriu gali sudaryti faila, reiksme 0 reiskia neribota kieki
			start_sector,
			header_size,
			record_size; //reikalingas jei failas sudarytas is fiksuoto dydzio irasu, kurie negali buti perskirti, kas gali atsitikti ties sektoriu riba; isuo atveju cia record size turi dalinti 256 be liekanos, pvz. 1, 2, 4 ir t.t.
	char file_name[MAX_FILE_NAME_SIZE],
			header[MAX_HEADER_SIZE];
};
#pragma pack()

#pragma pack(1)
struct FS { //256 bytes [vienas page'as]
	unsigned int copy;
	unsigned char fat[FAT_SIZE]; //8 bytes
	struct DirectoryEntry dir[DIRECTORY_ENTRIES]; //244 bytes
};
#pragma pack()

int fs_errors(void);
int fs_copy(void);
void fs_assert(int value,int error);
int fs_ismounted(void);
int fs_mount(void);
int fs_checkdisk(void);
void fs_format(void);
int fs_flush(void);
int fs_direntryempty(int);
int fs_filenamevalid(char *file_name);
int fs_freesectorfind(void);
int fs_freesectors(void);
int fs_filesearch(char *file_name);
int fs_filesearch_tail(char *file_tail);
int fs_filenew(char *file_name,short max_sectors,short record_size);
void fs_filedelete(int file);
int fs_fileheadersize(int file);
int fs_filerecordsize(int file);
int fs_filedatasize(int file);
int fs_filesize(int file);
char *fs_filename(int file);
char *fs_fileheader(int file);
void fs_fileheaderset(int file,char *header,int len);
void fs_fileappend(int file,char *buf,int len);
int fs_fileread_seq(int file,char *buf,int len);
void fs_fileread_datapart(int file,int adr,int len,unsigned char *buf);

#endif
