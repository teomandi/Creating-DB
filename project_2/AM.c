/* gcc AM.c main.c  BF_64.a*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h> //gia to F_OK st acces
#include <string.h>
#include <ctype.h>
#include "defn.h"
#include "AM.h"
#include "BF.h"
#include "stack.c"


int AM_errno;

typedef struct S_info{//oi plhrofories gia ta arxeia
  int idD;
  int id;//an xreisimopoihtai h oxi
  int op;
  void *value;
  int f_index;
  char type[3];
  }S_info;

S_info s_info[MAXSCANS];

typedef struct F_info{//oi plhrofories gia ta arxeia
  int id;
  char file_name[10];
  char type[3];
  int root;
}F_info;

F_info f_info[MAXOPENFILES];

void AM_Init( void ){
  BF_Init();
  int i;
  for(i=0;i<MAXOPENFILES;i++){
    f_info[i].id=-1;
    f_info[i].root = -1;
  }
    for(i=0;i<MAXSCANS;i++){
    s_info[i].id=-1;
    }
}


int AM_CreateIndex(char *fileName, char attrType1, int attrLength1, char attrType2, int attrLength2 ){

  int bf;
  void *block;

 if( access( fileName, F_OK ) != -1 ) {
  printf("The file %s exist\n",fileName);
   bf = remove(fileName);
   if(!bf) printf("The file deleted succesfully");
  }

  if ( BF_CreateFile(fileName) < 0){
    BF_PrintError("Error creating file");
    AM_errno = create;
    return create;
  }

  if( (bf = BF_OpenFile(fileName)) < 0){
    BF_PrintError("Error openning file");
    AM_errno = create;
    return create;    
  }

  if( BF_AllocateBlock(bf) < 0){
    BF_PrintError("Error allocating block");
    AM_errno = create;
    return create;
  }

  if (BF_ReadBlock(bf, 0,  &block) < 0) {
    BF_PrintError("Error getting block");
    AM_errno = create;
    return create;
  }
  char str[2+attrLength2+attrLength1],str1[attrLength1],str2[attrLength2],str3[1];
  printf("%c\n",attrType1);

  str3[0]=attrType1;
  strcpy(str,str3);
  sprintf(str1,"%d",attrLength1);
  strcat(str,str1);
  memset(str3,0,1);
  str3[0]=attrType2;
  sprintf(str2,"%d",attrLength2);
  strcat(str,str3);
  strcat(str,str2);
  strcat(str,"\n");
  strcpy((char*)block,str);//apothikeuete st morfh "i4i4"
  
  printf("teliko sto block = %s\n",(char*)block);
  if (BF_WriteBlock(bf, 0) < 0){
    BF_PrintError("Error writing block back");
    AM_errno = create;
    return create ;
  }
  if (BF_CloseFile(bf) < 0) {
    BF_PrintError("Error closing file");
    AM_errno = create;
    return create ;
  }
  return AME_OK;
}




int AM_OpenIndex (char *fileName){
  int bf, i ;
  for(i=0;i<MAXOPENFILES;i++)
    if(f_info[i].id==-1)break;
  if( (bf = BF_OpenFile(fileName))<0){
    BF_PrintError("Error opening file");
    AM_errno=open;
    return open;
  }
  for(i=0;i<MAXSCANS;i++){
    if(s_info[i].id!=-1){
      AM_errno=open;
      return open;

    }
  }
  f_info[i].id=bf;
  strcpy(f_info[i].file_name,fileName);
  void* block;
  if (BF_ReadBlock(bf, 0,  &block) < 0) {
    BF_PrintError("Error getting block");
    AM_errno = create;
    return open;
  }
  int j=0,k;
  char temp[strlen((char*)block)];
  strcpy(temp,(char*)block);
  for(k=0;k<strlen(temp);k++){
    if(j==2)break;
    if(isalpha(temp[k])){
      f_info[i].type[j]=temp[k];
      j++;
    }
  }
  f_info[i].type[j]='\0';

  printf("%d  %s %s bf file\n",bf,f_info[i].file_name,f_info[i].type);
  return i;
}


int AM_CloseIndex (int fileDesc ){
  printf("inside close %d\n",f_info[fileDesc].id);
  if (BF_CloseFile(f_info[fileDesc].id) < 0) {//diagrafei to me bash ton arithmo p epistrefei g bf_open
    printf("close: gamithike\n");
    BF_PrintError("Error closing file");
    AM_errno= close ;
    return close;

  }
    if(s_info[fileDesc].id!=-1){ 
      AM_errno= close ;
      return close;
  }
  f_info[fileDesc].id = -1 ;
  memset( f_info[fileDesc].file_name ,0,10);//to adeiazei
  printf("close :dn gamithike\n");
  return AME_OK;

}



int AM_DestroyIndex( char *fileName ){
  int i;
  for(i=0;i<MAXOPENFILES;i++){
    if(strcmp(f_info[i].file_name,fileName)==0){//denxreiazetai na chekaroume gia to scan files gt an exei ginei swsta close tote to s_info einai -1
      printf("the file is not Closed\n");
      BF_PrintError("Error Destroying file");
      AM_errno=destroy;
      return destroy;
    }
  }
  if( access( fileName, F_OK ) != -1 ){
      if(s_info[i].id!=-1){
        AM_errno=destroy;
        printf("There are opened scanned files\n");
        return destroy;
      }
    }
    if(!remove(fileName)){
      printf("The file deleted succesfully\n");
      return AME_OK;
    }
    AM_errno=destroy;
    return destroy;
}






int AM_InsertEntry(int fileDesc, void *value1, void *value2){
  printf("INSERT\n");
  if(f_info[fileDesc].id==-1){
    printf("Error in Insert: the file is not opned\n");
    AM_errno=insert;
    return insert;
  }
  /*CREATIN CHAR* OF to INSERT  dataBLOCK            TEMP0 EINAI TO DATA MAS*/
  char temp0[519],temp1[256];//254 gt to c pernei times apo 1-256 ,,,516 gt borei na boune 2*256+4"'"+'\n'+"d\n"(sthn arxh tou block) (1bit to kathena)
  
  if(f_info[fileDesc].type[0]=='c')strcpy(temp1,(char*)value1);
  else if(f_info[fileDesc].type[0]=='i')sprintf(temp1,"%d",(int)value1);
  else if(f_info[fileDesc].type[0]=='f')sprintf(temp1,"%f",(float*)value1);
  strcpy(temp0,temp1);      //me cat kanei seg fault
  strcat(temp0,",");
  memset(temp1,0,256);
  if(f_info[fileDesc].type[1]=='c')strcpy(temp1,(char*)value2);
  else if(f_info[fileDesc].type[1]=='i')sprintf(temp1,"%d",(int)value2);
  else if(f_info[fileDesc].type[1]=='f')sprintf(temp1,"%f",(float*)value2);
  strcat(temp0,temp1);
  strcat(temp0,"\n");

  char s[2] ="\n";
  char line[513];       //2 max string megethous 256 to ka8ena k ena komma
  int blcnt,x,y;
  void *block;
  if(f_info[fileDesc].root==-1){          //an dn uparxei riza prepei na ftia3oume to prwto data block mas
    if( BF_AllocateBlock(f_info[fileDesc].id) < 0){
      BF_PrintError("Error allocating block");
      AM_errno = insert;
      return insert;
    }
    
    blcnt=BF_GetBlockCounter(f_info[fileDesc].id);
    if (BF_ReadBlock(f_info[fileDesc].id ,blcnt-1, &block)< 0){
      BF_PrintError("Error getting block");
      AM_errno= insert;
      return insert;
    }
    char temp3[BLOCK_SIZE];
    strcpy(temp3,"d\n");          //ka8e datablock 3ekinaei me d stin arxh
    strcat(temp3,temp0);
    strcpy((char*)block,temp3);
    if (BF_WriteBlock(f_info[fileDesc].id,blcnt-1) < 0){
      BF_PrintError("Error writing block back");
      AM_errno = insert;
      return insert;
    }
    f_info[fileDesc].root=blcnt-1;
  }
  else{
    Stack *S = (Stack*)malloc(sizeof(Stack));
    S->size = 0;
    int prosorinos = f_info[fileDesc].root;
    /* XEKINAME */
    if (BF_ReadBlock(f_info[fileDesc].id ,prosorinos, &block)< 0){
      BF_PrintError("Error getting block");
      AM_errno= insert;
      return insert;
    }
    char temp[BLOCK_SIZE];
    strcpy(temp,(char*)block);

    while( temp[0]!='d'){                 //mexri na broume datablock
      printf("temppp!!! \n%s\n",temp);
      Stack_Push(S, prosorinos);
      x=0;
      int flag1,j,i=0,flag2=0;//to falg 2 ginetai 1 otan brhseki p na kateuthinthei
      char str1[256], str2[256];
      while( temp[x] != '\0' ){
        flag1=0;
        memset(str1,0,256);
        memset(str2,0,256);
        while(temp[x]!='\n' && temp[x]!='\0'){
          if(temp[x]==','){
            flag1=1;
            x++;//prospernaei to ,
            j=i;
          }
          if(temp[x]=='\0')break;
          if(!flag1)str1[i]=temp[x];
          else str2[i-j]=temp[x];
          i++;
          x++;
        }
        x++;//prospernaei to \n
        if(strlen(str2)==0)break;
        if(f_info[fileDesc].type[0]=='c'){
            if(strcmp((char*)value1, str2) <= 0){            
              prosorinos = atoi(str1);
              flag2=1;
              break;
            }
          }
          else if(f_info[fileDesc].type[0]=='i'){
            if((int)value1 < atoi(str2)){
            prosorinos = atoi(str1);
            flag2=1;
            break;
            }
          }
        /*else if(f_info[fileDesc].type[0]=='f'){
            if( (float*)value1 < (float)atof(str2)){
              flag2=1;
              prosorinos = atoi(str1);
              break;
            }
          }*/
        i=0;
      }
      if(!flag2)prosorinos=atoi(str1);//ston teleutaio pointer
      if (BF_ReadBlock(f_info[fileDesc].id ,prosorinos, &block)< 0){
        BF_PrintError("Error getting block");
        AM_errno= insert;
        return insert;
      }
      memset(temp,0,BLOCK_SIZE);
      strcpy(temp,(char*)block);
    }
    if (strstr(temp, temp0) != NULL) {
      printf("The value already exist in the tree\n");      //elegxos ama uparxei idia eisagogh
      AM_errno= insert;
      return insert;
    }
    int length = strlen(temp)+strlen(temp0);
    if(length <= BLOCK_SIZE){                         //an xwraei to eisagei                                  //--------------------------------------------------------------------------------salam
      char * temptemp;
      strcat(temp,temp0);
      temptemp = sort(temp, f_info[fileDesc].type[0]);
      memset(temp,0,BLOCK_SIZE);
      strcpy(temp,temptemp);
      printf("temp after sort\n%s\n",temp);
      
      strcpy((char*)block,temp);

      if (BF_WriteBlock(f_info[fileDesc].id, prosorinos) < 0){
        BF_PrintError("Error writing block back");
        AM_errno= insert;
        return insert;
      }
    }
    else{                                         //alliws split to block
      //printf("%s\n","split!?!??!!?!?" );

      char PROS[length];
      char temp3[BLOCK_SIZE];
      strcpy(temp3,"d\n");
      strcpy(PROS,temp);
      strcat(PROS,temp0);
      char * PROSpros;

      PROSpros = sort(PROS, f_info[fileDesc].type[0]);
      strcpy(PROS,PROSpros);
      //printf("PROS after sort\n%s\n",PROS);
      memset(temp,0,BLOCK_SIZE);
      int pleaf = -1;                         //not counting "d" (first line)
      x=0;
      while( PROS[x] != '\0' ) {
        while(PROS[x] != '\n'){
          x++;
        }
        x++; ;
        pleaf++;  
      }
      if( BF_AllocateBlock(f_info[fileDesc].id) < 0){
        BF_PrintError("Error allocating block");
        AM_errno= insert;
        return insert;
      }
      blcnt=BF_GetBlockCounter(f_info[fileDesc].id);
      int new = blcnt-1;
      sprintf(temp, "d%d\n",new );                  //gia to neo Data block
      int i = 0;
      int j = (pleaf+1)/2;                          //apo th 1h mexri k thn j grammh sto prwto ta epomena sto epomeno
      char willneed[513];
      x = 0;
      y = 0;
      while(PROS[x] != '\0' ) {
        while(PROS[x]!='\n'){
          line[y] = PROS[x];
          x++;
          y++;
        }
        if(i!= 0){
          if(i < j){
           strcat(temp, line);
           strcat(temp,"\n");
          }
          else if(i==j){
            strcat(temp, line);
            strcat(temp,"\n");
            strcpy(willneed , line);                //8ane h timi tou index block
          }
          else{
            strcat(temp3, line);
            strcat(temp3,"\n");
          }
        } 
        i++;
        y=0;
        x++;
        memset(line,0,513);
      }
      strcpy((char*)block, temp);
      if (BF_WriteBlock(f_info[fileDesc].id, prosorinos) < 0){
        BF_PrintError("Error writing block back");
        AM_errno= insert;
        return insert;
      }
      if (BF_ReadBlock(f_info[fileDesc].id ,new, &block)< 0){
        BF_PrintError("Error getting block");
        AM_errno= insert;
        return insert;
      }
      strcpy((char*)block,temp3);
      if (BF_WriteBlock(f_info[fileDesc].id, new) < 0){
        BF_PrintError("Error writing block back");
        AM_errno= insert;
        return insert;
      }
      int FINISHED = 1;
      while(FINISHED){                                           
        char temp2[BLOCK_SIZE];
        int i = 0;
        char need[256];
        while(willneed[i] != '\0'){
          if(willneed[i]==',')
            break;
          need[i] = willneed[i];                                        //h timi p 8a mpei sto index block
          i++;
        }
        if(S->size == 0){                                               //stoiva kenh ara dn uparxoun index blocks
          if( BF_AllocateBlock(f_info[fileDesc].id) < 0){
            BF_PrintError("Error allocating block");
            AM_errno= insert;
            return insert;
          }
          blcnt=BF_GetBlockCounter(f_info[fileDesc].id);
          f_info[fileDesc].root = blcnt-1;
          if (BF_ReadBlock(f_info[fileDesc].id ,blcnt-1, &block)< 0){
            BF_PrintError("Error getting block");
            AM_errno= insert;
            return insert;
          }
          
          sprintf(temp2, "%d,%s\n%d,", prosorinos , need , new);       
          strcpy((char*) block, temp2);
          if (BF_WriteBlock(f_info[fileDesc].id, blcnt-1) < 0){
            BF_PrintError("Error writing block back");
            AM_errno= insert;
            return insert;
          }
          FINISHED = 0;
        }
        else {
          int index = Stack_Top(S);                                   //pernoume to akrivws panw index block 
          Stack_Pop(S);        
          if (BF_ReadBlock(f_info[fileDesc].id ,index, &block)< 0){
            BF_PrintError("Error getting block");
            AM_errno= insert;
            return insert;
          }
          strcpy(temp2, (char*) block);
          char add[232];                          //(,)+argument+\n+integer
          sprintf(add, "%s\n%d", need , new);
          int sum = strlen(temp2) +strlen(add);
          char value[256];
          if(sum <= 30 ){                                  //ama xwraei eisagei ekei sthn swsti 8esh ---------------------------------------blocksize
            int i = 0, flag=1;
            x = 0;
            y = 0;
            
            char t0[BLOCK_SIZE];

            while( temp2[x]!= '\0' ){                      //epektinei to index block me tis nees times dioti spasame to datablock
              while(temp2[x] !=',' && temp2[x]!='\0'){
                line[y]=temp2[x];
                x++;
                y++;
              }
              
              if(i==0){                                     //ekxwrei to prwto deikth k to komma
                strcpy(t0,line);
                strcat(t0, ",");
              }
              if(i!=0){
                int j=0;
                while(line[j] != '\n'){
                  value[j] = line[j];
                  j++;
                }
          
                //comp value me need
                if(strcmp(value, need) > 0  && flag ){//brhskei pou prepei na bei k to eisagei ..an flag 0 dn 3anabenei
                  strcat(t0,add);
                  strcat(t0, ",");
                  strcat(t0,line);
                  strcat(t0, ",");
                  flag=0;
                }
                else{//sunexizei na to gemizei
                  strcat(t0, line);
                  strcat(t0, ",");
                }
              }
              i++;
              y=0;
              x++;//prospernaei to komma
              memset(line,0,513);
            }
            if(flag)strcat(t0,add);                     //an dn bale pouthena to bazei st telos
            strcpy((char*)block, t0);
            if (BF_WriteBlock(f_info[fileDesc].id, index) < 0){
              BF_PrintError("Error writing block back");
              AM_errno= insert;
              return insert;
            }
            FINISHED = 0;
          }
          else{//spaei to index block
            char PROS1[sum];
            int i = 0, flag=1;
            x=0;
            y=0;
            while( temp2[x] != '\0' ) {                         //ftiaxeni to neo index block me t kainouria timh ta3inomimena
              while(temp2[x] != ',' && temp2[x]!='\0'){ 
                line[y]=temp2[x];
                y++;
                x++;
              }
              if(i==0){       
                strcpy(PROS1,line);
                strcat(PROS1, ",");
              }
              if(i!=0){
                int j=0;
                while(line[j] != '\n'){
                  value[j] = line[j];
                  j++;
                }                                                                 //comp value me need
                if(strcmp(value, need) > 0  && flag ){
                  strcat(PROS1,add);
                  strcat(PROS1, ",");
                  strcat(PROS1,line);
                  strcat(PROS1, ",");
                  flag=0;
                }
                else{
                  strcat(PROS1, line);
                  strcat(PROS1, ",");
                }
              }
              i++;
              x++;
              y=0;
              memset(line,0,513);
            }
            if(flag){                                                        // se periptwsh p dn exei mpei baltw sto telos
              strcat(PROS1,add); 
              strcat(PROS1,",");
            }    
            //to pros1 exei to str p 8eloume... to i exei to plh8os twn pointers
            int j = (i+1)/2, k=0;
            char t1[BLOCK_SIZE], t2[BLOCK_SIZE];
            flag =1;
            int f =0;
            x=0;
            y=0;
            while( PROS1[x] != '\0' ) {
              while(PROS1[x] != ',' && PROS1[x]!='\0'){     
                line[y]=PROS1[x];
                y++;
                x++;
              }
              if(k < j){                                                    //split tou prosorinou sta t1,t2 opws prepei nane domimena
                if(k == 0){ 
                  strcpy(t1,line);
                  strcat(t1, ",");
                }
                else{
                  strcat(t1,line);
                  strcat(t1, ",");
                }
              }
              else{
                if(flag){
                  flag = 0;
                  char tempo[256];                  
                  int xx=0, yy=0;
                  while( line[xx]!='\0'){
                    while(line[xx]!='\n' && line[xx]!='\0'){
                      tempo[yy]=line[xx];
                      xx++;
                      yy++;
                    }                   
                    if(!f){
                      strcpy(need, tempo);                          //to need opws prin g mellontiki xrhsh
                      xx++;
                      yy=0;
                      memset(tempo,0,256);
                    } 
                    else{
                      strcpy(t2, tempo);
                      strcat(t2, ",");
                    }
                    f = 1;
                  }
                }
                else{
                  strcat(t2, line);
                  strcat(t2, ",");
                } 
              }
              k++;
              x++;
              y=0;
              memset(line,0,513);
            }
            strcpy((char*)block, t1);
            if (BF_WriteBlock(f_info[fileDesc].id, index) < 0){
              BF_PrintError("Error writing block back");
              AM_errno= insert;
              return insert;
            }
            if( BF_AllocateBlock(f_info[fileDesc].id) < 0){
              BF_PrintError("Error allocating block");
              AM_errno= insert;
              return insert;
            }
            blcnt=BF_GetBlockCounter(f_info[fileDesc].id);
            if (BF_ReadBlock(f_info[fileDesc].id ,blcnt-1, &block)< 0){
              BF_PrintError("Error getting block");
              AM_errno= insert;
              return insert;
            }
            strcpy((char*)block, t2);
            if (BF_WriteBlock(f_info[fileDesc].id, blcnt-1) < 0){
              BF_PrintError("Error writing block back");
              AM_errno= insert;
              return insert;
            }
            prosorinos = index;
            new = blcnt-1;
          }
        } 
      }
    }
    free(S);
    printf("%s\n","END INSERT" );
  }
}   
    


int AM_OpenIndexScan(int fileDesc, int op, void *value ){
  if(f_info[fileDesc].id==-1){
    printf("Error in OpenIndexScan: the file is not opned\n");
    AM_errno=openscan;
    return openscan;
  }
  printf("root:%d\n",f_info[fileDesc].root);
  if(f_info[fileDesc].root==-1){
    printf("Error in OpenIndexScan: the file has no root\n");
    AM_errno=openscan;
    return openscan;
  }
  int iii,fl=0;
  for(iii=0;iii<MAXSCANS;iii++){
    if(s_info[iii].id==-1){//brhskei to shmeio tou pinaka pou tha kataxwrithei to scan
      fl=1;
      break;
    }
  }
  if(fl==0){//se periptwsh p dn brethike
    printf("There is already %d files scanned\n",MAXSCANS);
    AM_errno=openscan;
    return openscan;
  }
  s_info[iii].value=value;
  s_info[iii].op=op;//arxikopoihseis
  strcpy(s_info[iii].type,f_info[fileDesc].type);
  s_info[iii].idD = f_info[fileDesc].id;

  void *block;
  if (BF_ReadBlock(fileDesc ,f_info[fileDesc].root, &block)< 0){
    BF_PrintError("Error getting block");
    AM_errno=openscan;
    return openscan;
  }
  char temp[BLOCK_SIZE];
  strcpy(temp,(char*)block);
    int flag2,flag1,kk,k,point,x=0;
  while(temp[0]!='d'){
    s_info[iii].f_index=x;//o anagnwristikos arithmos tou index block...tha ton xreiastoume gia thn epomenh snarthsh
    int lines=0,j=0;
    while(temp[j]!='\0'){//brhskei tis grammes
      if(temp[j]=='\n')lines++;
      j++;
    }
    char char_val[256],temp1[256];
    flag1=0;
    k=0;kk=0;

    for(j=0;j<lines-1;j++){//ekxwrei to periexomeno tou index block stous pinakes
      int l=0;
      while(temp[k]!='\n'){
        if(temp[k]==','){
          point=atoi(temp1);
          l=0;
          memset(temp1,0,256);//to xrisimopoioume gia to 2o stoixeio
          k++;//prospername to komma
        }
        temp1[l]=temp[k];
        l++;
        k++;    
      }
      k++;//prospernaei to \n
      int flag=0;//an to brei ginetai 1
      if(op==1){
        if((temp,f_info[fileDesc].type[0])=='i'){
          if((int)value<atoi(temp1)){//brhke to swsto pointer
            flag1=1;
            x=point;
            break;
          }
          else if((int)value>atoi(temp1)){
            continue;
          }
          else if((int)value==atoi(temp1)){
            flag=1;
            memset(temp1,0,256);
            l=0;
            while(temp[k]!=','){
              temp1[l]=temp[k];
              k++;
              l++;
            }
            x=atoi(temp1);
            break;
          }
        }
        else if((temp,f_info[fileDesc].type[0])=='c'){
          if (strcmp((char*)value,temp1)>0){//brhke to swsto pointer
            flag1=1;
            x=point;
            break;
          }
          else if(strcmp((char*)value,temp1)<0){
            continue;
          }
          else if (strcmp((char*)value,temp1)==0){
            flag=1;
            memset(temp1,0,256);
            l=0;
            while(temp[k]!=','){
              temp1[l]=temp[k];
              k++;
              l++;
            }
            x=atoi(temp1);
            break;
          }
        }
       /* else if((temp,f_info[fileDesc].type[0])=='f'){
          if((float*)value<atof(temp1)){//brhke to swsto pointer
            flag1=1;
            x=point;
            break;
          }
          else if((float)value>atof(temp1)){
            continue;
          }
          else if((float)value==atof(temp1)){
            flag=1;
            memset(temp1,0,256);
            l=0;
            while(temp[k]!=','){
              temp1[l]=temp[k];
              k++;
              l++;
            }
            x=atoi(temp1);
            break;
          }
        }*/

      }
      if(op==2){
        //tha ekteleitai st find next entry
      }
      if(op==3 || op==5){
        if((temp,f_info[fileDesc].type[0])=='i'){
          if((int)value<atoi(temp1)){//brhke to swsto pointer
            flag1=1;
            x=point;
            break;
          }
          else if((int)value>atoi(temp1)){
            continue;
          }
          else if((int)value==atoi(temp1)){
            flag1=1;
            x=point;
            break;
        }
      }
        else if((temp,f_info[fileDesc].type[0])=='c'){
          if (strcmp((char*)value,temp1)>0){//brhke to swsto pointer
            flag1=1;
            x=point;
            break;
          }
          else if(strcmp((char*)value,temp1)<0){
            continue;
          }
          else if (strcmp((char*)value,temp1)==0){
            flag1=1;
            x=point;
            break;
        }
      }
     /*   else if((temp,f_info[fileDesc].type[0])=='f'){
          if((float)value<atof(temp1)){//brhke to swsto pointer
            flag1=1;
            x=point;
            break;
          }
          else if((float)value>atof(temp1)){
            continue;
          }
          else if((float)value==atof(temp1)){
            flag1=1;
            x=point;
            break;
        }

      }*/
    }
    else if(op==4 || op==6){//oloidio me to ==1~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
        if((temp,f_info[fileDesc].type[0])=='i'){
          if((int)value<atoi(temp1)){//brhke to swsto pointer
            flag1=1;
            x=point;
            break;
          }
          else if((int)value>atoi(temp1)){
            continue;
          }
          else if((int)value==atoi(temp1)){
            flag=1;
            memset(temp1,0,256);
            l=0;
            while(temp[k]!=','){
              temp1[l]=temp[k];
              k++;
              l++;
            }
            x=atoi(temp1);
            break;
          }
        }
        else if((temp,f_info[fileDesc].type[0])=='c'){
          if (strcmp((char*)value,temp1)>0){//brhke to swsto pointer
            flag1=1;
            x=point;
            break;
          }
          else if(strcmp((char*)value,temp1)<0){
            continue;
          }
          else if (strcmp((char*)value,temp1)==0){
            flag=1;
            memset(temp1,0,256);
            l=0;
            while(temp[k]!=','){
              temp1[l]=temp[k];
              k++;
              l++;
            }
            x=atoi(temp1);
            break;
          }
        }
     /*   else if((temp,f_info[fileDesc].type[0])=='f'){
          if((float)value<atof(temp1)){//brhke to swsto pointer
            flag1=1;
            x=point;
            break;
          }
          else if((float)value>atof(temp1)){
            continue;
          }
          else if((float)value==atof(temp1)){
            flag=1;
            memset(temp1,0,256);
            l=0;
            while(temp[k]!=','){
              temp1[l]=temp[k];
              k++;
              l++;
            }
            x=atoi(temp1);
            break;
          }
        }*/


    }
      memset(temp1,0,256);
    }
    if(!flag1){//se periptwsh p dn brethike paei st teleutaio index
      k++;//gia na perasei to \n
      int l=0;
      while(temp[k]!='\n'){
        temp1[l]=temp[k];
        l++;
        k++;
      }
      x=atoi(temp1);
    }
    memset((char*)block,0,BLOCK_SIZE);
    memset(temp,0,BLOCK_SIZE);
    if (BF_ReadBlock(fileDesc ,x, &block)< 0){
      BF_PrintError("Error getting block");
      AM_errno=openscan;
      return openscan;
    }
    strcpy(temp,(char*)block);
  }// exei brei to datablock!!....pai3e me t op.
  s_info[iii].id=x;
  return iii;
}








void *AM_FindNextEntry(int scanDesc ){
  void *block;
  int x,y,z,flag, f;
  char temp[BLOCK_SIZE];
  char v1[256], v2[256];
  char v[10];
  if(s_info[scanDesc].id == -1 ){
    printf("%s\n","Not in the scan array" );
    //AM_erno = findnext;
    return (int)findnext;
  }
  int epomenh =0;         //g na 3eroume ama exoume brei th prwth otan broume thn epomenh na tin epistre4oume
  int bf = s_info[scanDesc].id;
  while(bf != -1){
    if (BF_ReadBlock(s_info[scanDesc].idD ,bf, &block)< 0){
      BF_PrintError("Error getting block");
      //AM_erno= findnext;
      return (int)findnext;
    }
    strcpy(temp, (char*)block);
    
    x=0; y=0; z=0, f=0;     
    while(temp[x] != '\n'){          //passing the first line  p einai d+(int)
      if(temp[1]!= '\n' && x!=0)
        v[y] = temp[x];
      else
        bf = -1;
      x++;
    }
    if(bf != -1)
      bf = atoi(v);
    memset(v,0,10);
    y=0;
    while(temp[x] != '\0'){
     while(temp[x] != '\n'){
        if(temp[x] == ','){
          f =1;
          x++;
        }
        if(!f){
          v1[y] = temp[x];
          y++;
          x++;
        }
        else{
          v2[z] = temp[x];
          z++;
          x++;
        }
      }                               //edw exoume ta v1 v2
      if(s_info[scanDesc].op == EQUAL){
        if(strcmp(v1, (char*)s_info[scanDesc].value) == 0){
          if(epomenh)       //na stilw to swsto type prepei edw
            if(s_info[scanDesc].type[2] == 'c')
              return (char*)v2;
            else if(s_info[scanDesc].type[2] == 'i')
              return (int)v2;
            else if(s_info[scanDesc].type[2] == 'f')
              return (float*)v2;
          else
            epomenh =1;
        }
      }
      else if(s_info[scanDesc].op == NOT_EQUAL){
        if(strcmp(v1, (char*)s_info[scanDesc].value) != 0){
          if(epomenh)       //na stilw to swsto type prepei edw
            if(s_info[scanDesc].type[2] == 'c')
              return (char*)v2;
            else if(s_info[scanDesc].type[2] == 'i')
              return (int)v2;
            else if(s_info[scanDesc].type[2] == 'f')
              return (float*)v2;
          else
            epomenh =1;
        }
      }
      else if(s_info[scanDesc].op == LESS_THAN){
        if(strcmp(v1, (char*)s_info[scanDesc].value) < 0){
          if(epomenh)       //na stilw to swsto type prepei edw
            if(s_info[scanDesc].type[2] == 'c')
              return (char*)v2;
            else if(s_info[scanDesc].type[2] == 'i')
              return (int)v2;
            else if(s_info[scanDesc].type[2] == 'f')
              return (float*)v2;
          else
            epomenh =1;
        }
      }
      else if(s_info[scanDesc].op == GREATER_THAN){
        if(strcmp(v1, (char*)s_info[scanDesc].value) > 0){
          if(epomenh)       //na stilw to swsto type prepei edw
            if(s_info[scanDesc].type[2] == 'c')
              return (char*)v2;
            else if(s_info[scanDesc].type[2] == 'i')
              return (int)v2;
            else if(s_info[scanDesc].type[2] == 'f')
              return (float*)v2;
          else
            epomenh =1;
        }
      }
      else if(s_info[scanDesc].op == LESS_THAN_OR_EQUAL){
        if(strcmp(v1, (char*)s_info[scanDesc].value) <= 0){
          if(epomenh)       //na stilw to swsto type prepei edw
            if(s_info[scanDesc].type[2] == 'c')
              return (char*)v2;
            else if(s_info[scanDesc].type[2] == 'i')
              return (int)v2;
            else if(s_info[scanDesc].type[2] == 'f')
              return (float*)v2;
          else
            epomenh =1;
        }
      }
      else if(s_info[scanDesc].op == GREATER_THAN_OR_EQUAL){
        if(strcmp(v1, (char*)s_info[scanDesc].value) >= 0){
         if(epomenh)       //na stilw to swsto type prepei edw
            if(s_info[scanDesc].type[2] == 'c')
              return (char*)v2;
            else if(s_info[scanDesc].type[2] == 'i')
              return (int)v2;
            else if(s_info[scanDesc].type[2] == 'f')
              return (float*)v2;
          else
            epomenh =1;
        }
      }
      memset(v1,0,256);
      memset(v1,0,256);
    }
    //e3w apo to /0
  }
  //AM_errno = AME_EOF;
  return NULL;
}


int AM_CloseIndexScan( int scanDesc){
  s_info[scanDesc].id=-1;
  return AME_OK;

}


/*ta3inomei to block
ekxwrei kathe grammh tou se enan pinaka
meta ton ta3inomei me bash to val1
meta strcat gia thn eisxwrei sto block k apostolh t*/

char* sort(char block[],char c){

  int i=0,j,lines=0,size;
  while(block[i]!='\n')i++;
  size=i;
  i=0;
  char first_line[size];
  while(block[i]!='\n'){
    first_line[i]=block[i];
    i++;
  }
  strcat(first_line,"\n");
  printf("fl\n%s\n",first_line);
  i++;//gia na perasei to \n
  j=i;
  while(block[j]!='\0'){
    if(block[j]=='\n')lines++;
    j++;
  }
  char **char_ar,**char_val;
  int flag,kk,k;
  flag=1;
  char_ar = (char**) calloc(lines, sizeof(char*));//exei olh th grammh
  for ( k = 0; k < lines; k++ ){
    char_ar[k] = (char*) calloc(514, sizeof(char));//borei na exei 2 char ton 512 k 2 akm bit
  }

  char_val=(char**) calloc(lines, sizeof(char*));//exei mono to val1 p xrisimopoihtai g th sugkrish
  for ( k = 0; k < lines; k++ ){
    char_val[k] = (char*) calloc(256, sizeof(char));
  }
  k=0;
  kk=0;
  int l=i;
  while(block[l]!='\0'){
    l++;
  }
  while(block[i]!='\0'){//gemizei tous pinakes
    while(block[i]!='\n'){
      if(block[i]==',')flag=0;
      if(flag)char_val[k][kk]=block[i];
      char_ar[k][kk]=block[i];
      i++;
      kk++;
    }
    k++;
    kk=0;
    i++;
    flag=1;
  }
  char temp[256];
  for (i = 0 ; i < ( lines - 1 ); i++){//bubblesort
    for (j = 0 ; j < lines - i - 1; j++){
      if(c=='i'){
        if (atoi(char_val[j]) > atoi(char_val[j+1])){
          strcpy(temp,char_val[j]);
          memset(char_val[j],0,sizeof(char_val[j]));
          strcpy(char_val[j],char_val[j+1]);
          memset(char_val[j+1],0,sizeof(char_val[j+1]));
          strcpy(char_val[j+1],temp);
          memset(temp,0,256);
          strcpy(temp,char_ar[j]);
          memset(char_ar[j],0,sizeof(char_ar[j]));
          strcpy(char_ar[j],char_ar[j+1]);
          memset(char_ar[j+1],0,sizeof(char_ar[j+1]));
          strcpy(char_ar[j+1],temp);
        }
      }
      else if(c=='f'){
        if (atof(char_val[j]) > atof(char_val[j+1])){
          strcpy(temp,char_val[j]);
          memset(char_val[j],0,sizeof(char_val[j]));
          strcpy(char_val[j],char_val[j+1]);
          memset(char_val[j+1],0,sizeof(char_val[j+1]));
          strcpy(char_val[j+1],temp);
          memset(temp,0,256);
          strcpy(temp,char_ar[j]);
          memset(char_ar[j],0,sizeof(char_ar[j]));
          strcpy(char_ar[j],char_ar[j+1]);
          memset(char_ar[j+1],0,sizeof(char_ar[j+1]));
          strcpy(char_ar[j+1],temp);
        }
      }
      else if(c=='c'){
        if(strcmp(char_val[j],char_val[j+1])>0){
          strcpy(temp,char_val[j]);
          memset(char_val[j],0,sizeof(char_val[j]));
          strcpy(char_val[j],char_val[j+1]);
          memset(char_val[j+1],0,sizeof(char_val[j+1]));
          strcpy(char_val[j+1],temp);
          memset(temp,0,256);
          strcpy(temp,char_ar[j]);
          memset(char_ar[j],0,sizeof(char_ar[j]));
          strcpy(char_ar[j],char_ar[j+1]);
          memset(char_ar[j+1],0,sizeof(char_ar[j+1]));
          strcpy(char_ar[j+1],temp);
        }
      }
      memset(temp,0,256);
    }
  }
  char *re_val=(char*)malloc(BLOCK_SIZE);
  strcpy(re_val,first_line);
  for(i=0;i<lines;i++){
    strcat(re_val,char_ar[i]);
    strcat(re_val,"\n");
  }
  printf("\n\nIN SORT\n%s\n\n",re_val);
  for( i = 0; i < lines; i++){
    free(char_ar[i]);
    free(char_val[i]);
  }
  free(char_ar);
  free(char_val);
  return (char*)re_val;



} 



 void AM_PrintError(char *errString ){
  printf("%s\n", errString );
  if( AM_errno == open) 
    printf("Error in Open\n");
  else if( AM_errno == create) 
    printf("Error in Create\n");
  else if( AM_errno == close) 
    printf("Error in Close\n");
  else if( AM_errno == destroy) 
    printf("Error in Destroy\n");
  else if( AM_errno == insert) 
    printf("Error in Insert\n");
  else if( AM_errno == openscan) 
    printf("Error in OpenScan\n");
  else if( AM_errno == closescan) 
    printf("Error in CloseScan\n");
  else if( AM_errno == findnext) 
    printf("Error in FindNext\n");
}

