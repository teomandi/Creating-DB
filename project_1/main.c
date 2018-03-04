#include <stdio.h>
#include "heap.h"

#define size 100 //analogo tou csv arxeiou


int main(){
	HP_Init();
	if(HP_CreateFile("geo1")==0){
		int a =HP_OpenFile("geo1");
		if(a>0){
			FILE *fp;
			fp=fopen("100.csv","r");
			Record *r[size];
			char *line = NULL;
			char read;
			size_t len=0;
			int i=0;
			while ((read = getline(&line, &len, fp)) != -1){
				r[i]=malloc(sizeof(Record));
				int j=0,count=0,k,l;
				char n[20];
				while(line[j]!='\n'){
					if(line[j]=='"')count++;
					if(count==0){//apodw pernei to id
						while(line[j]!=','){
							n[j]=line[j];
							j++;
						}
						r[i]->id=atoi(n);
						printf("id: %d\n",r[i]->id);
					}
					if(count==1){//apo dw pernei to name
						j++;
						k=j;
						memset(n,0,20);
						while(line[j]!='"'){
							n[j-k]=line[j];
							j++;
						}
						j++;//prospernaei to "
						strcpy(r[i]->name,n);
						printf("name: %s\n",r[i]->name);
					}
					if(count==2){
						j++;
						k=j;
						memset(n,0,20);
						while(line[j]!='"'){
							n[j-k]=line[j];
							j++;
						}
						j++;//prospernaei to "
						strcpy(r[i]->surname,n);
						printf("surname: %s\n",r[i]->surname);

					}
					if(count==3){
						j++;
						k=j;
						memset(n,0,20);
						while(line[j]!='"'){
							n[j-k]=line[j];
							j++;
						}
						j++;//prospernaei to "
						strcpy(r[i]->city,n);
						printf("city: %s\n",r[i]->city);

					}				
					j++;
				}
				l=HP_InsertEntry(a,*r[i]);
				free(r[i]);
				i++;
			}
			fclose(fp);
			char value[20];
			strcpy(value,"Krishna");
			//HP_GetAllEntries(a,"name",&value);
			memset(value,0,20);

			strcpy(value,"Spradley");
			//HP_GetAllEntries(a,"surname",&value);
			memset(value,0,20);

			strcpy(value,"Kos");
			//HP_GetAllEntries(a,"city",&value);
			memset(value,0,20);

			//HP_GetAllEntries(a,"all",&value);

			strcpy(value,"2958979");
			//HP_GetAllEntries(a,"id",&value);
			printf("WEEW\n");
			int b =HP_CloseFile(a);
		}
	}
	return 0;
}




