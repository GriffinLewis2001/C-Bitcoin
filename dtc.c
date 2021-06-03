#pragma GCC diagnostic ignored "-Wformat="
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>
#include"sha256.h"
#include <io.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <windows.h>



char* StrSHA256(const char* str, long long length, char* sha256);

typedef struct wallet{
    char private_key[70];
    char address[65];
    float balance;
}Wallet;

Wallet* init_wallet(){
    printf("正在为你生成Bitmoney钱包\n");
    Wallet* my_wallet=(Wallet*)malloc(sizeof(Wallet));
    int base;
    srand((unsigned)time(NULL));
    base = rand();
    char hash[65]={0};
    sprintf(hash,"%f",(float)time(0)/base);
    char* key=(char*)malloc(65*sizeof(char));
    StrSHA256(hash,strlen(hash),key);
    char pkey[100];
    sprintf(pkey,"%s%s","0x01",key);

    char* address=(char*)malloc(65*sizeof(char));
    StrSHA256(pkey,strlen(pkey),address);

    strcpy(my_wallet->private_key,pkey);
    strcpy(my_wallet->address,address);
    my_wallet->balance=0;
    return my_wallet;

}

typedef struct block{

	int index;
	int proof_no;
	char prev_hash[200];
	char data[120];
	int timestamp;

}Block;

char* calculate_hash(Block* block){

	char hash[340];
	char index0[6];
	char proof_no0[6];
	char timestamp0[6];
	sprintf(index0,"%d",block->index);
	sprintf(proof_no0,"%d",block->proof_no);
	sprintf(timestamp0,"%d",block->timestamp);
	sprintf(hash,"%s %s %s %s %s",index0,proof_no0,block->prev_hash,block->data,timestamp0);
	char* sha256=(char *)malloc(200*sizeof(char));
	StrSHA256(hash,strlen(hash),sha256);
	return sha256;

}

char* calculate_hash_info(int index,int proof_no,char prev_hash[200],char sender[70],char address[70],int volume,int timestamp){

  char hash[340];
  char index0[6];
  char proof_no0[6];
  char timestamp0[6];
  char volume0[8];
  sprintf(index0,"%d",index);
  sprintf(proof_no0,"%d",proof_no);
  sprintf(timestamp0,"%d",timestamp);
  sprintf(volume0,"%d",volume);
  sprintf(hash,"%s %s %s %s %s %s %s",index0,proof_no0,prev_hash,sender,address,volume0,timestamp0);
  char* sha256=(char *)malloc(200*sizeof(char));
  StrSHA256(hash,strlen(hash),sha256);
  return sha256;

}

Block* Blockchain(){

    Block* chain=(Block* )malloc(sizeof(Block)*500);
    return chain;

}

int chain_length(Block* chain){

    Block* ptr=chain;
    while(ptr->index != 0){
        ptr++;
    }
    return ptr-chain;
}


int check_proof(Block *block){

    if(strncmp((char*)calculate_hash(block),"0",1) != 0){
		return -1;}
	return 0;

}

int check_validity(Block* block,Block* prev_block){

    if(prev_block->index+1 != block->index){
        return -1;
    }
    else if(strcmp(calculate_hash(prev_block),calculate_hash(block)) != 0){
        return -1;
    }
    else if(block->timestamp < prev_block->timestamp){
        return -1;
    }
    return 0;
}

int proof_of_work(Block* block){

	block->proof_no=0;
	while(strncmp((char*)calculate_hash(block),"00",2) != 0){
		(block->proof_no)++;
	}
	return (block->proof_no);

}
int line_count(){
    FILE *fp;
    int n = 0;
    int ch;

    if((fp = fopen("blockchain.txt","r+")) == NULL)
    {
       fprintf(stderr,"open file 1.c error! %s\n",strerror(errno));
    }

    while((ch = fgetc(fp)) != EOF)
    {
       if(ch == '\n')
       {
           n++;
       }
    }

    fclose(fp);
    return n;
}

void construct_genesis(Block* chain,char address[65]){

    chain->index=1;
    strcpy(chain->prev_hash,"00000000");
    chain->proof_no=0;
    sprintf(chain->data,"reward %s 25",address);
    chain->timestamp=time(0);

}

typedef struct datac{
  int index;
  int proof_no;
  char prev_hash[66];
  char sender[65];
  char recepient[65];
  int volume;
  int timestamp;

} datac;

void mining_process(char recepient_address[65]){
  FILE* fp=fopen("blockchain.txt","r");
  //如第一次挖取，将生成区块链账本
  if(fp==NULL){

    fp=fopen("blockchain.txt","w");
    Block* chain=(Block* )malloc(sizeof(Block)*500);
    Block* ptr;
    for(ptr=chain;ptr-chain<2;ptr++){
      if(ptr==chain){
        construct_genesis(chain,recepient_address);
        printf("此次交易哈希值:%s\n",calculate_hash(ptr));
        printf("工作量证明:%d\n",ptr->proof_no);
        printf("上一次交易哈希值:%s\n",ptr->prev_hash);
        printf("区块高度:%d\n",(ptr-chain+1));
        printf("交易奖励:25 Bitmoney\n\n");
        fprintf(fp,"block height:%d proof_of_work:%d prevhash:%s data:%s timestamp:%d\n",ptr->index,ptr->proof_no,ptr->prev_hash,ptr->data,ptr->timestamp);
      }
      else
      {
        clock_t start,end;
        ptr->index=ptr-chain+1;
        char data[120];
        sprintf(data,"reward %s 25",recepient_address);
        strcpy(ptr->data,data);
        ptr->timestamp=time(0);
        strcpy(ptr->prev_hash,calculate_hash(ptr-1));
        start=clock();
        int num=proof_of_work(ptr);
        end=clock();
        printf("此次交易哈希值:%s\n",calculate_hash(ptr));
        printf("工作量证明:%d\n",ptr->proof_no);
        printf("上一次交易哈希值:%s\n",ptr->prev_hash);
        printf("区块高度:%d\n",(ptr-chain+1));
        printf("当前电脑哈希算力:%fMH/s\n",num/((double)(end-start)/CLK_TCK)/1000000);
        printf("交易奖励:25 Bitmoney\n\n");
        fprintf(fp,"block height:%d proof_of_work:%d prevhash:%s data:%s timestamp:%d\n",ptr->index,ptr->proof_no,ptr->prev_hash,ptr->data,ptr->timestamp);
      }
    }
    fclose(fp);

  }
  else
  {
    // 你的钱包地址为:0A4689C7F5DE774C4A0ADB0D28179AEDCAB88C6F1FF81EAF3BAAF425C75A9A5D
    datac* x=(datac*)malloc(sizeof(datac)*500);
    datac* ptr1=x;
    int i;
    for(i=0;i<line_count();i++){
      fscanf(fp,"block height:%d proof_of_work:%d prevhash:%s data:%s %s %d timestamp:%d\n",&ptr1->index,&ptr1->proof_no,ptr1->prev_hash,ptr1->sender,ptr1->recepient,&ptr1->volume,&ptr1->timestamp);
      ptr1++;
    }
    ptr1-=1;
    fclose(fp);
    fp=fopen("blockchain.txt","a+");
    Block* chain=(Block* )malloc(sizeof(Block)*500);
    Block* ptr;
    char* last=calculate_hash_info(ptr1->index,ptr1->proof_no,ptr1->prev_hash,ptr1->sender,ptr1->recepient,ptr1->volume,ptr1->timestamp);

    for(ptr=chain;ptr-chain<2;ptr++){
      clock_t start,end;
      ptr->index=i+1;
      char data[120];
      sprintf(data,"reward %s 25",recepient_address);
      strcpy(ptr->data,data);
      ptr->timestamp=time(0);
      if(ptr==chain)
        strcpy(ptr->prev_hash,last);
      else
        strcpy(ptr->prev_hash,calculate_hash(ptr-1));
      start=clock();
      int num=proof_of_work(ptr);
      end=clock();
      printf("此次交易哈希值:%s\n",calculate_hash(ptr));
      printf("工作量证明:%d\n",ptr->proof_no);
      printf("上一次交易哈希值:%s\n",ptr->prev_hash);
      printf("区块高度:%d\n",i+1);
      printf("当前电脑哈希算力:%fMH/s\n",num/((double)(end-start)/CLK_TCK)/1000000);
      printf("交易奖励:25 Bitmoney\n\n");
      fprintf(fp,"block height:%d proof_of_work:%d prevhash:%s data:%s timestamp:%d\n",ptr->index,ptr->proof_no,ptr->prev_hash,ptr->data,ptr->timestamp);
      i++;
    }
    fclose(fp);

  }
}

void transaction(char sender_address[65],char recepient_address[63]){



}
int main(int argc,char* argv[]){
    printf("欢迎使用简化版Bitmoney\n");
    printf("Bitmoney 是一个基于区块链技术的支付系统.\n用户可通过挖矿获取Bitmoney\n此支付系统尚简陋，仅供测试\n本程序致敬");
    HANDLE consolehwnd;//创建句柄
    consolehwnd=GetStdHandle(STD_OUTPUT_HANDLE);//实例化句柄
    SetConsoleTextAttribute(consolehwnd,FOREGROUND_GREEN);//设置字体颜色
    printf("Satoshi Nakamoto");

	  SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
    printf("和");
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 6);
    printf("Vitalik Buterin\n");
	  SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), 7);
    printf("-----------------------\n");
    printf("你可进行以下操作:\n");
    START:
    printf("\n\n\n\n0 创建Bitmoney钱包\n");
    printf("1 查看我的钱包\n");
    printf("2 挖矿(一次最多只能挖两个区块)\n");
    printf("请输入对应操作的编号以进行该操作:\n");
    char answer[10]={0};
    LOOP:
    scanf("%s",answer);
    if(strcmp("0",answer)==0){
      Wallet* wallet=init_wallet();
      printf("你的私钥为:%s\n",wallet->private_key);
      printf("你的钱包地址为:%s\n",wallet->address);
      printf("请牢记你的私钥地址,私钥丢失后钱包将永远丢失!\n钱包地址可通过私钥进行一次哈希运算得到\n");
      printf("\n\n\n接下来你可以:\n");
      goto START;
    }
    else if(strcmp("1",answer)==0){
      char p_key[80];
      printf("请输入你的私钥:\n");
      scanf("%s",p_key);
      if(strlen(p_key) != 68 || strncmp((char*)p_key,"0x01",4) != 0){
        printf("此私钥无效,请重新输入或创造新的钱包\n");
        goto START;
      }
      else
      {


        char* address=(char*)malloc(65*sizeof(char));
        StrSHA256(p_key,strlen(p_key),address);
        printf("钱包地址:%s\n",address);
        datac* x=(datac*)malloc(sizeof(datac)*500);
        datac* ptr1=x;
        int i;
        FILE* fp=fopen("blockchain.txt","r");
        int a=0;
        for(i=0;i<line_count();i++){
          fscanf(fp,"block height:%d proof_of_work:%d prevhash:%s data:%s %s %d timestamp:%d\n",&ptr1->index,&ptr1->proof_no,ptr1->prev_hash,ptr1->sender,ptr1->recepient,&ptr1->volume,&ptr1->timestamp);
          if(strcmp(ptr1->recepient,address)==0){

            a+=(ptr1->volume);
          }
          ptr1++;
        }
        fclose(fp);
        printf("钱包余额:%d\n",a);
        printf("\n\n\n接下来你可以\n");
        free(address);free(x);
        goto START;

      }
    }
    else if(strcmp("2",answer)==0){
      printf("请输入你的钱包地址:");
      char address[65];
      scanf("%s",address);
      if(strlen(address) != 64){
        printf("此地址无效");
        goto START;
      }
      else{
        mining_process(address);
        printf("挖矿完毕\n");
        goto START;
      }

    }
    else{
      printf("请按提示进行有效输入!\n");
      goto LOOP;

    }

}
