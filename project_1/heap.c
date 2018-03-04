#include <stdio.h>
#include "BF.h"
#include "heap.h"

void HP_Init(){
  BF_Init();
}

int HP_CreateFile(char *fileName) {
  if(BF_CreateFile(fileName)==0){
    int bid=BF_OpenFile(fileName);
    if(bid>=0){
      char *text = "This is a heapFile\n"; //h eidikh plhroforia tou 1ou block
      if(BF_AllocateBlock(bid)==0){
        void* block;
        int x=BLOCK_SIZE;//to megethos tou block
        x=x-sizeof(text)-5;//to -5 einai to megethos twn byte p tha einai apothikeumena
        if (BF_ReadBlock(bid,0, &block)== 0){  
          char xx[4];
          sprintf(xx,"%d",x);
          strcpy(block,text);
          strcat(block,xx);
          strcat(block,"\n");//to block periexei "This is a heapFile\n1011\n"
          if (BF_WriteBlock(bid,0) < 0)
            BF_PrintError("Error writing block back");
          return 0;
        }
        else
          BF_PrintError("Error getting block");
        }
      else 
        BF_PrintError("Error allocating block");
    if(BF_CloseFile(bid)<0)
      BF_PrintError("Error closing file");
    }
    else
      BF_PrintError("Error opening file");

    }
    else{
      BF_PrintError("Error creating file");
    }

    return -1;

}



int HP_OpenFile(char *fileName) {
  int bid=BF_OpenFile(fileName);
  if(bid>=0){
    void* block;
    int blcnt=BF_GetBlockCounter(bid);
    if (BF_ReadBlock(bid,blcnt-1, &block)== 0){
      printf("\n\n%s \n",block);
      char array[18];
      strncpy(array,block,18);
      printf("%s \n",array);
      if(strstr(array,"This is a heapFile\n")==0)
        return bid;
      else
        printf("This is not a HeapFile\n");
    }
    else
          BF_PrintError("Error getting block");
  }
  else
      BF_PrintError("Error opening file");
  
  return -1;
}



int HP_CloseFile(int fileDesc) {
  if(BF_CloseFile(fileDesc)==0)
    return 0;
  return -1;
}

int HP_InsertEntry(int fileDesc, Record record) {
  char char_rec[49];
  char x[10];
  sprintf(x,"%d",record.id);
  strcpy(char_rec,x);
  strcat(char_rec,",");
  strcat(char_rec,record.name);
  strcat(char_rec,",");
  strcat(char_rec,record.surname);
  strcat(char_rec,",");
  strcat(char_rec,record.city);
  strcat(char_rec,"\n");
  printf("The new Record: %s\n",char_rec);

  int rec_size=sizeof(char_rec);
  void *block;
  int blcnt=BF_GetBlockCounter(fileDesc);
  if(blcnt==1){
    if(BF_AllocateBlock(fileDesc)==0){
      void * block;
      int xx=BLOCK_SIZE;
      xx-=rec_size;
      blcnt=BF_GetBlockCounter(fileDesc);
      if (BF_ReadBlock(fileDesc,blcnt-1, &block)== 0){
        char y[4];
        sprintf(y,"%d",xx); 
        strcpy(block,y);
        strcat(block,"\n");
        strcat(block,char_rec);
        printf("BLOCK:\n%s\n",block);
        if (BF_WriteBlock(fileDesc,blcnt-1) < 0){
            BF_PrintError("Error writing block back");
            return -1;
        }
        return 0;
      }
      else
        BF_PrintError("Error getting block");
    }
    else
      BF_PrintError("Error allocating block");
    
  }
  else{     //an dn exei mono ena block
    if (BF_ReadBlock(fileDesc,blcnt-1, &block)== 0){
      char text[BLOCK_SIZE];
      strcpy(text,block);
      char n[3];
      int i=0;
      while(text[i]!='\n'){
        n[i]=text[i];
        i++;
      }
      int remaining_size=atoi(n);
      if(remaining_size>=rec_size){
        remaining_size-=rec_size;
        memset(n,0,3);
        sprintf(n,"%d",remaining_size);
        i=0; 
        while(n[i]!='\0'){
          text[i]=n[i];
          i++;
        }
        if(text[i]!='\n')text[i]='\n';
        strcat(text,char_rec);
        strcpy(block,text);
        printf("BLOCK:\n%s\n",block);
        if (BF_WriteBlock(fileDesc,blcnt-1) < 0){
          BF_PrintError("Error writing block back");
          return -1;
        }
        return 0;
      }
      else{//se periptwsh p remaining size<recsize
        if(BF_AllocateBlock(fileDesc)==0){
          void * block;
          int xx=BLOCK_SIZE;
          xx-=rec_size;
          blcnt=BF_GetBlockCounter(fileDesc);
          if (BF_ReadBlock(fileDesc,blcnt-1, &block)== 0){
            char y[4];
            sprintf(y,"%d",xx); 
            strcpy(block,y);
            strcat(block,"\n");
            strcat(block,char_rec);
            printf("BLOCK:\n%s\n",block);
            if (BF_WriteBlock(fileDesc,blcnt-1) < 0){
              BF_PrintError("Error writing block back");
              return -1;
            }
            return 0;
          }
          else
            BF_PrintError("Error getting block");
        }
        else
          BF_PrintError("Error allocating block");
      }
    }
    else
        BF_PrintError("Error getting block");

    }
	return -1;
}






void HP_GetAllEntries(int fileDesc, char* fieldName, void *value) {
  char text[BLOCK_SIZE];
  int i,blcnt=BF_GetBlockCounter(fileDesc);
  void *block;
  for(i=1;i<blcnt;i++){
    memset(text,0,BLOCK_SIZE);
    if (BF_ReadBlock(fileDesc,i, &block)== 0){
      strcpy(text,block);
      int j=0;
      while(text[j]!='\n')j++;//gia na perasoume to size tou block
      j++;
      j++;
      int k=j;//kathe fora p tha allazoume grammh tha apothikeuetai to j sto k
      if(strcmp(fieldName,"id")==0){//1h periptwsh
        int count=0;
        while(text[j]!='\0'){
          if(text[j]=='\n'){
            count=0; //gia kathe grammh to count mhdenizetai
            k=j+1;
            j++;
          }
          if(count==0){
            if(text[j]=='\n')j++;
            int l=j;
            char n[5];
            while(text[j]!=','){
              n[j-l]=text[j];
              j++;
            }
            
            int i1=atoi(n);
           // printf("\nn=%s i1=%d ---value=%s\n",n,i1,value);
            if(i1!=0){//epeidh m ekane kt blkies!
              int i2=atoi(value);
              if(i1==i2){//printf("--------------------------%d-----%d------\n",i1,i2);
                l=k;
                if(text[l]=='\n')l++;
                while(text[l]!='\n'){
                  printf("%c",text[l]);//kanei print thn grammh
                  l++;
                }
                printf("\n");
              }
            }
            memset(n,0,15);
          }
          if(text[j]==',')count++;
          j++;
        }

      } 
    
      else if(strcmp(fieldName,"name")==0){
        int count=0;
        while(text[j]!='\0'){
          if(text[j]=='\n'){
            count=0; //gia kathe grammh to count mhdenizetai
            k=j+1;
          }
          if(count==1){//exei dei ena komma ara brhskomaste st epipedo name
            int l=j;
            char n[15];
            while(text[j]!=','){
              n[j-l]=text[j];
              j++;
            }
            if(strcmp(n,value)==0){
              l=k;
              while(text[l]!='\n'){
                printf("%c",text[l]);//kanei print thn grammh
                l++;
              }
              printf("\n");
            }
            memset(n,0,15);
          }
          if(text[j]==',')count++;
          j++;
        }
      }
      else if(strcmp(fieldName,"surname")==0){
        int count=0;
        while(text[j]!='\0'){
          if(text[j]=='\n'){
            count=0; //gia kathe grammh to count mhdenizetai
            k=j+1;
          }
          if(count==2){//exei dei duo kommata ara brhskomaste st epipedo surname
            int l=j;
            char n[20];
            while(text[j]!=','){
              n[j-l]=text[j];
              j++;
            }
            if(strcmp(n,value)==0){
              l=k;
              while(text[l]!='\n'){
                printf("%c",text[l]);//kanei print thn grammh
                l++;
              }
              printf("\n");
            }
            memset(n,0,20);
          }
          if(text[j]==',')count++;
          j++;
        }
      }
      else if(strcmp(fieldName,"city")==0){
        int count=0;
        while(text[j]!='\0'){         
          if(count==3){//exei dei tria kommata ara brhskomaste st epipedo city
            int l=j;
            char n[10];
            while(text[j]!='\n'){
              n[j-l]=text[j];
              j++;
            }
            if(strcmp(n,value)==0){
              l=k;
              while(text[l]!='\n'){
                printf("%c",text[l]);//kanei print thn grammh
                l++;
              }
              printf("\n");
            }
            memset(n,0,10);
            if(text[j]=='\n'){
              count=0; //gia kathe grammh to count mhdenizetai
              k=j+1;
            }
          }
          if(text[j]==',')count++;
          j++;
        }
      }
      else if(strcmp(fieldName,"all")==0){
        printf("%s\n",text);
      }
    }
    else
      BF_PrintError("Error getting block");
  }
  printf("Diabasthkan %d block\n",blcnt);
}







/* printf("FileDesc : %d\n",fileDesc);
    if(BF_ReadBlock(fileDesc,blcnt-1,&block)==0){
      printf("block=== %s\n",block);
      char text[BLOCK_SIZE];
      strcpy(text,block);
      int flag=0;
      char n[3];
      int j,remaining_size=0;
      for( remaining_size=0;remaining_size<strlen(text);remaining_size++){
        if(flag){
          if(text[remaining_size]!='\n'){
            printf("%c\n",text[remaining_size]);
            n[remaining_size-j]=text[remaining_size];            
          }
          else flag=0;
        }
        if(text[remaining_size]=='\n'){
          flag=1;
          j=remaining_size+1;
      }
    }
    remaining_size=atoi(n); //anakuklwsh metablitwn! :D 
    printf("dhmiourghsaaaa!!!!!! %s    %d\n",n,remaining_size);
    if(remaining_size>=rec_size){
      remaining_size-=rec_size;*/