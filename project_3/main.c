#include "BF.h"
#include "column_store.h"
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>




int main() {
  /* Καλείστε να γράψετε κώδικα που να αποδεικνύει την ορθότητα του προγράμματός σας */
  // Add your test code here
	char *fieldNames[6];
	int i,x,y,k;;

	fieldNames[0]= "id";
	fieldNames[1]= "name";
	fieldNames[2]= "surname";
	fieldNames[3]= "status";
	fieldNames[4]= "dateOfBirth";
	fieldNames[5]= "salary";
	char path[] = "/home/giwrgos/Documents/sxolh/Data_base/project3/FOLDER"; //edw prepei na bei to katallhlo path

	CS_Init();
	if(CS_CreateFiles(fieldNames,path) < 0 ){
		printf("ERROR IN CREATE\n");
        return 0;
    }
	HeaderInfo *HI = malloc(sizeof(HeaderInfo)); 
	if(CS_OpenFile(HI, path) < 0 ){
		printf("ERROR IN OPEN\n");
        return 0;
    }
	for(i=0; i<6; i++){
		printf("D--->%s %d\n",HI->column_fds[i].columnName ,HI->column_fds[i].fd );
	}
   		
	//<read text and create record >
	FILE *fp;
    ssize_t rd;
    size_t len=0;
	fp = fopen("records.dat", "r");	
	char line1[256],temp[20];			//temp 20 gt to megalutero apo records
	char *line;
    Record Eg;
    while ((rd = getline(&line, &len, fp)) != -1){  
    	x=0; y=0; k=0;
    	while(line[x] != '\0' ){
    		while(line[x] != ',' && line[x] != '\"' && line[x] != '\0' && line[x] != '\n'){				// && ll      === && line[x] != '\"'
    			temp[y] = line[x];
                y++;
    			x++;
    		}
    		if(k==0){
    			Eg.id = atoi(temp);
    			k++;
    			x++;			//na perasei to epomeno " meta to id
    		}
    		else if(k==1){
    			strcpy(Eg.name, temp);
    			k++;
    			x = x + 2 ;  //na perasei ta "," 
    		}
    		else if(k==2){
    			strcpy(Eg.surname, temp);
    			k++;
    			x = x + 2 ;	
    		}
    		else if(k==3){
    			strcpy(Eg.status, temp);
    			k++;
    			x = x + 2 ;
    		}
    		else if(k==4){
    			strcpy(Eg.dateOfBirth, temp);
    			k++;
    			x = x + 1 ;			//+1 na perasei ta ", gt meta exei integer
    		}
    		else if(k==5){
    			Eg.salary = atoi(temp);
    		} 
    		if(line[x] == '\0' ) break;
    		x++;
            y=0;
            memset(temp,0,20);
    	}
    	printf("RECORD: %d %s %s %s %s %d\n",Eg.id, Eg.name, Eg.surname, Eg.status, Eg.dateOfBirth, Eg.salary );
    	//call insert and then memset
        if(CS_InsertEntry(HI, Eg)<0)
            printf("%s\n","Error in insert!!!" );
    	memset(Eg.name,0,15);
    	memset(Eg.surname,0,20);
    	memset(Eg.status,0,10);
    	memset(Eg.dateOfBirth,0,11); 
    }
    printf("%s\n","END OF INSERT" );
    fclose(fp);
    //get alla entries stuff
    char *fieldNames1[0];
    fieldNames1[0]= "id";
    fieldNames1[1]= "name";
    fieldNames1[2]= "surname";
    CS_GetAllEntries(HI,"name","Nikolas","salary dateOfBirth name status ", 3);
    CS_GetAllEntries(HI,NULL,NULL,"salary dateOfBirth id status ", 3);
    if(CS_CloseFile(HI) < 0 )
        printf("ERROR IN close\n"); 
    free(HI);
    return 0;
}
