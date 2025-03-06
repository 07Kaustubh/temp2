/*
 * TML.c
 *  Created on: Apr 9, 2024
 *  Author: 91944
 */
#ifndef INC_TML_C_
#define INC_TML_C_
#include "TML.h"
extern UART_HandleTypeDef huart5;
extern UART_HandleTypeDef huart6;
extern UART_HandleTypeDef huart1;
extern TIM_HandleTypeDef htim6;
extern ADC_HandleTypeDef hadc1;
extern ADC_HandleTypeDef hadc2;
extern RTC_HandleTypeDef hrtc;
extern LedHendler ledHendler;
extern IWDG_HandleTypeDef hiwdg;
extern bool isTimeSet;
extern bool isDateSet;
extern bool Fotaflags[3];
extern bool TIMER_DIABLE_FLAG;
int seconds = 0, nsam_seconds = 0;
double latHitory = 0, lonHistory = 0;
int count = 0;
int sleepcount = 0;
int FRAME_NUMBER = 1, mccCount = 0;
//int SAMPLIMG_FREQUENCY;
int Wait_time = 0;
bool isEC200Working=false;
GPSData L86Data;
char finalData[MAX_DATA_SIZE] = {0};
char historyData[MAX_DATA_SIZE] = {0};
extern uint64_t CP, CD;
uint64_t SP;
extern uint32_t L1, L2, L3;
bool SendIFDataFlag = false,gsmUpdateFlag=false;
bool PowerOnFlag = true, powerOnCount = false; // unused
//Queue dataQueue;
bool bleSmsWakeupStatus=false;
Stack dataQueue;
Stack packetHistoryQueue;
bool interflag = false;
bool sendPDDataFlag = false, FLAG_TCP_Pass = false, FLAG_TCP_Fail = false,
		NF_RD_FLAG = false, NF_WR_FLAG = false;
//Queue dataQueue;
//commented appadress cause not used anywhere
//uint8_t appAddrs = L2_v2; //we have to change the appAdrrs when new update has to be published
char Packet_Header[] = "MMI123", Hardware_Version[] = "1.0.1",
		Firmware_Version[] = "5.13.68",
		//		Config_Version[] = "1.0",   // Updating FLash Base Address
		Packet_Type[]="NR",Message_ID[] = "1", Packet_Status[] = "L",
		MODEL_NAME[]="sCAN_AX1";
int FLAG_IGN = 0;
char tmpMain[200]={0};
float inputVoltage = 0,internalVoltage = 0,AinVoltage=0.3;
bool vehicalBatteryStatus = false;
bool IMU_FLAG_SET = true;
extern bool SRESET_FLAG;
extern bool updateStatus;
bool uartCmdStatus = false;
uint8_t CMD_Buf[200] = { };
int Tx_Counter, TCP_fail_counter, UpdateCheckerCnt = 0;
char updateVersion[32] = {0};		//changes 32 previous it is 6
extern FLASH_AddressTypeDef write_Address, L86_Read_Address;
extern char ComKeys[8][33], StatKeys[8][33];
char tmp_str[500] = {0};
char mqttString[MAX_DATA_SIZE] = { };
int8_t signalStrength = 0;
extern char MNC[20], MCC[20], LAC[20], Celid[20];
char Network[30] = "NA", temper = 'C';
extern uint64_t NBlock, NPage;
int upcnt = 0;
uint8_t StrtFotaTmbuf[2] = { 0 }, getFotaTmbuf[2] = { 0 }, ConfigFotaTmbuf = 6;
extern long int currentEpochTime;
extern long int lastFotaTime;
extern int updateTimeFota;
char CP_Data[8] = {0};
char tmpSplit[5][100];
char PHnum[10] = {0};
float internal = 0;
long long int lastTimeUpdate=0,lastFotaRetry=0,gnssTimeDateCounter,timedateTimer;
uint8_t fotaRetryCounter=0,Dout1=0,Dout2=0,DigitalOutput=00,tcpFotaTime=0,tcpTwoFotaTime=0,httpFotaTime=0;
uint16_t DigitalInput=0000;
bool Din1Enable=false,Din2Enable=false,bleWakeup=false;
uint8_t SAMPLING_TIME=9,AdcMccFrequency=6;
int fotaCount=0,blesleepcounter=0,timer_counter=0,Ec200Workingcounter=0;
extern uint8_t TxData[3][8];//,TxDataDemo[8]={0};//={0x2B,0xA2,0x30,0x00,0x01,0x00,0x00,0x00};
//uint8_t TxDataDemo[8]={0};//{0x2B,0xA2,0x30,0x00,0x00,0x00,0x00,0x00};
//char *configVersion={0};
//uint8_t arrayCanTX[8]={0};
char *canTx;
//bool send_status=false,testing_function=false;
void init() {
	CHG_ON();
	setupPrintUart(&huart6, true);
	getinitFlashConfig();
	check_immobilization();
	init_GPSData(&L86Data);
	initRTC(&hrtc);
	initializeStack(&dataQueue, 30, MAX_DATA_SIZE);
	initializeStack(&packetHistoryQueue, 80, MAX_DATA_SIZE);
	L86_setup(&huart5);
	initEvb(&hadc1);
	initIvb(&hadc2);
	//initAin(&hadc1);
	HAL_TIM_Base_Start_IT(&htim6);
	printc(MinDelay, "\nsCAN Set Up Started.....\nVersion %s\n",Firmware_Version);
	uint8_t ec200retrycount=0;
	resetEC200: if (setupEc200(&huart1)) {
		isEC200Working=true;
		printc(MinDelay, "SetupEc200Done\n");
	} else {
		printc(MinDelay, "SetupEc200 Failed\n");
		if(ec200retrycount<3){
			ec200retrycount++;
			goto resetEC200;
		}
	}
		get_gnss_time_date();
	SMS_Init();
	if(bleSmsWakeupStatus)
		return;
	sprintf(updateVersion,"5.8.00");
	http_init();
	//		setConfigValue(Config_PowerFlag, "T");
	char *temppwerVal; // Adjust size as needed based on the maximum expected length of the config values
	temppwerVal = getConfigValue(Config_PowerFlag);
	if (temppwerVal[0] == 'T') {
		//	setConfigValue(Config_PowerFlag, 'S');
		if(FOTA_Download_check()){
			timer_counter++;
		}
	}
	/****************************************************************************/
	//SPI_FLASH_ReadID();er_log();flushBufferToFlash();readLogsFromFlash();
	/*******************************************************************************/
	//ble_init();
	setConfigValue(Config_PowerFlag, "T");
	saveConfigToFlash();
	LSM6DSL_EnableAccelerometer();
	IMU_FLAG_SET = true;
	SMS_Init();
	Check_USMS();
	if (SP == 1) {
		char *tPhnum = { 0 };
		tPhnum = getConfigValue(Config_PHNUM);
		memcpy(PHnum, tPhnum, 10);
		if (PHnum != NULL) {
			char SmBuff[] = {"FOTA Update Complete"};
			SMS_FSend(PHnum, SmBuff);
			writeF_CP((uint64_t) 1, (uint64_t) 0, (uint64_t) 0);
		}
	}
	initCan();
	TCP_init();
	if(lastFotaTime==0){
		lastFotaTime = currentEpochTime;
		printc(MinDelay,"Last FOTA Time=%d\n",lastFotaTime);
	}
	printc(MinDelay, "\nsCAN Set Up Done\n");
	saveTcuInfo();
}

void loop() {
	if(!isEC200Working){
		Ec200Workingcounter++;
		if(Ec200Workingcounter>30){
			SET_RESET();
		}
	}
	if(get_ble_read()){
		parse_ble_message();
	}
	if(getTcpConfigStatus()){
		if ((get_Fotatime_difference(tcpFotaTime, currentEpochTime))> 60){
			TCP_init();
			tcpFotaTime=currentEpochTime;
		}
	}
	if(get_http_Config_status()){
		if ((get_Fotatime_difference(httpFotaTime, currentEpochTime))> 60){
			http_init();
			httpFotaTime=currentEpochTime;
			//set_http_Config_status(false);
		}
	}
	if(get_tcp_receive()){
		tcp_receive();
		set_tcp_receive(false);
	}
	if(get_immobilization_tcp()){
		uint8_t value=atoi(getConfigValue(Config_IMZ));
		if(value==1){
			TX_DataCAN(TxData[0]);
		}
		else if(value==0){
			TX_DataCAN(TxData[1]);
		}
	}
	check_immobilization();
	if(!getDin2Status()){
		Din2Enable=true;
	}
	if(!(isDateSet&&isTimeSet)){
		if ((get_Fotatime_difference(timedateTimer, currentEpochTime))> 60){
			get_gnss_time_date();
			timedateTimer=currentEpochTime;
		}
	}
	if(gsmUpdateFlag){
		gsmParameterUpdate();
		gsmUpdateFlag=false;
	}
	if (sleepcount > 1) {
		sleepcount = 0;
		GO_TO_SLEEP();
	}
	else if(blesleepcounter>3){
		blesleepcounter=0;
		GO_TO_SLEEP();
	}
	if (FLAG_IGN == 1) {
		generateData("IN");
		sendPDDataFlag = true;
		task2();
		FLAG_IGN = 0;
	}
	//	if (getPrintUartCmdStatus()) {
	//		char cotaResp[200] = { };
	//		//SMS_COTA(CMD_Buf, cotaResp)
	//		if (COTA_process(1, CMD_Buf, cotaResp)) {
	//			printc(MinDelay, "SMS COTA Success %s\n", cotaResp);
	//		} else {
	//			printc(MinDelay, "SMS COTA Fail\n");
	//		}
	//		setPrintUartCmdStatus(false);
	//	}

	// Handle When any SMS come for get information
	if (getMessageStatus()){
		handleCotaSMS();
	}
	//When update is available it will update the device
	if (getUpdateStatus()) {
		//retry handled within fota
		if(update()) {
		}
		setUpdateStaus(false);
	}
	else {
		task2();
	}
	/*updatetimefota depend on that time it will come*/
	if ((get_Fotatime_difference(lastFotaTime, currentEpochTime))> 3600){
		fotaRetryCounter=0;
		lastFotaTime = currentEpochTime;
		FOTA_Download_check();
		fotaCount++;
		SET_RESET();
	}
	/*every 1 minute it will come and check*/
	if(Fotaflags[PACKET_ONE_FAIL] || Fotaflags[PACKET_THREE_FAIL]){
		if((get_Fotatime_difference(lastFotaRetry, currentEpochTime)) > 60) {
			if(fotaRetryCounter<3 && check_fota_retry()){
				fotaRetryCounter++;
			}
			lastFotaRetry = currentEpochTime;
		}
	}
	if(getTcpConfigStatus()){
		if ((get_Fotatime_difference(tcpFotaTime, currentEpochTime))> 60){
			TCP_init();
			tcpFotaTime=currentEpochTime;
		}
	}
	if(get_http_Config_status()){
		if ((get_Fotatime_difference(httpFotaTime, currentEpochTime))> 60){
			http_init();
			httpFotaTime=currentEpochTime;
			//set_http_Config_status(false);
		}
	}
}

void saveTcuInfo(){
	setConfigValue(Config_HW_VER, Hardware_Version);
	setConfigValue(Config_FW_VER, Firmware_Version);
	setConfigValue(Config_ModelName, MODEL_NAME);
}

void check_immobilization(){
	Dout1=atoi(getConfigValue(Config_DOUT1));
	Dout2=atoi(getConfigValue(Config_DOUT2));
	if(Dout1==1){
		D1_ON();
	}
	else if(Dout1==0){
		D1_OFF();
	}
	if(Dout2==1){
		D2_ON();
	}
	else if(Dout2==0){
		D2_OFF();
	}
}
void onTemperCallback() {
	generateData("TA");
}
void handleCotaSMS() {
	setMessageStatus(false);
	clearBuffer(CMD_Buf, sizeof(CMD_Buf));
	SMS_Read(0,(uint8_t *) CMD_Buf);
	if(strstr(CMD_Buf,"+CMGR:")==NULL){
		return;
	}
	if (strlen(CMD_Buf) > 6) {
		clearBuffer((char *)tmpSplit, sizeof(tmpSplit));		//is this really clear buffwr?
		splitString(CMD_Buf, "\r\n", tmpSplit, 4);
		clearBuffer(CMD_Buf, sizeof(CMD_Buf));
		if (tmpSplit[1][0] == '\0') {		//added[0]
			printc(MinDelay, "Failed to Parse Number\n");
			return;
		}
		if (tmpSplit[2][0] == '\0') {	//added[0]
			printc(MinDelay, "Failed to Parse Message");
			clearBuffer(CMD_Buf, sizeof(CMD_Buf));
			strcpy(CMD_Buf, "Failed to Parse Message\n");
		}
		else{
			printc(MinDelay, "Received MSG %s", CMD_Buf);
			//if (!SMS_COTA(tmpSplit[2], CMD_Buf))
			if (!parseCommandSMS(tmpSplit[2], CMD_Buf)) {
				clearBuffer(CMD_Buf, sizeof(CMD_Buf));
				strcpy(CMD_Buf, "INVALID CMD");
			}
			//else {
			//char *tmpData = strdup(CMD_Buf);
			//sprintf(CMD_Buf, "%s", tmpData);
			//}
		}
		clearBuffer(tmpMain,sizeof(tmpMain));
		memcpy(tmpMain,tmpSplit[1],sizeof(tmpSplit[1]));
		//		char *number = strdup(tmpSplit[1]);

		clearBuffer((char *)tmpSplit, sizeof(tmpSplit));
		splitString(tmpMain, ",", tmpSplit, 3);
		for(int SFC=0;SFC<3;SFC++){
			if(SMS_Send(&tmpSplit[1][4], CMD_Buf)){
				break;
			}
		}
		SMS_Delete(0);
		if (tmpSplit[1][2] != '\0') {
			memset(PHnum, 0, sizeof(PHnum));
			memcpy(PHnum, &tmpSplit[1][4], 10);
		}
		printc(MinDelay, "PhoneNum:%s\n",tmpSplit[1]);
	} else {
		printc(MinDelay, "UNABLE TO Parse Message");
	}
	setMessageStatus(false);
	if (SRESET_FLAG == true) {
		SET_RESET();
	}
}

char* getUpdateVersion() {
	return updateVersion;
}
void setIMEI(char *imei) {
	if (imei == NULL)
		return;
	setConfigValue(Config_IMEI, imei);
}
void Sleep_routine() {
	int retries = 0;
	int Flen = strlen(Firmware_Version);
	//		ledHendler.pwrLedState=OFF;
	//		ledHendler.gsmLedState=OFF;
	//		ledHendler.gpsLedState=OFF;
	char tmpVal[15] = {0};
	sprintf(tmpVal,"%.6f", latHitory);
	setConfigValue(Config_LAT, tmpVal);
	memset(tmpVal,0,15);
	sprintf(tmpVal, "%.6f", lonHistory);
	setConfigValue(Config_LON, tmpVal);
	gnss_data_off();
	//	uint8_t SAVE_LC_DATA[] = "$PAIR511*3F\r\n";
	//	uint8_t data[100]={0};
	memset(tmpVal,0,15);
	HAL_UART_Transmit(&huart5,  "$PAIR511*3F\r\n", strlen("$PAIR511*3F\r\n"), 1000);
	HAL_UART_Receive(&huart5, tmpVal, 15, 1000);
	//	print(1000, data, 200);
	//	printc(MinDelay, "\n");
	//	clearBuffer(data, sizeof(data));
	task2();
	while (retries < 6 && packetHistoryQueue.top >= 0) {
		clearBuffer(historyData, sizeof(historyData));
		pop(&packetHistoryQueue, historyData);
		historyData[20 + Flen] = 'H';
		if (!TCP_Send(historyData)) {
			//		if(send_status==false){
			push(&packetHistoryQueue, historyData, strlen(historyData));
			retries++;
		}
		HAL_IWDG_Refresh(&hiwdg);
	}
	while (packetHistoryQueue.top >= 0) {
		NF_WR_FLAG = true;
		clearBuffer(historyData, sizeof(historyData));
		pop(&packetHistoryQueue, historyData);
		historyData[20 + Flen] = 'H';
		if(!writeL86Data(historyData, MAX_DATA_SIZE)){
			push(&packetHistoryQueue, historyData, strlen(historyData));
		}
		HAL_IWDG_Refresh(&hiwdg);
	}
	//Firware Update after a successfull download Over FOTA
	//FeedAllConfig();
	//	saveConfigToFlash();		//because afte sleep routine i am calling
	//	GNSS_OFF();
	//	sendPDDataFlag = true;
	//	setSleppStatus(true);
}

void Handle_LEDS() {
	ledHendler.pwrLedState = SOLID;
	ledHendler.gsmLedState = LOW;
	ledHendler.gpsLedState = LOW;
}

void setPrintUartCmdStatus(bool tmpStatus) {
	uartCmdStatus = tmpStatus;
}

bool getPrintUartCmdStatus() {
	return uartCmdStatus;
}

void onPrintUartCmd(char *cmd) {
	clearBuffer(CMD_Buf, sizeof(CMD_Buf));
	strcpy(CMD_Buf, cmd);
	setPrintUartCmdStatus(true);
}

void gsmParameterUpdate() {
	bool tmpStatus = false;
	signalStrength = getSignal();
	if (signalStrength == -1) {
		printc(MinDelay, "Fail to get Signal");
		signalStrength = 0;
	}
	if(signalStrength>8){
		get_MCC_MNC_CellID();
	}
	if (Network[0] == 'N') {
		tmpStatus = getOperatorName(Network);
		Network[strlen(Network) - 1] = '\0';
		if (!tmpStatus) {
			printc(MinDelay, "Don't get NETWRORK \n");
			strcpy(Network, "NA");
			ledHendler.gsmLedState = LOW;
		}
	} else {
		ledHendler.gsmLedState = HEIGH;
	}
}
void saveTcpHistoryPacket(){
	int Flen = strlen(Firmware_Version);

	while (packetHistoryQueue.top >= 0) {
		NF_WR_FLAG = true;
		clearBuffer(historyData, sizeof(historyData));
		pop(&packetHistoryQueue, historyData);
		historyData[20 + Flen] = 'H';
		if(!writeL86Data(historyData, MAX_DATA_SIZE)){
			push(&packetHistoryQueue, historyData, strlen(historyData));
		}
		HAL_IWDG_Refresh(&hiwdg);
	}
	while (dataQueue.top >= 0) {
		NF_WR_FLAG = true;
		clearBuffer(historyData, sizeof(historyData));
		pop(&dataQueue, historyData);
		historyData[20 + Flen] = 'H';
		if(!writeL86Data(historyData, MAX_DATA_SIZE)){
			push(&dataQueue, historyData, strlen(historyData));
		}
		HAL_IWDG_Refresh(&hiwdg);
	}
	//saveConfigToFlash();
}
void makeMessageHeader(char *mqttString, char *Packet_Type) {
	sprintf(mqttString, "$%s,%s,%s,%s,%s,%s,%s,%s", Packet_Header,
			Hardware_Version, Firmware_Version, Packet_Type, Message_ID,
			Packet_Status, getConfigValue(Config_IMEI), getConfigValue(Config_VIN));
}

void addGSMData(char *tmp_str) {
	temper = 'C';
	if (getTemperStatus()) {
		temper = 'O';
	}
	sprintf(tmp_str, "%s,%d,%d,%.2f,%.2f,%c,%d,%s,%s,%s,%s,%04d,%02d,%.2f,%s",
			&Network[1],    //"Network",
			getIgnition(),    //ignition
			vehicalBatteryStatus, inputVoltage,    //inputVoltage
			internalVoltage,    //"Battery Voltage",
			temper,    //"Alert",
			signalStrength,    //"SignalStrength",
			MCC,    //"MCC",
			MNC,    //"MNC",
			LAC,    //"LAC",
			Celid,    //"Cell ID",
			DigitalInput,	//"Digital Input",
			DigitalOutput,	//"Digital Output",
			AinVoltage,	//"Analog Input 1",
			"0.3,"	//"Analog Input 2,"
	);
}
void getDigitalInputStatus(uint16_t*status) {
	*status=0;
	if (Din1Enable) {
		*status += 1000; // Add 1000 for Din1
	}
	if (Din2Enable) {
		*status += 100;  // Add 0100 for Din2
	}
}
void getDigitalOutputStatus(uint8_t*status) {
	*status=0;
	if (Dout1) {
		*status += 10; // Add 1000 for Din1
	}
	if (Dout2) {
		*status += 1;  // Add 0100 for Din2
	}
}
void addGPSData(GPSData *data, char *tmp_str, char *tmpTime, char *tmpDate) {
	if (data->latitude != 0 && data->longitude != 0) {
		latHitory = data->latitude;
		lonHistory = data->longitude;
	}

	sprintf(tmp_str, ",%d,%s,%s,%.6f,%c,%.6f,%c,%.2f,%d,%d,%d,%.2f,%.2f,",
			data->gps_fix, // Convert bool to int
			tmpDate, tmpTime, latHitory, data->lat_dir, lonHistory,
			data->lon_dir, data->speed, data->heading, data->sat_num, data->alt,
			data->PDOP, data->HDOP);
}

void GPSDataCallback(GPSData *gpsData) {
	clearBuffer(&L86Data, sizeof(GPSData));
	memcpy(&L86Data, gpsData, sizeof(GPSData));
}

void task2() {
	int Flen = strlen(Firmware_Version);
	if (sendPDDataFlag) {
		if (dataQueue.top >= 0) {
			//TCP_Receive_Open();
			//clearBuffer(finalData, sizeof(finalData));
			clearBuffer(finalData, sizeof(finalData));
			pop(&dataQueue, finalData);
			//send the rest of the data in the dataqueue to history queue
			while (dataQueue.top >= 0) {
				clearBuffer(historyData, sizeof(historyData));
				pop(&dataQueue, historyData);
				historyData[20 + Flen] = 'H';
				//check stack size before pushing
				push(&packetHistoryQueue, historyData, strlen(historyData));
				TCP_fail_counter++;
			}
			if (!TCP_Send(finalData))
				//if(send_status==false)
			{
				finalData[20 + Flen] = 'H';
				//push if hist stack is less than
				push(&packetHistoryQueue, finalData, strlen(finalData));
				TCP_fail_counter++;
				Tx_Counter = 0;
			}
			else
			{
				Tx_Counter++;
				if (Tx_Counter >= 10) {
					FLAG_TCP_Pass = true;
				}
			}
			sendPDDataFlag = false;
		}
	}
	if (interflag) {
		if (packetHistoryQueue.top >= 0) {
			clearBuffer(finalData, sizeof(finalData));
			pop(&packetHistoryQueue, finalData);
			/*this send_status is for checking*/
			if ((!isEC200Working)||(!TCP_Send(finalData)))
				//if (send_status==false)
			{
				finalData[20 + Flen] = 'H';
				TCP_fail_counter++;

				push(&packetHistoryQueue, finalData, strlen(finalData));
				printc(MinDelay, "Stack Current Size  - %d ",
						packetHistoryQueue.top);
				//				printc(MinDelay, "Stack Current Size  - %d ",
				//						packetHistoryQueue.top);
			} else
				Tx_Counter++;

			interflag = false;
		}
	}
	if (TCP_fail_counter >= 10) {	// need to know
		FLAG_TCP_Pass = false;
		FLAG_TCP_Fail = true;
		Tx_Counter = 0;
		TCP_fail_counter = 0;
	}
	if (Tx_Counter >= 5) {			//need to know
		FLAG_TCP_Pass = true;
		FLAG_TCP_Fail = false;
		TCP_fail_counter = 0;
		Tx_Counter = 0;
	}

	if (packetHistoryQueue.top >= 70 && (!NF_RD_FLAG && !NF_WR_FLAG)
			&& FLAG_TCP_Fail) {
		if (NPage == 255 && NBlock == 255) {
			NPage = 0;
			NBlock = 0;
		}
		if (NPage == 64) {
			NBlock++;
			NPage = 0;
		}
		printc(MinDelay,"Writing History PAckets to flash from Page - %d, Block - %d, Stack - %d ",NPage, NBlock, packetHistoryQueue.top);
		for (int z = 0; z < 64; z++) {
			NF_WR_FLAG = true;
			clearBuffer(historyData, sizeof(historyData));
			pop(&packetHistoryQueue, historyData);
			historyData[20 + Flen] = 'H';
			bool Ws = writeL86Data(historyData, MAX_DATA_SIZE);

			HAL_IWDG_Refresh(&hiwdg);
			if (!Ws) {
				push(&packetHistoryQueue, historyData, strlen(historyData));
				NPage--;
			}
		}
		printc(MinDelay, "Write_NandFlash NPage - %d , NBlock - %d", NPage,NBlock);
		setFlashInttoString();
		saveConfigToFlash();
		NF_WR_FLAG = false;
		FLAG_TCP_Fail = false;
		FLAG_TCP_Pass = false;
	}
	clearBlockflash();
}

void clearBlockflash() {
	if ((NBlock > 0 || NPage > 0) && packetHistoryQueue.top == -1
			&& (!NF_RD_FLAG && !NF_WR_FLAG) && FLAG_TCP_Pass) {

		printc(MinDelay,"Read_Conditions MET_ Reading History PAckets from flash Page - %d, Block - %d",
				NPage, NBlock);

		if (RdN_Block()) {
			printc(MinDelay,"REad_NandFlash NPage - %d , NBlock - %d, Stack - %d ",NPage, NBlock, packetHistoryQueue.top);
		}
	}
}
void generateData(char *packetType) {
	memset(mqttString, 0, sizeof(mqttString));
	makeMessageHeader(mqttString, packetType);
	char tmpTime[7] = { 0 };
	char tmpDate[9] = { 0 };
	getDateTime(tmpDate, tmpTime);
	//add GPS DATA
	clearBuffer(tmp_str, sizeof(tmp_str));
	addGPSData(&L86Data, tmp_str, tmpTime, tmpDate);
	strcat(mqttString, tmp_str);
	memset(tmp_str, 0, sizeof(tmp_str));
	inputVoltage = 0;
	vehicalBatteryStatus = false;
	if (getInputVoltage(&inputVoltage)) {
		if (inputVoltage > 7) {
			vehicalBatteryStatus = true;
		}
	}
	//	AinVoltage=0;
	//	if(getAin1(&AinVoltage)){
	//		printc(MinDelay,"Analog Voltage:%f\n",AinVoltage);
	//	}
	internalVoltage = 0;
	getInternalBattery(&internalVoltage);
	getDigitalInputStatus(&DigitalInput);
	getDigitalOutputStatus(&DigitalOutput);
	addGSMData(tmp_str);
	strcat(mqttString, tmp_str);
	memset(tmp_str, 0, sizeof(tmp_str));
	Map copyCanData;
	getCANData(&copyCanData);
	char canStr[NUM_PID * 8 * 8] = { };
	for (int i = 0; i < copyCanData.size; i++) {
		char tmpCan[32] = { };
		sprintf(tmpCan, "%08X:", copyCanData.pairs[i].key);
		for (uint8_t x = 0; x < 8; x++)
			sprintf(tmpCan + strlen(tmpCan), "%02X",
					copyCanData.pairs[i].value[x]);
		if (i + 1 != copyCanData.size) {
			strcat(tmpCan, ";");
		}
		strcat(canStr, tmpCan);
	}
	strcat(mqttString, canStr);
	strcat(mqttString, ",()"); //OTA
	sprintf(mqttString + strlen(mqttString), ",%d", FRAME_NUMBER);
	uint8_t checksum = Calculate_CRC8(&mqttString[1], strlen(mqttString) - 1);
	sprintf(mqttString + strlen(mqttString), "*%02X", checksum);
	printc(1000, "Data To Send\n%s\n", mqttString);
	push(&dataQueue, mqttString, strlen(mqttString));
	FRAME_NUMBER++;
	if(FRAME_NUMBER>=999){
		FRAME_NUMBER=1;
	}
}
int IMU_Seconds = 0;
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim6) {

	if (htim6->Instance == TIM6) {
		seconds++;
		Wait_time++;
		nsam_seconds++;
//				IMU_Seconds++;
	}
	//	if ((IMU_Seconds % 10 == 0) && (IMU_FLAG_SET)) {
	//		accel_raw();
	//		gyro_raw();
	//		IMU_Seconds = 0;
	//	}
	if(TIMER_DIABLE_FLAG){
		return;
	}
	blinkLed(seconds);
	//sleepstatus variable getting corrupted
	//	if (getSleepStatus())
	//		return;
	//	Setting Up sampling frequency if we have latitude and longitude
	if (seconds >= (SAMPLING_TIME * 10)) {
		//		setMessageStatus(true);	//
		//generateData("NR");
		if (!getUpdateStatus()) {
			ledHendler.gsmLedState = HEIGH;
			generateData("NR");
			mccCount++;
			if (mccCount > AdcMccFrequency) {
				gsmUpdateFlag=true;
				mccCount = 0;
			}
		}
		else{
			ledHendler.gsmLedState = LOW;
		}
		seconds = 0;
		//		if (!getIgnition()) {
		//			if (sleepcount == 0 && SendIFDataFlag) {
		//				generateData("IF");
		//				SendIFDataFlag = false;
		//			}
		//			sleepcount++;
		//		} else {
		//			sleepcount = 0;
		//			SendIFDataFlag = true;
		//		}
		if (!getIgnition()) {
			if (bleSmsWakeupStatus) {
				blesleepcounter++;
				//				generateData("RI");
			} else {
				if (sleepcount == 0 && SendIFDataFlag) {
					generateData("IF");
					SendIFDataFlag = false;
				}
				sleepcount++;
			}
		} else {
			sleepcount = 0;
			blesleepcounter = 0;
			SendIFDataFlag = true;
			bleSmsWakeupStatus=false;
		}
	}
	//gen data greater than, right now considering same sampling freq
	if (nsam_seconds >= (10 * SAMPLING_TIME)) {
		sendPDDataFlag = true;
		interflag = false;
		nsam_seconds = 0;
	}
	if (!((nsam_seconds > ((SAMPLING_TIME - 2) * 10)) || (sendPDDataFlag))) {
		interflag = true;
	} else {
		interflag = false;
	}

}
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {

	if (huart->Instance == UART5) {
		L86_RxCpltCallback();
	}
	//	} else if (huart->Instance == USART1) {
	//		E200_RXCpltCallback();
	//	}
}
void HAL_UART_RxHalfCpltCallback(UART_HandleTypeDef *huart) {
	if (huart->Instance == UART5) {
		L86_RxHalfCpltCallback();
	}
	//	else if (huart->Instance == USART1) {
	//		E200_RxHAlfCpltCallback();
	//	}
}
void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size) {
	if (huart->Instance == UART5) {
		//	L86_RxCpltCallback();
	} else if (huart->Instance == USART1) {
		Ec200_RxEventCallback(Size);
	} else if (huart->Instance == USART6) {
		print_RxEventCallback(Size);
	}

}
//void setSampling(int sampling) {
//	SAMPLIMG_FREQUENCY = sampling;
//}
void clearHistory(){
	NBlock=0;
	NPage=0;
	packetHistoryQueue.top=-1;
	saveConfigToFlash();
}
void Check_USMS() {
	char Sp_Data[8] = { 0 };
	FLASH_Read_Buff(Current_App_LOC + 32, Sp_Data, 8);
	SP = Sp_Data[0];
}

#endif /* INC_TML_C_ */
