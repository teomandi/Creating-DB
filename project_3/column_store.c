#include "BF.h"
#include "column_store.h"
#include <sys/stat.h>
#include <stdio.h>
#include <string.h>

struct stat st = {0};

void CS_Init(){
  BF_Init();
}

int CS_CreateFiles(char **fieldNames, char *dbDirectory) {          //to dbDirectory periexei to path oxi mono to onoma tou directory
  // Add your code here
  if (stat(dbDirectory, &st) == -1) {
    mkdir(dbDirectory, 0700);
    printf("The Directory created!\n" );
  }
  
  int i,bf,hbf;
  void *block, *block1;
  char path[80];
  strcpy(path, dbDirectory);
  strcat(path,"/header_info" );
  printf("PATH: %s\n",path );
  if ( BF_CreateFile(path) < 0){
    BF_PrintError("Error creating file");
    return -1;
  }
  if( (hbf = BF_OpenFile(path)) < 0){
    BF_PrintError("Error openning file");
    return -1;    
  }
  if( BF_AllocateBlock(hbf) < 0){
    BF_PrintError("Error allocating block");
    return -1;
  }
 if(BF_ReadBlock(hbf, 0,  &block) < 0) {
    BF_PrintError("Error getting block");
    return -1;
  }

  memset(path, 0, 50);
  char header_info[BLOCK_SIZE];
  for(i=0; i<6; i++){
    strcpy(path, dbDirectory);
    strcat(path, "/CSFile_");
    strcat(path, fieldNames[i]);
    if ( BF_CreateFile(path) < 0){
      BF_PrintError("Error creating file");
      return -1;
    }
    if( (bf = BF_OpenFile(path)) < 0){
      BF_PrintError("Error openning file");
      return -1;    
    }
    if( BF_AllocateBlock(bf) < 0){
      BF_PrintError("Error allocating block");
      return -1;
    }
    if(BF_ReadBlock(bf, 0,  &block1) < 0) {      // 0 giati g to ka8ena einai to prwto block
      BF_PrintError("Error getting block");
      return -1;
    }
    char title[15];
    strcpy(title,fieldNames[i]);
    strcpy((char*)block1, title);                    //first block created
    
    if (BF_WriteBlock(bf, 0) < 0){
      BF_PrintError("Error writing block back");
      return -1;
    }
    char temp[10];
    if( i==0 ){
      strcpy(header_info , path);
      sprintf(temp, " %d\n",bf);
      strcat(header_info,temp);
    }
    else{
      strcat(header_info , path);
      sprintf(temp, " %d\n",bf);
      strcat(header_info,temp);
    }
    memset(title,0,15);
    memset(path,0,50);
  }
  strcpy((char*)block, header_info);
  if (BF_WriteBlock(hbf, 0) < 0){
    BF_PrintError("Error writing block back");
    return -1;
  }
  if (BF_CloseFile(hbf) < 0) {
    BF_PrintError("Error closing file");
    return -1;
  }
  printf("HEADEr\n%s\n",header_info );
  return 0;
}



int CS_OpenFile(HeaderInfo* header_info, char *dbDirectory) {
  // Add your code here
  char path[50], temp[BLOCK_SIZE], line[100];
  int hbf,x,y,i, flag;
  void *block;
  strcpy(path, dbDirectory);
  strcat(path,"/header_info" );
  if( (hbf = BF_OpenFile(path)) < 0){
    BF_PrintError("Error openning file");
    return -1;    
  }
  if(BF_ReadBlock(hbf, 0,  &block) < 0) {
    BF_PrintError("Error getting block");
    return -1;
  }
  strcpy(temp,(char*)block);

  x=0, y=0, i=0, flag=0;
  while(temp[x] != '\0' ){
    while(temp[x] != '\n' && temp[x]!= '\0' && temp[x]!= ' ' ){
      line[y] = temp[x];
      //printf("%c\n",line[y] );
      y++;
      x++;
    }
    if(temp[x] == '\0') break;
    if(flag == 0){
      strcpy( header_info->column_fds[i].columnName , line);
      flag = 1;
    }
    else{
      header_info->column_fds[i].fd = atoi(line);
      flag = 0;
      i++;
    }
    x++;
    y=0;
    memset(line,0,100);
  }
  if (BF_CloseFile(hbf) < 0) {
    BF_PrintError("Error closing file");
    return -1;
  }
  int k;
  for(i=0;i<6;i++){
    memset(temp, 0 ,BLOCK_SIZE);
    if( (k = BF_OpenFile(header_info->column_fds[i].columnName)) < 0){
      BF_PrintError("Error openning file");
      return -1;    
    }
    printf("OPEN: to block counter: %d tou %d\n",BF_GetBlockCounter(header_info->column_fds[i].fd), header_info->column_fds[i].fd );
    if(BF_ReadBlock(k, 0,  &block) < 0) {
      BF_PrintError("Error getting block");
      return -1;
    }
    //<check>
    strcpy(temp, (char*)block);
    if(i==0){
      if( strcmp(temp, "id") !=0 ){
        printf("%s\n","Not a colum file" );
        return -1;
      }
    }
    else if(i == 1){
      if( strcmp(temp, "name") !=0 ){
        printf("%s\n","Not a colum file" );
        return -1;
      }
    }
    else if(i == 2){
      if( strcmp(temp, "surname") !=0 ){
        printf("%s\n","Not a colum file" );
        return -1;
      }
    }
    else if(i == 3){
      if( strcmp(temp, "status") !=0 ){
        printf("%s\n","Not a colum file" );
        return -1;
      }
    }
    else if(i == 4){
      if( strcmp(temp, "dateOfBirth") !=0 ){
        printf("%s\n","Not a colum file" );
        return -1;
      }
    }
    else if(i == 5){
      if( strcmp(temp, "salary") !=0 ){
        printf("%s\n","Not a colum file" );
        return -1;
      }
    }
    printf("OPEN'': to block counter: %d tou %d\n",BF_GetBlockCounter(header_info->column_fds[i].fd), header_info->column_fds[i].fd );
  }
  return 0;
}

int CS_CloseFile(HeaderInfo* header_info) {
  // Add your code here
  int i;
  for(i=0;i<6;i++){
    if(BF_CloseFile(header_info->column_fds[i].fd) < 0) {
      BF_PrintError("Error closing file");
      return -1;
    }
    printf("File:%d was closed succesfully!\n",header_info->column_fds[i].fd);
  }
  return 0;
}



/*bazei to value sto telos...an dn exei xwro to block kanei kenourgio
kathe timh teleiwnei se \n */
int CS_InsertEntry(HeaderInfo* header_info, Record record) {  
  char path[50],temp[BLOCK_SIZE],field[20];
  int i,bf;
  void *block;
  for(i=0;i<6;i++){
      bf = header_info->column_fds[i].fd;
      int blcnt ;
      blcnt=BF_GetBlockCounter(bf);
      if(blcnt<0){
        BF_PrintError("Error getting blockCounter");
        return -1;    
      }
      //printf("INSERT: to block counter: %d tou %d\n",blcnt, bf );       //ERROR Epistrefei 0 adi g ena who knows why?
      if(blcnt==1){   //se periptwsh p exei mono to block me thn eidikh plhroforia
        if( BF_AllocateBlock(bf) < 0){
          BF_PrintError("Error allocating block");
          return -1;
        }
        blcnt=BF_GetBlockCounter(bf);
      }
      if(BF_ReadBlock(bf, blcnt-1,  &block) < 0) {
        BF_PrintError("Error getting block");
        return -1;
      }
      strcpy(temp,(char*)block);
      switch(i){//topothetei sto field to katallhlo pedio kathe fora
      case 0 :
        sprintf(field,"%d",record.id);        
        break;
      case 1 :
        strcpy(field,record.name);
        break;
      case 2 :
        strcpy(field,record.surname);      
        break;
      case 3 :
        strcpy(field,record.status);   
        break;
      case 4 :
        strcpy(field,record.dateOfBirth);   
        break;
      case 5 :
        sprintf(field,"%d",record.salary);
        break;
      }
      strcat(field,"\n");
      if(strlen(temp)+strlen(field)<=BLOCK_SIZE){
        strcat(temp,field);//dn ta bala pio prin gt an toxa kanei me sugkrisei strlen(temp)<=blocksize tote tha htan lathos gt mono mexri blocksize apothukeuei to temp
        strcpy((char*)block,temp); 
      }
      else{//den xwraei ara kanoume allocate neo block
        printf("allocate at last\n");
        if( BF_AllocateBlock(bf) < 0){
          BF_PrintError("Error allocating block");
          return -1;
        }
        blcnt=BF_GetBlockCounter(bf);
        if(blcnt<0){
          BF_PrintError("Error getting blockCounter");
          return -1;    
        }
        if(BF_ReadBlock(bf, blcnt-1,  &block) < 0) {
          BF_PrintError("Error getting block");
          return -1;
        }
        strcpy((char*)block,field);
      }
      if (BF_WriteBlock(bf, blcnt-1) < 0){
        BF_PrintError("Error writting block back");
        return -1;
      }
      memset(temp,0,BLOCK_SIZE);
      memset(field,0,20);
      memset(field,0,50);
  }
  printf("%s\n","Added correctly" );

  //for(i=0;i<6;i++)
      //printf("INSERT': to block counter: %d tou %d\n",BF_GetBlockCounter(header_info->column_fds[i].fd), header_info->column_fds[i].fd );
  return 0;
} 



/*sthn ousia h logikh einai: briskei thn timh pou epithumoumai
k krataei th grammh p brethike..automatos paei sta alla arxeia p zhtountai analoga me ta pedia
k kanei print thn katallhlh grammh...meta sunexizei na brei k tis upoloipes times*/
void CS_GetAllEntries(HeaderInfo* header_info,char *fieldName,void *value,char **fieldNames,int n){
  int blcnt,i,line=0,bf,count=0;//tha exei ta block p diabasthkan
  void *block;
  char temp[BLOCK_SIZE],path[50];

  if(fieldName==NULL && value==NULL){
    
    for(i=0;i<6;i++){//gia ola ta arxeia
      bf = header_info->column_fds[i].fd;
      int blcnt ;
      blcnt=BF_GetBlockCounter(bf);
      if(blcnt<0){
        BF_PrintError("Error getting blockCounter");
        return -1;    
      }
      switch(i){//topothetei sto field to katallhlo pedio kathe fora
      case 0 :
        printf("ID\n");     
        break;
      case 1 :
        printf("NAME\n");     
        break;
      case 2 :
        printf("SURNAME\n");
        break;
      case 3 :
        printf("STATUS\n");    
        break;
      case 4 :
        printf("DATE OF BIRTH\n");           
        break;
      case 5 :
        printf("SALARY\n");     
        break;
      }
      count+=blcnt-1;//-1 gt einai to 1o block p dn to metrame
      int j;
      for(j=1;j<blcnt;j++){//to 0 exei thn eidikh plhroforia
        if(BF_ReadBlock(bf,j,  &block) < 0) {
          BF_PrintError("Error getting block");
          return -1;
        }
        strcpy(temp,(char*)block);
        printf("%s",temp);
        memset(temp,0,BLOCK_SIZE);
      }
    }
    
  }
  else if(strcmp(fieldName,"id")==0){
       bf = header_info->column_fds[0].fd;
      blcnt=BF_GetBlockCounter(bf);
      count+=blcnt-1;
        if(blcnt<0){
          BF_PrintError("Error getting blockCounter");
          return ;    
        }
        if(blcnt==1){
          BF_PrintError("There is no Entries");
          return ;
        }
        for(i=1;i<blcnt;i++){
          if(BF_ReadBlock(bf,i,  &block) < 0) {
            BF_PrintError("Error getting block");
            return ;
          }
          strcpy(temp,(char*)block);
          int j=0,k=0,val1;
          char val[5];
          while(temp[j]!='\0'){
            while(temp[j]!='\n'){//pernei thn timh
              val[k]=temp[j];
              k++;
              j++;
            }

            j++;//prospernaei to \n
            
            int val1=atoi(val);
            
            if(val1==(int)value){//tha paei se kathe pinaka k tha ektupwsei th grammh line
              int ii,*ar,jj=0;
             
              printf("id:%d\n",val1);
              ar=malloc(sizeof(n));
               
                if(strstr(fieldNames,"name")!=NULL){//briskei poia periexei
                  ar[jj]=1;
                  jj++;
                }
                if(strstr(fieldNames,"surname")!=NULL){
                  ar[jj]=2;
                  jj++;
                }
                if(strstr(fieldNames,"salary")!=NULL){
                  ar[jj]=5;
                  jj++;
                }

                if(strstr(fieldNames,"name")!=NULL){
                  ar[jj]=1;
                  jj++;
                }
                if(strstr(fieldNames,"dateOfBirth")!=NULL){
                  ar[jj]=4;
                  jj++;
                }
                   
              int bf2,blcnt2,line2,kk;
              char temp2[BLOCK_SIZE];
              void *block2;
              for(ii=0;ii<n;ii++){
                line2=0;
                bf2= header_info->column_fds[ar[ii]].fd;
                blcnt2=BF_GetBlockCounter(bf2);
                if(blcnt2<0){
                  BF_PrintError("Error getting blockCounter");
                  return ;
                }
                if(BF_ReadBlock(bf2,0,&block2) < 0){
                  BF_PrintError("Error getting block");
                return ;
                }count+=blcnt2-1;
                strcpy(temp2,(char*)block2);
                printf("%s:",temp2);//kanei print thn eidikh plhroforia t arxeiou
                memset(temp2,0,BLOCK_SIZE);
                char temp3[20];
                int flag=0;//an ginei 1 toxei kanei print k bgainei apo ta loop
                for(jj=1;jj<blcnt2;jj++){
                  if(BF_ReadBlock(bf2,jj,&block2) < 0) {
                    BF_PrintError("Error getting block");
                  return ;
                  }
                  strcpy(temp2,(char*)block2);
                  kk=0;

                  while(temp2[kk]!='\0' && flag==0){
                    if(line2==line){//einai st swsth grammh ara prepei na kanei print
                      int iii=0;
                      flag=1;
                      while(temp2[kk]!='\n'){
                        temp3[iii]=temp2[kk];
                        iii++;
                        kk++;
                      }
                      temp3[iii]='\0';
                      kk++;
                      flag=1;
                      printf("%s\n",temp3);
                    }
                    else{
                      while(temp2[kk]!='\n')kk++;
                      kk++;//prospernaei to \n
                      line2++;
                    }
                  }
                  if(flag)break;
                  memset(temp2,0,BLOCK_SIZE);
                }
                memset(temp3,0,20);
              }
              free(ar);
              printf("\n\n");
            }
            line++;
            k=0;
            memset(val,0,5);
          }
          memset(temp,0,BLOCK_SIZE);
        }
      }

    else if(strcmp(fieldName,"name")==0){
      bf = header_info->column_fds[1].fd;
      blcnt=BF_GetBlockCounter(bf);
        if(blcnt<0){
          BF_PrintError("Error getting blockCounter");
          return ;    
        }
        if(blcnt==1){
          BF_PrintError("There is no Entries");
          return ;
        }count+=blcnt-1;
        for(i=1;i<blcnt;i++){
          if(line>300)break;
          if(BF_ReadBlock(bf,i,  &block) < 0) {
            BF_PrintError("Error getting block");
            return ;
          }
          strcpy(temp,(char*)block);
          int j=0,k=0,val1;
          char val[15];
          while(temp[j]!='\0'){
            while(temp[j]!='\n'){//pernei thn timh
              val[k]=temp[j];
              k++;
              j++;
            }
            j++;//prospernaei to \n
            //printf("testing : %s\n",val);
            if(strcmp(val,(char*)value)==0){//tha paei se kathe pinaka k tha ektupwsei th grammh line
              int ii,*ar,jj=0;
              
              printf("name:%s\n",val);
              ar=malloc(sizeof(n));
               
                if(strstr(fieldNames,"id")!=NULL){//briskei poia periexei
                  ar[jj]=0;
                  jj++;
                }
                if(strstr(fieldNames,"surname")!=NULL){
                  ar[jj]=2;
                  jj++;
                }
                if(strstr(fieldNames,"salary")!=NULL){
                  ar[jj]=5;
                  jj++;
                }

                if(strstr(fieldNames,"status")!=NULL){
                  ar[jj]=3;
                  jj++;
                }
                if(strstr(fieldNames,"dateOfBirth")!=NULL){
                  ar[jj]=4;
                  jj++;
                }
                   
              int bf2,blcnt2,line2,kk;
              char temp2[BLOCK_SIZE];
              void *block2;
              for(ii=0;ii<n;ii++){
                line2=0;
                bf2= header_info->column_fds[ar[ii]].fd;
                blcnt2=BF_GetBlockCounter(bf2);
                if(blcnt2<0){
                  BF_PrintError("Error getting blockCounter");
                  return ;
                }count+=blcnt2-1;
                if(BF_ReadBlock(bf2,0,&block2) < 0){
                  BF_PrintError("Error getting block");
                return ;
                }
                strcpy(temp2,(char*)block2);
                printf("%s:",temp2);//kanei print thn eidikh plhroforia t arxeiou
                memset(temp2,0,BLOCK_SIZE);
                char temp3[20];
                int flag=0;//an ginei 1 toxei kanei print k bgainei apo ta loop
                for(jj=1;jj<blcnt2;jj++){
                  if(BF_ReadBlock(bf2,jj,&block2) < 0) {
                    BF_PrintError("Error getting block");
                  return ;
                  }
                  strcpy(temp2,(char*)block2);
                  kk=0;

                  while(temp2[kk]!='\0' && flag==0){
                    if(line2==line){//einai st swsth grammh ara prepei na kanei print
                      int iii=0;
                      flag=1;
                      while(temp2[kk]!='\n'){
                        temp3[iii]=temp2[kk];
                        iii++;
                        kk++;
                      }
                      temp3[iii]='\0';
                      kk++;
                      flag=1;
                      printf("%s\n",temp3);
                    }
                    else{
                      while(temp2[kk]!='\n')kk++;
                      kk++;//prospernaei to \n
                      line2++;
                    }
                  }
                  if(flag)break;
                  memset(temp2,0,BLOCK_SIZE);
                }
                memset(temp3,0,20);
              }
              free(ar);
              printf("\n\n");
            }
            line++;
            k=0;
            memset(val,0,15);
          }
          memset(temp,0,BLOCK_SIZE);
        }
      }

    else if(strcmp(fieldName,"surname")==0){ 
      bf = header_info->column_fds[2].fd;
      blcnt=BF_GetBlockCounter(bf);
        if(blcnt<0){
          BF_PrintError("Error getting blockCounter");
          return ;    
        }count+=blcnt-1;
        if(blcnt==1){
          BF_PrintError("There is no Entries");
          return ;
        }
        for(i=1;i<blcnt;i++){
          if(BF_ReadBlock(bf,i,  &block) < 0) {
            BF_PrintError("Error getting block");
            return ;
          }
          strcpy(temp,(char*)block);
          int j=0,k=0,val1;
          char val[20];
          while(temp[j]!='\0'){
            while(temp[j]!='\n'){//pernei thn timh
              val[k]=temp[j];
              k++;
              j++;
            }
            j++;//prospernaei to \n
            //printf("testing : %s\n",val);
            if(strcmp(val,(char*)value)==0){//tha paei se kathe pinaka k tha ektupwsei th grammh line
              int ii,*ar,jj=0;
              
              printf("surname:%s\n",val);
              ar=malloc(sizeof(n));
               
                if(strstr(fieldNames,"id")!=NULL){//briskei poia periexei
                  ar[jj]=0;
                  jj++;
                }
                if(strstr(fieldNames,"name")!=NULL){
                  ar[jj]=1;
                  jj++;
                }
                if(strstr(fieldNames,"salary")!=NULL){
                  ar[jj]=5;
                  jj++;
                }

                if(strstr(fieldNames,"status")!=NULL){
                  ar[jj]=3;
                  jj++;
                }
                if(strstr(fieldNames,"dateOfBirth")!=NULL){
                  ar[jj]=4;
                  jj++;
                }
                   
              int bf2,blcnt2,line2,kk;
              char temp2[BLOCK_SIZE];
              void *block2;
              for(ii=0;ii<n;ii++){
                line2=0;
                bf2= header_info->column_fds[ar[ii]].fd;
                blcnt2=BF_GetBlockCounter(bf2);
                if(blcnt2<0){
                  BF_PrintError("Error getting blockCounter");
                  return ;
                }count+=blcnt2-1;
                if(BF_ReadBlock(bf2,0,&block2) < 0){
                  BF_PrintError("Error getting block");
                return ;
                }
                strcpy(temp2,(char*)block2);
                printf("%s:",temp2);//kanei print thn eidikh plhroforia t arxeiou
                memset(temp2,0,BLOCK_SIZE);
                char temp3[20];
                int flag=0;//an ginei 1 toxei kanei print k bgainei apo ta loop
                for(jj=1;jj<blcnt2;jj++){
                  if(BF_ReadBlock(bf2,jj,&block2) < 0) {
                    BF_PrintError("Error getting block");
                  return ;
                  }
                  strcpy(temp2,(char*)block2);
                  kk=0;

                  while(temp2[kk]!='\0' && flag==0){
                    if(line2==line){//einai st swsth grammh ara prepei na kanei print
                      int iii=0;
                      flag=1;
                      while(temp2[kk]!='\n'){
                        temp3[iii]=temp2[kk];
                        iii++;
                        kk++;
                      }
                      temp3[iii]='\0';
                      kk++;
                      flag=1;
                      printf("%s\n",temp3);
                    }
                    else{
                      while(temp2[kk]!='\n')kk++;
                      kk++;//prospernaei to \n
                      line2++;
                    }
                  }
                  if(flag)break;
                  memset(temp2,0,BLOCK_SIZE);
                }
                memset(temp3,0,20);
              }
              free(ar);
              printf("\n\n");
            }
            line++;
            k=0;
            memset(val,0,20);
          }
          memset(temp,0,BLOCK_SIZE);
        }
      }
    else if(strcmp(fieldName,"status")==0){ 
      bf = header_info->column_fds[3].fd;
      blcnt=BF_GetBlockCounter(bf);
        if(blcnt<0){
          BF_PrintError("Error getting blockCounter");
          return ;    
        }count+=blcnt-1;
        if(blcnt==1){
          BF_PrintError("There is no Entries");
          return ;
        }
        for(i=1;i<blcnt;i++){
          if(BF_ReadBlock(bf,i,  &block) < 0) {
            BF_PrintError("Error getting block");
            return ;
          }
          strcpy(temp,(char*)block);
          int j=0,k=0,val1;
          char val[5];
          while(temp[j]!='\0'){
            while(temp[j]!='\n'){//pernei thn timh
              val[k]=temp[j];
              k++;
              j++;
            }
            j++;//prospernaei to \n
            //printf("testing : %s\n",val);
            if(strcmp(val,(char*)value)==0){//tha paei se kathe pinaka k tha ektupwsei th grammh line
              int ii,*ar,jj=0;
              printf("status:%s\n",val);
              
              ar=malloc(sizeof(n));
               
                if(strstr(fieldNames,"id")!=NULL){//briskei poia periexei
                  ar[jj]=0;
                  jj++;
                }
                if(strstr(fieldNames,"surname")!=NULL){
                  ar[jj]=2;
                  jj++;
                }
                if(strstr(fieldNames,"salary")!=NULL){
                  ar[jj]=5;
                  jj++;
                }

                if(strstr(fieldNames,"name")!=NULL){
                  ar[jj]=1;
                  jj++;
                }
                if(strstr(fieldNames,"dateOfBirth")!=NULL){
                  ar[jj]=4;
                  jj++;
                }
                   
              int bf2,blcnt2,line2,kk;
              char temp2[BLOCK_SIZE];
              void *block2;
              for(ii=0;ii<n;ii++){
                line2=0;
                bf2= header_info->column_fds[ar[ii]].fd;
                blcnt2=BF_GetBlockCounter(bf2);
                if(blcnt2<0){
                  BF_PrintError("Error getting blockCounter");
                  return ;
                }count+=blcnt2-1;
                if(BF_ReadBlock(bf2,0,&block2) < 0){
                  BF_PrintError("Error getting block");
                return ;
                }
                strcpy(temp2,(char*)block2);
                printf("%s:",temp2);//kanei print thn eidikh plhroforia t arxeiou
                memset(temp2,0,BLOCK_SIZE);
                char temp3[20];
                int flag=0;//an ginei 1 toxei kanei print k bgainei apo ta loop
                for(jj=1;jj<blcnt2;jj++){
                  if(BF_ReadBlock(bf2,jj,&block2) < 0) {
                    BF_PrintError("Error getting block");
                  return ;
                  }
                  strcpy(temp2,(char*)block2);
                  kk=0;

                  while(temp2[kk]!='\0' && flag==0){
                    if(line2==line){//einai st swsth grammh ara prepei na kanei print
                      int iii=0;
                      flag=1;
                      while(temp2[kk]!='\n'){
                        temp3[iii]=temp2[kk];
                        iii++;
                        kk++;
                      }
                      temp3[iii]='\0';
                      kk++;
                      flag=1;
                      printf("%s\n",temp3);
                    }
                    else{
                      while(temp2[kk]!='\n')kk++;
                      kk++;//prospernaei to \n
                      line2++;
                    }
                  }
                  if(flag)break;
                  memset(temp2,0,BLOCK_SIZE);
                }
                memset(temp3,0,20);
              }
              free(ar);
              printf("\n\n");
            }
            line++;
            k=0;
            memset(val,0,5);
          }
          memset(temp,0,BLOCK_SIZE);
        }
      }
    else if(strcmp(fieldName,"dateOfBirth")==0){ printf("k\n");sleep(1);
      bf = header_info->column_fds[4].fd;
      blcnt=BF_GetBlockCounter(bf);
        if(blcnt<0){
          BF_PrintError("Error getting blockCounter");
          return ;    
        }count+=blcnt-1;
        if(blcnt==1){
          BF_PrintError("There is no Entries");
          return ;
        }
        for(i=1;i<blcnt;i++){
          if(BF_ReadBlock(bf,i,  &block) < 0) {
            BF_PrintError("Error getting block");
            return ;
          }
          strcpy(temp,(char*)block);
          int j=0,k=0,val1;
          char val[11];
          while(temp[j]!='\0'){
            while(temp[j]!='\n'){//pernei thn timh
              val[k]=temp[j];
              k++;
              j++;
            }
            j++;//prospernaei to \n
            //printf("testing : %s\n",val);
            if(strcmp(val,(char*)value)==0){//tha paei se kathe pinaka k tha ektupwsei th grammh line
              int ii,*ar,jj=0;
              
              printf("dateOfBirth:%s\n",val);
              ar=malloc(sizeof(n));
               
                if(strstr(fieldNames,"id")!=NULL){//briskei poia periexei
                  ar[jj]=0;
                  jj++;
                }
                if(strstr(fieldNames,"surname")!=NULL){
                  ar[jj]=2;
                  jj++;
                }
                if(strstr(fieldNames,"salary")!=NULL){
                  ar[jj]=5;
                  jj++;
                }

                if(strstr(fieldNames,"status")!=NULL){
                  ar[jj]=3;
                  jj++;
                }
                if(strstr(fieldNames,"name")!=NULL){
                  ar[jj]=1;
                  jj++;
                }
                   
              int bf2,blcnt2,line2,kk;
              char temp2[BLOCK_SIZE];
              void *block2;
              for(ii=0;ii<n;ii++){
                line2=0;
                bf2= header_info->column_fds[ar[ii]].fd;
                blcnt2=BF_GetBlockCounter(bf2);
                if(blcnt2<0){
                  BF_PrintError("Error getting blockCounter");
                  return ;
                }count+=blcnt2-1;
                if(BF_ReadBlock(bf2,0,&block2) < 0){
                  BF_PrintError("Error getting block");
                return ;
                }
                strcpy(temp2,(char*)block2);
                printf("%s:",temp2);//kanei print thn eidikh plhroforia t arxeiou
                memset(temp2,0,BLOCK_SIZE);
                char temp3[20];
                int flag=0;//an ginei 1 toxei kanei print k bgainei apo ta loop
                for(jj=1;jj<blcnt2;jj++){
                  if(BF_ReadBlock(bf2,jj,&block2) < 0) {
                    BF_PrintError("Error getting block");
                  return ;
                  }
                  strcpy(temp2,(char*)block2);
                  kk=0;

                  while(temp2[kk]!='\0' && flag==0){
                    if(line2==line){//einai st swsth grammh ara prepei na kanei print
                      int iii=0;
                      flag=1;
                      while(temp2[kk]!='\n'){
                        temp3[iii]=temp2[kk];
                        iii++;
                        kk++;
                      }
                      temp3[iii]='\0';
                      kk++;
                      flag=1;
                      printf("%s\n",temp3);
                    }
                    else{
                      while(temp2[kk]!='\n')kk++;
                      kk++;//prospernaei to \n
                      line2++;
                    }
                  }
                  if(flag)break;
                  memset(temp2,0,BLOCK_SIZE);
                }
                memset(temp3,0,20);
              }
              free(ar);
              printf("\n\n");
            }
            line++;
            k=0;
            memset(val,0,11);
          }
          memset(temp,0,BLOCK_SIZE);
        }
      }

    else if(strcmp(fieldName,"salary")==0){
       bf = header_info->column_fds[5].fd;
      blcnt=BF_GetBlockCounter(bf);
        if(blcnt<0){
          BF_PrintError("Error getting blockCounter");
          return ;    
        }count+=blcnt-1;
        if(blcnt==1){
          BF_PrintError("There is no Entries");
          return ;
        }
        for(i=1;i<blcnt;i++){
          if(BF_ReadBlock(bf,i,  &block) < 0) {
            BF_PrintError("Error getting block");
            return ;
          }
          strcpy(temp,(char*)block);
          int j=0,k=0,val1;
          char val[5];
          while(temp[j]!='\0'){
            while(temp[j]!='\n'){//pernei thn timh
              val[k]=temp[j];
              k++;
              j++;
            }

            j++;//prospernaei to \n
            //printf("testing : %s\n",val);
            int val1=atoi(val);
            //printf("val:%s val1:%d value:%d\n",val,val1,(int)value );
            if(val1==(int)value){//tha paei se kathe pinaka k tha ektupwsei th grammh line
              int ii,*ar,jj=0;
              
              printf("salary:%d\n",val1);
              ar=malloc(sizeof(n));
               
                if(strstr(fieldNames,"name")!=NULL){//briskei poia periexei
                  ar[jj]=1;
                  jj++;
                }
                if(strstr(fieldNames,"surname")!=NULL){
                  ar[jj]=2;
                  jj++;
                }
                if(strstr(fieldNames,"id")!=NULL){
                  ar[jj]=0;
                  jj++;
                }

                if(strstr(fieldNames,"name")!=NULL){
                  ar[jj]=1;
                  jj++;
                }
                if(strstr(fieldNames,"dateOfBirth")!=NULL){
                  ar[jj]=4;
                  jj++;
                }
                   
              int bf2,blcnt2,line2,kk;
              char temp2[BLOCK_SIZE];
              void *block2;
              for(ii=0;ii<n;ii++){
                line2=0;
                bf2= header_info->column_fds[ar[ii]].fd;
                blcnt2=BF_GetBlockCounter(bf2);
                if(blcnt2<0){
                  BF_PrintError("Error getting blockCounter");
                  return ;
                }count+=blcnt2-1;
                if(BF_ReadBlock(bf2,0,&block2) < 0){
                  BF_PrintError("Error getting block");
                return ;
                }
                strcpy(temp2,(char*)block2);
                printf("%s:",temp2);//kanei print thn eidikh plhroforia t arxeiou
                memset(temp2,0,BLOCK_SIZE);
                char temp3[20];
                int flag=0;//an ginei 1 toxei kanei print k bgainei apo ta loop
                for(jj=1;jj<blcnt2;jj++){
                  if(BF_ReadBlock(bf2,jj,&block2) < 0) {
                    BF_PrintError("Error getting block");
                  return ;
                  }
                  strcpy(temp2,(char*)block2);
                  kk=0;

                  while(temp2[kk]!='\0' && flag==0){
                    if(line2==line){//einai st swsth grammh ara prepei na kanei print
                      int iii=0;
                      flag=1;
                      while(temp2[kk]!='\n'){
                        temp3[iii]=temp2[kk];
                        iii++;
                        kk++;
                      }
                      temp3[iii]='\0';
                      kk++;
                      flag=1;
                      printf("%s\n",temp3);
                    }
                    else{
                      while(temp2[kk]!='\n')kk++;
                      kk++;//prospernaei to \n
                      line2++;
                    }
                  }
                  if(flag)break;
                  memset(temp2,0,BLOCK_SIZE);
                }
                memset(temp3,0,20);
              }
              free(ar);
              printf("\n\n");
            }
            line++;
            k=0;
            memset(val,0,5);
          }
          memset(temp,0,BLOCK_SIZE);
        }
      }
  printf("%d blocks was read\n",count);
}