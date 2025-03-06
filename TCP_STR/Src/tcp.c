/*
 * tcp.c
 *  Created on: April 23, 2024
 *      Author: saurabh
 */

#include "tcp.h"
char tcp_buff[2050];
char *SERVER_IP = {0};
char *SERVER_PORT = {0};
char *SERVER_IP2 = {0};
char *SERVER_PORT2 = {0};
uint8_t tcpFailCounter,tcpOpenCounter;
/*********************************/
bool tcpConfigStatus=false;
bool getTcpConfigStatus(){
	return tcpConfigStatus;
}
void setTcpConfigStatus(bool tcpconfig){
	tcpConfigStatus=tcpconfig;
}
/*********************************/
bool setTcpReceive=false;
bool get_tcp_receive(){
	return setTcpReceive;
}
void set_tcp_receive(bool tcpReceive){
	setTcpReceive=tcpReceive;
}
/**************************************/
bool tcpImmobilaizationFlag=false;
bool get_immobilization_tcp(){
	return tcpImmobilaizationFlag;
}
void set_immobilization_tcp(bool tcpImmobilaization){
	tcpImmobilaizationFlag=tcpImmobilaization;
}
/****************************************/
void clear_tcp_resp_buff() {
	clearBuffer(tcp_buff, sizeof(tcp_buff));
}

void TCP_init() {
	enableIdle();
	if (TCP_config()) {
		tcpConfigStatus=false;
		printc(MinDelay, "SUCCESS:TCP Configuration\n");
	}
	else {
		tcpConfigStatus=true;
		printc(MinDelay, "ERROR:TCP Configuration\n");
	}
}

bool TCP_config() {
	bool tcpResult=true;
	char Response_tcp_buff[1000]={0};
	clearBuffer(Response_tcp_buff, sizeof(Response_tcp_buff));
	//	set_ec200_response_check("+CEREG: 0,1");
	if (!EC200_Transmit("AT+CEREG?\r\n", Response_tcp_buff, 2000)) {
		printc(MinDelay, "ERROR:Not Registered\n");
		tcpResult=false;
	}
	clearBuffer(Response_tcp_buff, sizeof(Response_tcp_buff));
	set_ec200_response_check("OK");
	if (!EC200_Transmit("AT+QIDEACT=1\r\n", Response_tcp_buff, 2000)) {
		printc(MinDelay, "ERROR:Failed to close the Internet\n");
		tcpResult=false;
	}
	clearBuffer(Response_tcp_buff, sizeof(Response_tcp_buff));
	char command[100] = {0};
	set_ec200_response_check("OK");
	sprintf(command, "AT+QICSGP=1,1,\"%s\",\"\",\"\",0\r\n",getConfigValue(Config_DAPN));
	if (!EC200_Transmit(command, Response_tcp_buff,2000)) {
		printc(MinDelay, "ERROR:Failed to SET APN\n");
		tcpResult=false;
	}
	clearBuffer(Response_tcp_buff, sizeof(Response_tcp_buff));
	set_ec200_response_check("OK");
	if (!EC200_Transmit("AT+QIACT=1\r\n", Response_tcp_buff, 2000)) {
		printc(MinDelay, "ERROR:Failed to DATA ENABLE\n");
		tcpResult=false;
	}
	set_ec200_response_check("OK");
	if (!EC200_Transmit("AT+QIDNSCFG=1,\"8.8.8.8\",\"8.8.4.4\"\r\n", Response_tcp_buff, 2000)) {
		printc(MinDelay, "ERROR:DNS Fail\n");
		tcpResult=false;
	}
	clearBuffer(Response_tcp_buff, sizeof(Response_tcp_buff));
	if (!EC200_Transmit("AT+QIACT?\r\n", Response_tcp_buff, 2000)) {
		printc(MinDelay, "ERROR:ACT FAIL\n");
		tcpResult=false;
	}
	return tcpResult;
}
bool TCP_Open() {
	SERVER_IP = getConfigValue(Config_MIP);
	SERVER_PORT = getConfigValue(Config_MPORT1);
	bool result=false;
	char command[100] = {0};
	sprintf(command, "AT+QIOPEN=1,1,\"TCP\",\"%s\",%s,0,0\r\n",SERVER_IP,SERVER_PORT);
	printc(MinDelay, "tcp open:%s\n", command);
	clearBuffer(tcp_buff, sizeof(tcp_buff));
	set_ec200_response_check("+QIOPEN:");
	if (EC200_Transmit(command,tcp_buff, 7000)){
		if(strstr(tcp_buff,"1,563")||strstr(tcp_buff,"1,0")){
			tcpFailCounter=0;
			tcpOpenCounter=0;
			printc(MinDelay, "%s\n ", tcp_buff);
			result=true;
		}
		else if(strstr(tcp_buff,"1,567")){
			printc(MinDelay, "TCP OPEN FAIL%s\n",tcp_buff);
			tcpConfigStatus=true;
			tcpFailCounter++;
		}
		printc(MinDelay, "TCP OPEN %s\n",tcp_buff);
	}
	else {
		//		tcpConfigStatus=true;
		tcpOpenCounter++;
		if(tcpOpenCounter>=10){
			tcpOpenCounter=0;
			saveTcpHistoryPacket();
//			GPIO_LTE_OFF();
//			HAL_Delay(5000);
			SET_RESET();
		}
	}
	////////////////////////////
	if(result==false){
		tcp_close(1);
	}
	if(tcpFailCounter>=50){
		tcpFailCounter=0;
		saveTcpHistoryPacket();
		SET_RESET();
	}
	return result;
}
// for testing remove tcp
//bool tcp_send=false;
bool TCP_Send(const char *data) {
	uint8_t command[50]={0};
	bool result = false;
	clear_tcp_resp_buff();
	if(TCP_Open()){
		sprintf(command, "AT+QISEND=1,%d\r\n", strlen(data));
		clear_tcp_resp_buff();
		printc(MinDelay,"%s \n",command);
		EC200_Transmit(command, tcp_buff, EC200MINRESONSETIME);
		clear_tcp_resp_buff();
		set_ec200_response_check("SEND OK");
		if (EC200_Transmit(data, tcp_buff, EC200MAXRESONSETIME)){
			result = true;
		}
		else {
			printc(MinDelay, "Don't Get Response For Send\n");
			tcp_close(1);
			result = false;
		}
	}
	clear_tcp_resp_buff();
	return result;
}
bool tcp_close(uint8_t CONNCTION_ID) {
	clear_tcp_resp_buff();
	char command[150] = {0};
	set_ec200_response_check("OK");
	sprintf(command, "AT+QICLOSE=%d\r\n",CONNCTION_ID);
	if(EC200_Transmit(command, tcp_buff, EC200MAXRESONSETIME)){
		printc(MinDelay, "SocketClose Response %s\n", tcp_buff);
		return true;
	}
	return false;
}

bool tcp_receive() {
	char res_buff[1000]={0};
	set_ec200_response_check("+QIRD:");
	if (EC200_Transmit("AT+QIRD=1,1500\r\n", res_buff, EC200MAXRESONSETIME)) {
		if(extractCommandValue(res_buff)){
			printc(MinDelay,"SUCCESS\n");
		}
		else{
			printc(MinDelay,"FAILURE\n");
		}
		printc(MinDelay, "Received Data: %s\n", res_buff);
	} else {
		printc(MinDelay, "Failed to execute AT+QIRD.\n");
		return false;
	}
}

void check_tcp_response() {
	if (compare_ec200_response("+QIURC: \"recv\",1")){//||compare_ec200_response("+QIURC: \"closed\",1",res_buffer)) {{
		set_tcp_receive(true);
	}
}
bool extractCommandValue(const char *cmd) {
    char *startIMZ = strstr(cmd, "SET IMZ");    // Check for IMZ command
    char *startDOUT = strstr(cmd, "SET DOUT");  // Check for DOUT command

    if (startIMZ && startIMZ[7] == ':' && (startIMZ[8] == '0' || startIMZ[8] == '1')) {
        char value[2];
        value[0] = startIMZ[8];
        value[1] = '\0';
        setConfigValue(Config_IMZ, value);
        set_immobilization_tcp(true);
        return true;  // IMZ command processed
    }

    if (startDOUT &&
        (startDOUT[8] == '1' || startDOUT[8] == '2') &&
        startDOUT[9] == ':' &&
        (startDOUT[10] == '0' || startDOUT[10] == '1')) {

        uint8_t doutNumber = startDOUT[8] - '0';
        char value[2];
        value[0] = startDOUT[10];
        value[1] = '\0';

        if (doutNumber == 1) {
            setConfigValue(Config_DOUT1, value);
        } else if (doutNumber == 2) {
            setConfigValue(Config_DOUT2, value);
        }
        return true;  // DOUT command processed
    }

    return false;  // No valid command found
}

//int8_t extractDoutValue(const char *cmd, uint8_t *doutNumber) {
//    // Validate length and fixed format
//    if (strlen(cmd) == 11 &&
//        strncmp(cmd, "SET DOUT", 8) == 0 &&
//        (cmd[8] == '1' || cmd[8] == '2') &&
//        cmd[9] == ':' &&
//        (cmd[10] == '0' || cmd[10] == '1')) {
//
//        *doutNumber = cmd[8] - '0';  // Store DOUT number (1 or 2)
//        return cmd[10] - '0';  // Return only 0 or 1
//    }
//    return -1;  // Ignore invalid cases without returning anything meaningful
//}
//bool TCP_Receive_Open() {
//	bool result = false;
//	SERVER_IP2 = getConfigValue(Config_IP2);
//		SERVER_PORT2 = getConfigValue(Config_IPPORT2);
//	char res_buff[2020] = {0};
//	char command[150] = {0};
//	// Construct the AT+QIOPEN command
//	sprintf(command, "AT+QIOPEN=2,2,\"TCP\",\"%s\",%s,0,0\r\n",SERVER_IP2,SERVER_PORT2);
//	printc(10, "TCP Open Command => %s", command);
//	clearBuffer(tcp_buff, sizeof(tcp_buff));
//	set_ec200_response_check("+QIOPEN:");
//	// Send the open command
//	if (EC200_Transmit(command, tcp_buff, EC200MAXRESONSETIME)) {
//		if(strstr(tcp_buff,"2,563")||strstr(tcp_buff,"2,0")){
//			printc(MinDelay, "TCP connection opened successfully: %s\n", tcp_buff);
//			result = true;
//		} else {
//			printc(MinDelay, "TCP connection failed: %s\n", tcp_buff);
//		}
//	} else {
//		printc(MinDelay, "Failed to send AT+QIOPEN command.\n");
//	}
//	if(result==false){
//		tcp_close(2);
//	}
//	return result;
//}
//void TCP_Init_Context2() {
//	enableIdle();
//	if (TCP_Config_Context2()) {
//		tcpTwoConfigStatus=false;
//		char data[] = "READY\n";
//		if (TCP_Send_Receive(data)) {
//			printc(MinDelay, "Data Sent Successfully\n");
//		}
//		else {
//			printc(MinDelay, "Error During Data Transmission\n");
//		}
//	} else {
//		tcpTwoConfigStatus=true;
//		printc(MinDelay, "Configuration for Context ID 2 Failed\n");
//	}
//}
//bool TCP_Config_Context2() {
//	bool tcpResult=true;
//	char Response_tcp_buff[100]={0};
//	//	clearBuffer(Response_tcp_buff, sizeof(Response_tcp_buff));
//	//	set_ec200_response_check("OK");
//	//	if (!EC200_Transmit("AT+QICFG=\"tcp/keepalive\",0\r\n", Response_tcp_buff, 2000)) {
//	//		printc(MinDelay, "ERROR:keep alive fail\n");
//	//		tcpResult=false;
//	//	}
//	clearBuffer(Response_tcp_buff, sizeof(Response_tcp_buff));
//	set_ec200_response_check("OK");
//	if (!EC200_Transmit("AT+QIDEACT=2\r\n", Response_tcp_buff, 2000)) {
//		printc(MinDelay, "ERROR:Failed to close the Internet\n");
//		tcpResult=false;
//	}
//	clearBuffer(Response_tcp_buff, sizeof(Response_tcp_buff));
//	char command[100] = {0};
//	set_ec200_response_check("OK");
//	sprintf(command, "AT+QICSGP=2,1,\"%s\",\"\",\"\",0\r\n",getConfigValue(Config_DAPN));
//	if (!EC200_Transmit(command, Response_tcp_buff,2000)) {
//		printc(MinDelay, "ERROR:Failed to SET APN\n");
//		tcpResult=false;
//	}
//	clearBuffer(Response_tcp_buff, sizeof(Response_tcp_buff));
//	set_ec200_response_check("OK");
//	if (!EC200_Transmit("AT+QIACT=2\r\n", Response_tcp_buff, 2000)) {
//		printc(MinDelay, "ERROR:Failed to DATA ENABLE\n");
//		tcpResult=false;
//	}
//	clearBuffer(Response_tcp_buff, sizeof(Response_tcp_buff));
//	set_ec200_response_check("OK");
//	if (!EC200_Transmit("AT+QIDNSCFG=2,\"8.8.8.8\",\"8.8.4.4\"\r\n", Response_tcp_buff, 2000)) {
//		printc(MinDelay, "ERROR:DNS Fail\n");
//		tcpResult=false;
//	}
//	clearBuffer(Response_tcp_buff, sizeof(Response_tcp_buff));
//	if (!EC200_Transmit("AT+QIACT?\r\n", Response_tcp_buff, 2000)) {
//		printc(MinDelay, "ERROR:ACT FAIL\n");
//		tcpResult=false;
//	}
//	return tcpResult;
//}
//bool TCP_Send_Receive(const char *data) {
//	uint8_t command[100];
//	bool result = false;
//	clear_tcp_resp_buff();
//	if(TCP_Receive_Open()){
//		sprintf(command, "AT+QISEND=2,%d\r\n",strlen(data));
//		EC200_Transmit(command, tcp_buff, 10000);
//		clear_tcp_resp_buff();
//		set_ec200_response_check("SEND OK");
//		if (EC200_Transmit(data, tcp_buff, 10000)){
//			result = true;
//		}
//		else {
//			printc(MinDelay, "Don't get response for send \n");
//			tcp_close(2);
//			result = false;
//		}
//	}
//	clear_tcp_resp_buff();
//	return result;
//}
