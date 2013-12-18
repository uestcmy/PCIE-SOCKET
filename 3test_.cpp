//This is in the branch:newbranch

#include <stdio.h>
//#include <sys/types.h>
//#include <sys/stat.h>
//#include <fcntl.h>
//#include <unistd.h>
//#include <sys/ioctl.h>
#include <sys/time.h>
#include <time.h>
//#include <stdlib.h>
//#include <errno.h>
#include <pthread.h>
//#include "ml605_api.h"
#include "ml605_api.cpp"
//#include "xpmon_be.h"
//#define RAWDATA_FILENAME    "/dev/ml605_raw_data"
//#define XDMA_FILENAME       "/dev/xdma_stat"
//#define PKTSIZE             4096
#define LEN 4096*3+10

#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/wait.h>
#include <unistd.h>
#include <arpa/inet.h>

#define LENGTH_OF_CORE 84
#define SERVPORT 6001
#define SERVPORT2 6002
#define TEST_4096
#define DEBUG84

FILE *fp1,*fp2;
static const int size = 4096;
bool isclose = false;
int testfd;
int count=0;
long long mycount = 0;
int mysleep = 1000000;

    //socket declaraion
    int sock_fd, recvbytes;
    int sock_fd2, recvbytes2;
    int sock_fd3, recvbytes3;
    int sock_fd4, recvbytes4;
    int sock_fd5, recvbytes5;
    int sock_fd6, recvbytes6;

    struct hostent *host;
    struct sockaddr_in serv_addr;
    struct sockaddr_in serv_addr2;
    struct sockaddr_in serv_addr3;
    struct sockaddr_in serv_addr4;
    struct sockaddr_in serv_addr5;
    struct sockaddr_in serv_addr6;

int continuecnt3c20 = 0;
char ofdm_buff[14404*3+10] = {0};
int continueflag3c20 = 0;//the mark whether send the continious frame , not a very first frame
int num_of_comma3c20 = 0;//counter the number of comma

int continueflag = 0;//the mark whether send the continious frame , not a very first frame
int num_of_comma = 0;//counter the number of comma
int endmark3c20 = 0;
int ofdm_position = 0;

int searchend3c20(char *inbuf){//continue
	char buff2[4096*3+1] = {0};
	int cnt2 = 0;
       for(  int i = 0 ; i < 4096*3 ; i++ ){
		buff2[cnt2++] = inbuf[i];
		if(inbuf[i] == ',') {
			num_of_comma3c20++;
			//printf("%d,",num_of_comma3c20);
		}//if
		if( num_of_comma3c20 == 14404 ){
			if(inbuf[i+1] == 'a' && inbuf[i+2]=='a' ){
				buff2[cnt2++] = inbuf[i+1]+1;
				buff2[cnt2++] = inbuf[i+2];
				buff2[cnt2++] = inbuf[i+3];
				buff2[cnt2++] = inbuf[i+4];
				buff2[cnt2++] = inbuf[i+5];
				buff2[cnt2++] = inbuf[i+6];
				endmark3c20 = 1;//ready to send to the client				
				strcat(ofdm_buff,buff2);//combine to the big buffer
				ofdm_position = 0;
				continueflag3c20 = 0;
				num_of_comma3c20 = 0;
				return 0;		
			}else{
				continueflag3c20 = 0;
				num_of_comma3c20 = 0;
				return 0;
			}			
		}//if
	}//for
	continueflag3c20++;
	buff2[4096*3] = '\0';
	strcat(ofdm_buff,buff2);//combine to the big buffer
}
int search_a0aa3c20(char *inbuf){
	/* funcion:
	 *		search the string and find the header which is aaa0 203c: OFDM data

	 */
	char buff2[4096*3] = {0};
	int cnt2 = 0;
	for( int i = 0 ; i < LEN-10 ; i++ ){

                //a0aa 28X0
		if(inbuf[i] == 'a' && inbuf[i+1] == 'a' && inbuf[i+2] == ',' && inbuf[i+3] == 'a' && inbuf[i+4] == '0' && inbuf[i+5] == ',' && inbuf[i+6] == '2' && inbuf[i+7] == '0' && inbuf[i+8] == ',' && inbuf[i+9] == '3' && inbuf[i+10] == 'c'){
			memset(ofdm_buff,0,14404*3+10);
			//endmark3c20 = 1;
			num_of_comma3c20 = 0;
			continueflag3c20 = 1;//continue to send the data
			for( int j = i ; j <= LEN ; j++ ){
				if(inbuf[j] == ','){
					num_of_comma3c20++;
				}//if
				buff2[cnt2++] = inbuf[j];
				ofdm_position++;
			}//for j
			buff2[cnt2] = '\0';
			strcat(ofdm_buff,buff2);
			return 0;
		}//if
	}//for
}
int search_a0aa4028(char* inbuf) {	
	/* funcion:
	 *		search the string and find the header which is a0aa4028
	 */
	for( int i = 0 ; i < LEN-10 ; i++ ){

#ifdef DEBUG84
                //a0aa 28X0
		if(inbuf[i] == 'a' && inbuf[i+1] == 'a' && inbuf[i+2] == ',' && inbuf[i+3] == 'a' && inbuf[i+4] == '0' && inbuf[i+5] == ',' && inbuf[i+6] == '2' && inbuf[i+7] == '8' && inbuf[i+8] == ',' && inbuf[i+10] == '0'){
#endif

#ifdef DEBUG14404

		if(inbuf[i] == 'a' && inbuf[i+1] == 'a' && inbuf[i+2] == ',' && inbuf[i+3] == 'a' && inbuf[i+4] == '0' ){
#endif 
//&& inbuf[i+5] == ',' && inbuf[i+6] == '2' && inbuf[i+7] == '0' && inbuf[i+8] == ',' && inbuf[i+9] == '3' && inbuf[i+10] == 'c'
			continueflag = 1;//continue to send the data
			for( int j = i ; j < LEN ; j++ ){
				if(inbuf[j] == ','){
					num_of_comma++;
					if( num_of_comma == LENGTH_OF_CORE ){//find the end of the core-frame
						continueflag = 0; // finish to find the whole frame, it is the time to reset the flag.not to send the continious frame but to quit and search another new frame header
						num_of_comma = 0; // reset the counter of the comma
						return 1;			
					}//if
				}//if
			}//for j
			return 1; // return true but not the end of the core frame 
		}//if



		if(inbuf[i] == 'a' && inbuf[i+1] == 'a' && inbuf[i+2] == ',' && inbuf[i+3] == 'a' && inbuf[i+4] == '0' && inbuf[i+5] == ',' && inbuf[i+6] == '2' && inbuf[i+7] == '8' && inbuf[i+8] == ',' && inbuf[i+9] == '6' && inbuf[i+10] == '0'){
			continueflag = 2;//continue to send the data
			for( int j = i ; j < LEN ; j++ ){
				if(inbuf[j] == ','){
					num_of_comma++;
					if( num_of_comma == LENGTH_OF_CORE ){//find the end of the core-frame
						continueflag = 0; // finish to find the whole frame, it is the time to reset the flag.not to send the continious frame but to quit and search another new frame header
						num_of_comma = 0; // reset the counter of the comma
						return 2;			
					}//if
				}//if
			}//for j
			return 2; // return true but not the end of the core frame 
	}
	}//for
	return 0;
}


void* mywrite(void* param)
{
unsigned char txbuff[size];

while(!isclose)
   {for(int i=0;i<size;i++)
    txbuff[i] = count;
       int sendsize = ML605Send(testfd,txbuff,size);
	   if(sendsize!=size)printf("send error!\n");
	   
	  // usleep(1);
    count++;
   }

}

char buff3[4096*3+1] = {0};

int cnt_element = 0;
int cnt_send  = 0;
void* myread(void* param)
{       
	
	fp1 = fopen("testdata.txt","w");	
	int mytimes=0;
	
	unsigned char rxbuff[size];

	
	while(!isclose){    //usleep(1);
        	int recvsize = ML605Recv(testfd,rxbuff,4096);
	   	if(recvsize!=size)
			printf("recv error!\n");
        	else{
			if(mycount == LEN){fclose(fp1);}
     			mycount = mycount + 1;
	
			
             //if(mytimes==0)
                               {
			
                     printf("recv successful!=%d  ",recvsize);
            		for(int i=0;i<10;i++){
	           		printf("%x,",rxbuff[i]);
#ifdef TEST_300
				unsigned char tmp = rxbuff[i];
				int bit[8] = {0};
				int cntbit = 0;
				char buff[3];
				char buff2[4]= {0};
				while(tmp>0){
					bit[cntbit++] =tmp % 2;
					//printf("%d",bit);
					tmp /= 2;				
				}
				int base = 8;
				int sum = 0;
				for( int j = 7 ; j >= 3 ; j-- ){
					sum += bit[j] * base;
					base /= 2;		
				}
				if( sum < 10 ){
					sprintf(buff,"%c",sum+'0');
				}else{
					sprintf(buff,"%c",sum-10+'a');
				}
				strcat(buff2,buff);
				base = 8;
				sum = 0;
				for( int j = 3 ; j >= 0 ; j-- ){
					sum += bit[j] * base;
					base /= 2;		
				}
				if( sum < 10 ){
					sprintf(buff,"%c",sum+'0');
				}else{
					sprintf(buff,"%c",sum-10+'a');
				}
				strcat(buff2,buff);
				
				buff2[2] = ',';
				strcat(buff3,buff2);
				
#endif
                     

			}//for
			//sendto(sock_fd,buff3,sizeof(buff3),0,(struct sockaddr *)&serv_addr,sizeof(serv_addr));
			//printf("\n%s\n",buff3);

			if( mycount == 2000 ) mycount = 0;			
			if(mycount % 2000 <= 100 ){   //mycount < LEN
			
			memset(buff3,0,(4096*3+1));
			cnt_element = 0; //reset the counter of the buffer
			for(int i=0;i<4096;i++){
				
	           		//fprintf(fp1,"%x,",rxbuff[i]);
				
#ifdef TEST_4096
				unsigned char tmp = rxbuff[i];
				int bit[8] = {0};
				int cntbit = 0;
				char buff[3];
				char buff2[4]= {0};
				while(tmp>0){
					bit[cntbit++] =tmp % 2;
					//printf("%d",bit);
					tmp /= 2;				
				}
				int base = 8;
				int sum = 0;
				for( int j = 7 ; j >= 3 ; j-- ){
					sum += bit[j] * base;
					base /= 2;		
				}
				if( sum < 10 ){
					sprintf(buff,"%c",sum+'0');
				}else{
					sprintf(buff,"%c",sum-10+'a');
				}
				strcat(buff2,buff);
				base = 8;
				sum = 0;
				for( int j = 3 ; j >= 0 ; j-- ){
					sum += bit[j] * base;
					base /= 2;		
				}
				if( sum < 10 ){
					sprintf(buff,"%c",sum+'0');
				}else{
					sprintf(buff,"%c",sum-10+'a');
				}
				strcat(buff2,buff);
				
				buff2[2] = ',';
				
				strcat(buff3,buff2);

				cnt_element++;//counter to the buff elements 6144 to change another buffer

#endif
				}//fprintf(fp1,"--------------------------------------------------\n");
				

				
				if(search_a0aa4028(&buff3[0]) == 1 || continueflag == 1 ){
					
        				sendto(sock_fd,buff3,sizeof(buff3),0,(struct sockaddr *)&serv_addr,sizeof(serv_addr));	//6001 for bar
					sendto(sock_fd2,buff3,sizeof(buff3),0,(struct sockaddr *)&serv_addr2,sizeof(serv_addr));//6002  for link
					//sendto(sock_fd3,buff3,sizeof(buff3),0,(struct sockaddr *)&serv_addr3,sizeof(serv_addr));
					//fprintf(fp2,"%s",buff3);	
	 			}//if

				if(continueflag3c20 == 0){
					num_of_comma3c20 = 0;
					search_a0aa3c20(&buff3[0]);
				}else{
					searchend3c20(buff3);				
				}

			       if(endmark3c20 == 1 ){
					endmark3c20 = 0;
					continueflag3c20 = 0;
					num_of_comma3c20 = 0;
        				sendto(sock_fd3,ofdm_buff,sizeof(ofdm_buff),0,(struct sockaddr *)&serv_addr3,sizeof(serv_addr)); // port:7001  channel H11
        				sendto(sock_fd4,ofdm_buff,sizeof(ofdm_buff),0,(struct sockaddr *)&serv_addr4,sizeof(serv_addr)); // port:7002  channel H12
					sendto(sock_fd5,ofdm_buff,sizeof(ofdm_buff),0,(struct sockaddr *)&serv_addr5,sizeof(serv_addr)); // port:7003  channel H21
					sendto(sock_fd6,ofdm_buff,sizeof(ofdm_buff),0,(struct sockaddr *)&serv_addr6,sizeof(serv_addr)); // port:7004  channel H22

	 			}//if
				
				

			
			}
                mytimes=1;
            		printf("one receive cycle\n");
		}//end if
        	}//end else		   
		//usleep(500000);
   	}// end while
}//myread
void* GetRate(void* param)
{   int scount;
    int ecount;
    double recvrata;
    while(!isclose){ 
          scount = mycount;
          printf("scount=%d\n",scount);
          usleep(1000000);
          ecount = mycount;
          printf("ecount=%d\n",ecount);
          recvrata = (ecount - scount)*4096*8/1000000;
          printf("receive data rata = %f M/S \n",recvrata);
}
}
int main()
{
	
    host=gethostbyname("127.0.0.1");
    sock_fd = socket(AF_INET, SOCK_DGRAM, 0);
    serv_addr.sin_family=AF_INET;
    serv_addr.sin_port=htons(SERVPORT);   // 6001 for bar
    serv_addr.sin_addr = *((struct in_addr *)host->h_addr);
    bzero(&(serv_addr.sin_zero),8);

    sock_fd2 = socket(AF_INET, SOCK_DGRAM, 0);
    serv_addr2.sin_family=AF_INET;
    serv_addr2.sin_port=htons(8001);//  8001 for link
    serv_addr2.sin_addr = *((struct in_addr *)host->h_addr);
    bzero(&(serv_addr2.sin_zero),8);

    sock_fd3 = socket(AF_INET, SOCK_DGRAM, 0);
    serv_addr3.sin_family=AF_INET;
    serv_addr3.sin_port=htons(7001);// for Channel H11
    serv_addr3.sin_addr = *((struct in_addr *)host->h_addr);
    bzero(&(serv_addr3.sin_zero),8);


    sock_fd4 = socket(AF_INET, SOCK_DGRAM, 0);
    serv_addr4.sin_family=AF_INET;
    serv_addr4.sin_port=htons(7002);// for Channel H12
    serv_addr4.sin_addr = *((struct in_addr *)host->h_addr);
    bzero(&(serv_addr4.sin_zero),8);

    sock_fd5 = socket(AF_INET, SOCK_DGRAM, 0);
    serv_addr5.sin_family=AF_INET;
    serv_addr5.sin_port=htons(7003);// for Channel H21
    serv_addr5.sin_addr = *((struct in_addr *)host->h_addr);
    bzero(&(serv_addr5.sin_zero),8);

    sock_fd6 = socket(AF_INET, SOCK_DGRAM, 0);
    serv_addr6.sin_family=AF_INET;
    serv_addr6.sin_port=htons(7004);// for Channel H22
    serv_addr6.sin_addr = *((struct in_addr *)host->h_addr);
    bzero(&(serv_addr6.sin_zero),8);


/*

  // test socket 1
    char a[100];
    sprintf(a,"Hello, server! This is crr_client. through socket3");
    sendto(sock_fd3,a,sizeof(a),0,(struct sockaddr *)&serv_addr3,sizeof(serv_addr));
*/




   //printf("my:hello fedora!\n");
if((testfd = ML605Open())<0)
   printf("open ml605 failed");

if(ML605StartEthernet(testfd, SFP_TX_START)<0) {
    printf("PCIe:Start ethernet failure\n");
	  ML605Close(testfd);
    exit(-1);
  }

/* pthread_t writethread;
  if (pthread_create(&writethread, NULL, mywrite, NULL)) 
  {
    perror("writethread process thread creation failed");
  }  
  sleep(1);
*/  


  pthread_t readthread;
  if (pthread_create(&readthread, NULL, myread, NULL)) 
  {
    perror("readthread process thread creation failed");
  }  
  sleep(1);

/* 
  pthread_t ratathread;
  if (pthread_create(&ratathread, NULL, GetRate, NULL)) 
  {
    perror("readthread process thread creation failed");
  }  
  sleep(1);
*/
/*
		// read only one time
		unsigned char rxbuff[size];
		int recvsize = ML605Recv(testfd,rxbuff,4096);
	   	if(recvsize!=size)
			printf("recv error!\n");
        	else{
     			mycount = mycount + 1;
	
			printf("recv successful!=%d",recvsize);
            		for(int i=0;i<10;i++)
	           		printf("%x,",rxbuff[i]);

            		//printf("one receive cycle\n");
        	}//end else
  */
  char ch_input;
  scanf("%c", &ch_input);
  isclose=true;
  ML605Close(testfd);
}




