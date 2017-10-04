#include "fs.h"
#include "iap.h"
#include "utils.h"
#include "LPC1769.h"
#include <ctype.h>
#include <string.h>

unsigned int fs_mounted=0,fs_error=0;
struct FS fs;

int fs_copy(void) {
	return fs.copy;
}

int fs_errors(void) {
	return fs_error;
}

int flash_read(unsigned char *buff,unsigned int addr,int len) {
	memcpy(buff,(unsigned char*)addr,len);
	return STATUS_OK;
}

int flash_write(unsigned char *buff,unsigned int addr,int len) {
	return iap_copy_ram_to_flash(addr,buff,len)==IAP_STA_CMD_SUCCESS?STATUS_OK:STATUS_ERROR;
}

int flash_erase_sectors(unsigned int adra,unsigned int adrb) {
	return iap_erase_sectors(sector(adra),sector(adrb))==IAP_STA_CMD_SUCCESS?STATUS_OK:STATUS_ERROR;
}

void fs_errorsclear(void) {
	fs_error=0;
}

void fs_assert(int value,int error) {
	if(!value) fs_error|=(1<<error);
}

int fs_ismounted(void) {
	return fs_mounted==1;
}

int fs_mount(void) {
	unsigned char t[4];
	int i;
	unsigned int v;

	//randu pirma page'a, kuris yra tuscias [reiksme 0xffffffff]
	for(i=0;i<FS_COPY_COUNT && (flash_read(t,FS_START_ADDRESS+i*PAGE_SIZE,4),v=*((unsigned int*)t),v!=0xffffffff);i++);
	//nuskaitau pries tuscia page'a esanti page'a; jei visi page'ai tusti, reiskia failu sistemos flashe nebuvo
	if(i==0)
		memset(&fs,0,PAGE_SIZE);
	else
		flash_read((unsigned char*)&fs,FS_START_ADDRESS+(i-1)*PAGE_SIZE,PAGE_SIZE);
	fs_mounted=1;
	return i;
}

int fs_checkdisk(void) {
	int i;
	for(i=0;i<DIRECTORY_ENTRIES && fs_filenamevalid(fs.dir[i].file_name) && fs.dir[i].max_sectors>=0 && fs.dir[i].max_sectors<SECTORS && fs.dir[i].start_sector>=0 && fs.dir[i].start_sector<SECTORS && fs.dir[i].header_size<MAX_HEADER_SIZE && fs.dir[i].record_size<SECTOR_SIZE;i++);
	return i==DIRECTORY_ENTRIES?STATUS_OK:STATUS_ERROR;
}

void fs_format(void) {
	unsigned char status=1;
	fs_errorsclear();
	memset(&fs,0,PAGE_SIZE);
	status = status && flash_erase_sectors(FS_START_ADDRESS,FS_END_ADDRESS)==STATUS_OK;
	fs_flush();
	fs_assert(status,ERROR_FLASH);
}

int fs_flush(void) {
	unsigned char status=1,t[4];
	int i;
	unsigned int v;
	
	//surandu pirma tuscia page'a [pirmi keturi baitai 0xffffffff]
	for(i=0;i<FS_COPY_COUNT && (flash_read(t,FS_START_ADDRESS+i*PAGE_SIZE,4),v=*((unsigned int*)t),v!=0xffffffff);i++);
	if(i==FS_COPY_COUNT) { //nere tusciu page'u -- viska istrinsiu ir pradesiu nuo pradziu
		i=0;
		flash_erase_sectors(FS_START_ADDRESS,FS_START_ADDRESS);
	}
	fs.copy+=1;
	status = status && flash_write((unsigned char*)&fs,FS_START_ADDRESS+i*PAGE_SIZE,PAGE_SIZE)==STATUS_OK;
	fs_assert(status,ERROR_FLASH);
	return i;
}

int fs_direntryempty(int file) {
	int i;
	fs_assert(file>=0 && file<DIRECTORY_ENTRIES,ERROR_FILEHANDLE);
	for(i=0;i<MAX_FILE_NAME_SIZE && fs.dir[file].file_name[i]==0;i++); //direktorijos irasas [failas] laikomas tusciu, jei nera failo vardo
	return i==MAX_FILE_NAME_SIZE;
}

int fs_filenamevalid(char *file_name) {
	int i;
	for(i=0;i<MAX_FILE_NAME_SIZE-1 && (isalpha(file_name[i]) || isdigit(file_name[i]) || file_name[i]=='.' || file_name[i]=='_');i++);
	return file_name[i]==0;
}

int fs_freesectorfind(void) {
	unsigned char sector_mask[SECTORS/8+SECTORS%8!=0]={0}; //SECTORS == 9?
	int i,size,sector;

	for(i=0;i<DIRECTORY_ENTRIES;i++)
		if(!fs_direntryempty(i)) //pazymimi visi failo uzimami sektoriai; failas visada uzima bent viena sektoriu [net jei jo dydis yra 0, vienas sektorius jam yra isskiriamas kuriant faila]
			for(sector=fs.dir[i].start_sector,size=fs.dir[i].data_size;size==fs.dir[i].data_size || size>0;sector_mask[sector/8]|=(1<<(sector%8)),sector=fs.fat[sector],size-=SECTOR_SIZE);
	for(i=0;i<SECTORS && (sector_mask[i/8]&(1<<(i%8)))!=0;i++); //ieskau nepanaudoto sektoriaus
	return i<SECTORS?i+1:STATUS_ERROR; //i+1, nes cia nulinis sektorius atitinka pirma ir t.t.
}

int fs_freesectors(void) {
	unsigned char sector_mask[SECTORS/8+SECTORS%8!=0]={0};
	int i,k,sector,size;

	for(i=0;i<DIRECTORY_ENTRIES;i++)
		if(!fs_direntryempty(i))
			for(sector=fs.dir[i].start_sector,size=fs.dir[i].data_size;size==fs.dir[i].data_size || size>0;sector_mask[sector/8]|=(1<<(sector%8)),sector=fs.fat[sector],size-=SECTOR_SIZE);
	
	for(k=i=0;i<SECTORS;i++)
		if((sector_mask[i/8]&(1<<(i%8)))==0) k+=1;
	return k;
}

int fs_filesearch(char *file_name) {
	int i;
	for(i=0;i<DIRECTORY_ENTRIES && strcmp(file_name,fs.dir[i].file_name)!=0;i++);
	return i==DIRECTORY_ENTRIES?STATUS_ERROR:i;
}

int fs_filesearch_tail(char *file_tail) {
	int i;
	for(i=0;i<DIRECTORY_ENTRIES && strcmp(file_tail,fs.dir[i].file_name+strlen(fs.dir[i].file_name)-strlen(file_tail))!=0;i++);
	return i==DIRECTORY_ENTRIES?STATUS_ERROR:i;
}

int fs_filenew(char *file_name,short max_sectors,short record_size) {
	int i,s,f,l=strlen(file_name);
	for(i=0;i<DIRECTORY_ENTRIES && !fs_direntryempty(i);i++); //ieskau ar yra laisvas irasas direktorijoje
	if(i<DIRECTORY_ENTRIES && l>0 && l<=MAX_FILE_NAME_SIZE-1 && max_sectors<SECTORS && record_size>0 && record_size<SECTOR_SIZE && (s=fs_freesectorfind())!=STATUS_ERROR) {
		strcpy(fs.dir[i].file_name,file_name);
		fs.dir[i].max_sectors=max_sectors;
		fs.dir[i].start_sector=s;
		fs.dir[i].header_size=0;
		fs.dir[i].record_size=record_size;
		fs.dir[i].data_size=0;
		f=i;
	}
	else f=STATUS_ERROR;
	return f;
}

void fs_filedelete(int file) {
	unsigned char status=1;
	int size,sector;
	fs_assert(file>=0 && file<DIRECTORY_ENTRIES,ERROR_FILEHANDLE);
	//reikia fiziskai istrinti visus failo uzimtus sektorius
	for(sector=fs.dir[file].start_sector,size=fs.dir[file].data_size;size==fs.dir[file].data_size || (size-=SECTOR_SIZE)>0;sector=fs.fat[sector]);
	status = status && flash_erase_sectors(FS_START_ADDRESS+fs.dir[file].start_sector*SECTOR_SIZE,FS_START_ADDRESS+sector*SECTOR_SIZE)==STATUS_OK;
	memset(&fs.dir[file],0,sizeof(struct DirectoryEntry));
	fs_assert(status,ERROR_FLASH);
}

int fs_fileheadersize(int file) {
	fs_assert(file>=0 && file<DIRECTORY_ENTRIES,ERROR_FILEHANDLE);
	return fs.dir[file].header_size;
}

int fs_filerecordsize(int file) {
	fs_assert(file>=0 && file<DIRECTORY_ENTRIES,ERROR_FILEHANDLE);
	return fs.dir[file].record_size;
}

int fs_filedatasize(int file) {
	fs_assert(file>=0 && file<DIRECTORY_ENTRIES,ERROR_FILEHANDLE);
	return fs.dir[file].data_size;
}

int fs_filesize(int file) {
	fs_assert(file>=0 && file<DIRECTORY_ENTRIES,ERROR_FILEHANDLE);
	return fs.dir[file].header_size+fs.dir[file].data_size;
}

char *fs_filename(int file) {
	fs_assert(file>=0 && file<DIRECTORY_ENTRIES,ERROR_FILEHANDLE);
	return fs.dir[file].file_name;
}

char *fs_fileheader(int file) {
	fs_assert(file>=0 && file<DIRECTORY_ENTRIES,ERROR_FILEHANDLE);
	return fs.dir[file].header;
}

void fs_fileheaderset(int file,char *header,int len) {
	fs_assert(file>=0 && file<DIRECTORY_ENTRIES,ERROR_FILEHANDLE);
	len=MIN2(len,MAX_HEADER_SIZE);
	fs.dir[file].header_size=len;
	memcpy(fs.dir[file].header,header,len);
}

void fs_fileappend(int file,char *buf,int len) { //cia len t.b. 256 arba 512
	unsigned char status=1;
	int k_sector,last_sector,new_sector,size,l;
	
	fs_assert(file>=0 && file<DIRECTORY_ENTRIES,ERROR_FILEHANDLE);
	fs_assert(len==256 || len==512,ERROR_DATALEN);
	
	//surandamas paskutinis failo uzimamas sektorius ir jame uzpildytu baitu kiekis
	for(k_sector=1,last_sector=fs.dir[file].start_sector,size=fs.dir[file].data_size;size-SECTOR_SIZE>0;k_sector+=1,last_sector=fs.fat[last_sector],size-=SECTOR_SIZE);
	do {
		//paskaiciuojam kiek telpa i dabartini sektoriu
		l=SECTOR_SIZE-size;
		if(l>0) { //kazkiek telpa i dabartini sektoriu
			l=MIN2(l,len);
			status = status && flash_write((unsigned char*)buf,FS_START_ADDRESS+last_sector*SECTOR_SIZE+size,l)==STATUS_OK;
			fs.dir[file].data_size+=l;
			size+=l;
			buf+=l;
			len-=l;
		}
		else { //reikalingas naujas sektorius
			if((fs.dir[file].max_sectors>0 && k_sector+1>fs.dir[file].max_sectors) || (new_sector=fs_freesectorfind())==STATUS_ERROR) { //failas pasieke jam skirtu sektoriu kieki arba nera tuscio sektoriaus -- rasysime i pirma failo sektoriu
				flash_erase_sectors(FS_START_ADDRESS+fs.dir[file].start_sector*SECTOR_SIZE,FS_START_ADDRESS+fs.dir[file].start_sector*SECTOR_SIZE);
				new_sector=fs.dir[file].start_sector;
				fs.dir[file].start_sector=fs.fat[fs.dir[file].start_sector];
				fs.dir[file].data_size-=SECTOR_SIZE;
			}
			size=0;
			fs.fat[last_sector]=new_sector;
			last_sector=new_sector;
		}
	} while(len>0);
	fs_assert(status,ERROR_FLASH);
}

int fs_fileread_seq(int file,char *buf,int len) {
	static int current_file=-1,state,size_returned;
	unsigned char status=1;
	int sector,size,l=0,r=0;

	if(current_file!=file) {
		current_file=file;
		size_returned=0;
		state=1;
	}
	if(current_file<0)
		state=0;
	
	if(state==1) { //nuskaitomas header'is
		l=MIN2(len,fs.dir[file].header_size-size_returned); //pasirinkimas tarp kiek liko issiusti ir buferio dydzio
		if(l>0) {
			memcpy(buf,fs.dir[file].header+size_returned,l);
			len-=l;
			buf+=l;
			size_returned+=l;
			r+=l;
		}
		if(size_returned==fs.dir[file].header_size) {
			size_returned=0;
			state=2;
		}
	}
	if(state==2) { //nuskaitomi failo duomenys
		//is pradziu reikia rasti sektoriu, kur prasideda dar negrazinti duomenys
		for(sector=fs.dir[file].start_sector,size=size_returned;size-SECTOR_SIZE>0;sector=fs.fat[sector],size-=SECTOR_SIZE);
		while(len>0 && size_returned!=fs.dir[file].data_size) {
			//cia sector yra dabartinis sektorius, size - kiek jame baitu jau grazinta
			l=MIN3(len,fs.dir[file].data_size-size_returned,SECTOR_SIZE-size);
			if(l>0) {
				status = status && flash_read((unsigned char*)buf,FS_START_ADDRESS+sector*SECTOR_SIZE+size,l)==STATUS_OK;
				len-=l;
				buf+=l;
				size_returned+=l;
				r+=l;
			}
			size=0;
			sector=fs.fat[sector];
		}
		if(size_returned==fs.dir[file].data_size) current_file=-1;
	}
	fs_assert(status,ERROR_FLASH);
	return r;
}

void fs_fileread_datapart(int file,int adr,int len,unsigned char *buf) {
	unsigned char status=1;
	int l,sector;
	fs_assert(file>=0 && file<DIRECTORY_ENTRIES,ERROR_FILEHANDLE);
	if(len<0) { //jei len<0, adr skaiciuojamas nuo failo pabaigos
		len=-len;
		adr=fs.dir[file].data_size-adr-len;
	}
	fs_assert(adr+len<=fs.dir[file].data_size,ERROR_ADRLEN);
	
	for(sector=fs.dir[file].start_sector;adr-SECTOR_SIZE>0;sector=fs.fat[sector],adr-=SECTOR_SIZE);
	while(len>0) {
		l=MIN2(len,SECTOR_SIZE-adr);
		if(l>0) {
			status = status && flash_read(buf,FS_START_ADDRESS+sector*SECTOR_SIZE+adr,l)==STATUS_OK;
			len-=l;
			buf+=l;
		}
		adr=0;
		sector=fs.fat[sector];
	}
	fs_assert(status,ERROR_FLASH);
}
