/*
 * http.c
 *  Created on: Jun 3, 2024
 *      Author: shekhar verma
 */

#include "http.h"
const char APN[] = "IOT.COM";
extern int receivedvalue;
/**********************************/
bool httpConfigStatus=false;
bool get_http_Config_status() {
	return httpConfigStatus;
}
void set_http_Config_status(bool httpConfig) {
	httpConfigStatus = httpConfig;
}
/************http ssl*************/
bool httpSLLFlag=false;
bool get_http_SSL_Status() {
	return httpSLLFlag;
}
void set_http_SSL_Status(bool sslFlag) {
	httpSLLFlag = sslFlag;
}
/**********************************/
bool http_init(){
	bool httpInitStatus=true;
	char resbuff[100]={0};char command[100] = {0};
	set_ec200_response_check("OK");
	if(!EC200_Transmit("AT+QHTTPCFG=\"contextid\",3\r\n",resbuff,EC200MINRESONSETIME)){
		printc(MinDelay,"ERROR:http CFG fail\n");
		httpInitStatus= false;
	}
	memset(resbuff,0,sizeof(resbuff));
	set_ec200_response_check("OK");
	if(!EC200_Transmit("AT+QHTTPCFG=\"CONTENTTYPE\",1\r\n",resbuff,EC200MINRESONSETIME)){
		printc(MinDelay,"ERROR:http contenttype fail\n");
		httpInitStatus= false;
	}
	set_ec200_response_check("OK");
	if(!EC200_Transmit("AT+QIDEACT=1\r\n",resbuff,EC200MINRESONSETIME)){
		printc(MinDelay,"ERROR:HTTP DEACT FAIL\n");
		httpInitStatus= false;
	}
	memset(resbuff,0,sizeof(resbuff));
	set_ec200_response_check("OK");
	sprintf(command, "AT+QICSGP=1,1,\"%s\",\"\",\"\",0\r\n",getConfigValue(Config_DAPN));
	if (!EC200_Transmit(command, resbuff,2000)) {
		printc(MinDelay, "ERROR:HTTP APN FAIL\n");
		httpInitStatus=false;
	}

	memset(resbuff,0,sizeof(resbuff));
	set_ec200_response_check("OK");
	if(!EC200_Transmit("AT+QIACT=1\r\n",resbuff,EC200MAXRESONSETIME)){
		printc(MinDelay,"ERROR:HTTP ACT FAIL\n");
		httpInitStatus= false;
	}
	memset(resbuff,0,sizeof(resbuff));
	set_ec200_response_check("OK");
	if (!EC200_Transmit("AT+QIDNSCFG=1,\"8.8.8.8\",\"8.8.4.4\"\r\n", resbuff, 2000)) {
		printc(MinDelay, "ERROR:DNS Fail\n");
		httpInitStatus=false;
	}
	//	http_ssl();
	httpConfigStatus = !httpInitStatus;
	return httpInitStatus;
}
//bool http_ssl()
//{
//	char resbuff[100]={0};
//	set_ec200_response_check("OK");
//	if(!EC200_Transmit("AT+QSSLCFG=\"sni\",1,1\r\n",resbuff,EC200MINRESONSETIME)){
//		return false;
//	}
//	memset(resbuff,0,100);
//	set_ec200_response_check("OK");
//	if(!EC200_Transmit("AT+QHTTPCFG=\"sslctxid\",1\r\n",resbuff,EC200MINRESONSETIME)){
//		return false;
//	}
//	memset(resbuff,0,100);
//	set_ec200_response_check("OK");
//	if(!EC200_Transmit("AT+QSSLCFG=\"sslversion\",1,3\r\n",resbuff,EC200MINRESONSETIME)){
//		return false;
//	}
//	memset(resbuff,0,100);
//	set_ec200_response_check("OK");
//	if(!EC200_Transmit("AT+QSSLCFG=\"ciphersuite\",1,0xFFFF\r\n",resbuff,EC200MINRESONSETIME)){
//		return false;
//	}
//	memset(resbuff,0,100);
//	set_ec200_response_check("OK");
//	if(!EC200_Transmit("AT+QSSLCFG=\"seclevel\",1,0\r\n",resbuff,EC200MINRESONSETIME)){
//		return false;
//	}
//	printc(MinDelay,"<<<<<<<<<<<<<HTTP SSL DONE>>>>>>>>>>>\n");
//	return true;
//}
bool http_post_data(const char *data,char *res,char*keyValue,int *postLength){
	bool httpStatus=false;
	char command[200]={0};
	printc(1000, "\nData Before Transmit: %s\n", data); // Log data
	set_ec200_response_check("CONNECT");
	if(EC200_Transmit("AT+QHTTPURL=50\r\n",res,EC200MAXRESONSETIME)){
		memset(res,0,sizeof(res));
		set_ec200_response_check("OK");
		if(EC200_Transmit("https://intouchdev.mapmyindia.com/IntouchFOTACOTA/",res,EC200MAXRESONSETIME)){
			memset(res,0,sizeof(res));
		}
		sprintf(command, "AT+QHTTPPOST=%d\r\n", (int)strlen(data));
		printc(MinDelay,"%s\n",command);
		set_ec200_response_check("CONNECT");
		if(EC200_Transmit(command, res, EC200MAXRESONSETIME)){
			memset(res,0,sizeof(res));
			set_ec200_response_check("+QHTTPPOST: 0,200");
			if(EC200_Transmit(data, res, EC200MAXRESONSETIME)){
				printc(MinDelay, "AT+QHTTPPOST-%s\n", res);
				httpStatus=true;
				if(strstr(res,"0,200,157")){
					httpStatus=false;
				}
				else{
					char opbuffer[5][100]={{0}};
					splitString(res, ",", opbuffer, 4);
					*postLength = atoi(opbuffer[2]);
				}
			}
		}
	}
	httpConfigStatus=!httpStatus;
	return httpStatus;
}
bool http_post_read_data(char *resbuff,int stringLength){
	switchIdle(false);
	EC200_Transmit("AT\r\n",NULL,EC200MAXRESONSETIME);
	HAL_Delay(1000);
	char cmd[100]={'\0'};
	sprintf(cmd,"AT+QHTTPREAD=80\r\n");
	HAL_UART_Transmit(&huart1, (uint8_t*) cmd, strlen(cmd), 2000);
	int rec_status = HAL_UART_Receive(&huart1, resbuff, stringLength+17, 10000);
	printc(MinDelay, "%d rec status",rec_status);
	switchIdle(true);
	enableIdle();
	if(rec_status==0)
		return true;
	else
		return false;
}
bool http_fota_get(char *resbuff){
	set_ec200_response_check("+QHTTPGET: 0,200");
	if (EC200_Transmit("AT+QHTTPGET\r\n", resbuff, EC200MAXRESONSETIME)) {
		printc(MinDelay, "%s\n", resbuff);
		return true;
	}
	return false;
}
bool http_fota_read_file(char *resbuff,char *filename){
	char cmd[100]={'\0'};
	set_ec200_response_check("+QHTTPREADFILE: 0");
	sprintf(cmd,"AT+QHTTPREADFILE=\"%s\",60\r\n",filename);
	if (EC200_Transmit(cmd, resbuff, EC200MAXRESONSETIME)) {
		printc(MinDelay, "%s\n", resbuff);
		return true;
	}
}
uint16_t file_open(const char * fileName,int mode){
	uint8_t resbuff[500] = { };
	char cmd[100]={'\0'};
	uint8_t opbuffer[5][100]={0};
	size_t buffsize = 100;
	set_ec200_response_check("+QFOPEN: ");
	snprintf(cmd, sizeof(cmd),"AT+QFOPEN=\"UFS:%s\",%d\r\n",fileName,mode);
	if(EC200_Transmit(cmd,resbuff,10000)){
		printc(MinDelay, "Res For AT+QFOPEN is %s\n", resbuff);
		splitString(resbuff,": ",opbuffer,buffsize);
		return atoi((char *)opbuffer[1]);
	}
	return false;
}
bool file_close(uint16_t filehandler){
	uint8_t resbuff[500] = { };
	char cmd[100]={'\0'};
	snprintf(cmd, sizeof(cmd),"AT+QFCLOSE=%d\r\n",filehandler);
	set_ec200_response_check("OK");
	if(!EC200_Transmit(cmd,resbuff,10000)){
		printc(MinDelay, "fail close fail\n");
		return false;
	}
	else{
		printc(MinDelay, "Res For AT+QFCLOSE is %s\n", resbuff);
		return true;
	}
}
bool file_read(uint16_t fileHandler,int length,char*resbuff){
	char cmd[100]={'\0'};
	//char resBuffer[2000]={0};
	sprintf(cmd, "AT+QFREAD=%d,%ld\r\n",fileHandler,length);
	printc(MinDelay, "%s\n",cmd);
	set_ec200_response_check("CONNECT");
	if(EC200_Transmit(cmd,resbuff,10000)){
		if(receivedvalue<1024)
		{
			printc(100, "incomplete read file");
		}
		return true;}
	else
		return false;
}

bool file_read_non_it(uint16_t fileHandler,int length,char*resbuff){
	char cmd[100]={0};
	char rec_buff[1046]={0};
	int reclength=4;
	char snum[100];
	sprintf(cmd, "AT+QFREAD=%d,%d\r\n",fileHandler,length);
	printc(MinDelay, "%s\n",cmd);
	//set_ec200_response_check("CONNECT");
	sprintf(snum, "%d", length);
	reclength = strlen(snum);
	HAL_UART_Transmit(&huart1, (uint8_t*) cmd, strlen(cmd), 2000);
	//
	int rec_status = HAL_UART_Receive(&huart1, resbuff, length+18+reclength, 10000);
	printc(1000, "%d rec status",rec_status);
	if(rec_status==0)
		return true;
	else
		return false;
}
bool file_delete(const char *fileNam){
	uint8_t resbuff[500] = {};
	char cmd[100]={'\0'};
	set_ec200_response_check("OK");
	sprintf(cmd,"AT+QFDEL=\"%s\"\r\n",fileNam);
	if(!EC200_Transmit(cmd,resbuff,EC200MINRESONSETIME)){return false;}
	else{
		printc(MinDelay, "Res For AT+QFDELETE is %s\n", resbuff);
		return true;
	}
}
bool file_seek(int fileHandler){
	char cmd[50]={};
	char resBuff[100]={};
	sprintf(cmd,"AT+QFSEEK=%d,0,0\r\n",fileHandler);
	return EC200_Transmit(cmd,resBuff,EC200MAXRESONSETIME);
}
bool http_post(char* data,char *resbuff){
	bool result = false;
	char tmpCmd[100]={};
	sprintf(tmpCmd,"AT+QHTTPPOST=%d\r\n",strlen(data));
	set_ec200_response_check("CONNECT");
	if(EC200_Transmit(data,resbuff,EC200MAXRESONSETIME)){
		clearBuffer(resbuff, strlen(resbuff));
		set_ec200_response_check("OK");
		if(EC200_Transmit(data,resbuff,EC200MAXRESONSETIME)){
			result = true;
		}
	}
	return result;
}
//bool checking_network(){
//	int EC200_counter=0;
//	Network:
//	char resbuff[100]={0};
//	if (EC200_Transmit("AT+CSQ\r\n", resbuff,2000)) {
//		printc(MinDelay, "Res For CSQ is %s\n", resbuff);
//		if(strstr(resbuff,"+CSQ: 99,99")!=NULL){
//			if(EC200_counter<3){
//				EC200_counter++;
//				goto Network;
//			}
//			else{
//				printc(MinDelay,"Signal Not found");
//				return false;
//			}
//		}
//	}
//}
//bool http_file_send(char*name){
//	char resbuff[100]={};
//	char tmp[100]={};
//	sprintf(tmp,"AT+QHTTPCFG=\"form/data\",\"file\",\"%s\",\"image/jpeg\"\r\n",name);
//	set_ec200_response_check("OK");
//	if(!EC200_Transmit(tmp,resbuff,EC200MINRESONSETIME)){
//		printc(10,"file configuration fail\n");
//		return false;
//	}
//	return true;
//}
//bool http_post_file(char*name){ 	//image
//	char resbuff[100]={0};
//	char tmp[100]={0};
//	set_ec200_response_check("+QHTTPPOSTFILE: 0,200");
//	sprintf(tmp,"AT+QHTTPPOSTFILE=\"%s\",80\r\n",name);
//	if(!EC200_Transmit(tmp,resbuff,EC200MAXRESONSETIME)){
//		return false;
//	}
//	return true;
//}
