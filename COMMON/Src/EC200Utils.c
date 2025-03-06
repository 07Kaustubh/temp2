

#include"EC200Utils.h"

UART_HandleTypeDef *ec200MqttUart;
uint8_t rx_data[2];
uint8_t EC200_ResponseBuff[RESBUFF_SIZE];
int responseIndex=0;
extern int Wait_time;
bool enable_Idle = true;
bool FLAG_EC200RESONSE=false;
bool FLAG_IS_EC200_ENABLED=false;
char tmpbuff[100]={};
bool test_status=false;
int test_count = 0;
int receivedvalue=0;
extern bool isTimeSet;
extern bool isDateSet;
char MNC[20] = "NA", MCC[20] = "NA", LAC[20] = "NA", Celid[20] = "NA";
uint8_t validateBuffer[100] = { 0 };
void set_ec200_response_check(const uint8_t *message) {
	if (message != NULL && FLAG_IS_EC200_ENABLED)
		strcpy(validateBuffer, message);
}

bool EC200_Transmit(const char *command,char *resBuff,int timeout){
	if(!FLAG_IS_EC200_ENABLED){
		printc(MinDelay,"ERROR:EC200 Not Enabled\n");
		return false;
	}
	test_status=false;
	FLAG_EC200RESONSE = false;
	Wait_time=0;
	HAL_UART_Transmit(&huart1, (uint8_t*)command,strlen(command), 2000);
	if(resBuff==NULL)
		return false;
	while((Wait_time< timeout/100)){
		if (FLAG_EC200RESONSE) {
			FLAG_EC200RESONSE = false;
			clearBuffer(resBuff,receivedvalue);
			memcpy(resBuff, EC200_ResponseBuff, receivedvalue);
			clearBuffer(EC200_ResponseBuff,RESBUFF_SIZE);
			test_status = true;
			if (validateBuffer[0] == '\0')
				break;
			if (strstr(resBuff, validateBuffer) != NULL) {
				clearBuffer(validateBuffer,sizeof(validateBuffer));
				break;
			}
			test_status = false;
		}
	}
	printc(MinDelay,"TIME:%d,FLAG:%d,STATUS:%d,RES:%s\n",
			Wait_time,FLAG_EC200RESONSE,test_status,resBuff);
	return test_status;
}
bool EC200_Init(void) {
	int EC200_counter=0;
	char resbuff[100]={0};
	if(EC200_Transmit("AT\r\n",resbuff,EC200MINRESONSETIME)){
		printc(MinDelay,"Res For At is %s\n",resbuff);
	}
	memset(resbuff,0,100);
	if(EC200_Transmit("AT+QURCCFG=\"urcport\",\"uart1\"\r\n",resbuff,EC200MINRESONSETIME)){
		printc(MinDelay,"Res For AT+QURCCFG=\"urcport\",\"uart1\"\r\n is %s\n",resbuff);
	}
	memset(resbuff,0,100);
	if(EC200_Transmit("ATV1\r\n",resbuff,EC200MINRESONSETIME)){
		printc(MinDelay,"Res For ATV1 is %s\n",resbuff);
	}
	memset(resbuff,0,100);
	if(EC200_Transmit("ATE0\r\n",resbuff,EC200MINRESONSETIME)){
		printc(MinDelay,"Res For ATE0 is %s\n",resbuff);
	}
	memset(resbuff,0,100);
	if(EC200_Transmit("AT+CMEE=2\r\n",resbuff,EC200MINRESONSETIME)){
		printc(MinDelay,"Res For AT+CMEE=2 is %s\n",resbuff);
	}
	memset(resbuff,0,100);
	if(EC200_Transmit("ATI\r\n",resbuff,EC200MINRESONSETIME)){
		printc(MinDelay,"Res For ATI is %s\n",resbuff);
	}
	memset(resbuff,0,100);
	EC200_counter=0;
	CPIN:
	if (EC200_Transmit("AT+CPIN?\r\n", resbuff,EC200MINRESONSETIME)) {
		printc(MinDelay, "Res For CPIN is %s\n", resbuff);
		if(strstr(resbuff,"+CPIN: READY")==NULL){
			if(EC200_counter<3){
				EC200_counter++;
				goto CPIN;
			}
			else{
				printc(MinDelay,"Sim Not Detected");
				GPIO_LTE_OFF();
				HAL_Delay(5000);
				SET_RESET();
				return false;
			}
		}
	}
	memset(resbuff,0,sizeof(resbuff));
	EC200_counter=0;
	network:
	if (EC200_Transmit("AT+CSQ\r\n", resbuff,2000)) {
		printc(MinDelay, "Res For CSQ is %s\n", resbuff);
		if(strstr(resbuff,"+CSQ: 99,99")!=NULL){
			if(EC200_counter<10){
				EC200_counter++;
				goto network;
			}
			else{
				printc(MinDelay,"Signal Not found");
			}
		}
	}
	memset(resbuff,0,100);
	if(getICCID(resbuff)){
		 setConfigValue(Config_ICCID, resbuff);
	}
	return true;
}
bool getICCID(char *opbuffer) {
    char tmpICCID[32] = {0};
    if (EC200_Transmit("AT+QCCID\r\n", opbuffer, EC200MINRESONSETIME)) {
        char *startPtr = strstr(opbuffer, "+QCCID: "); // Look for "+QCCID: "
        if (startPtr != NULL) {
            startPtr += 8;
            int i = 0;
            while (startPtr[i] != '\0' && startPtr[i] != '\r' && startPtr[i] != '\n' && i < sizeof(tmpICCID) - 1) {
                if (isalnum(startPtr[i])) {
                    tmpICCID[i] = startPtr[i];
                    i++;
                } else {
                    break;
                }
            }
            tmpICCID[i] = '\0';

            if (i == 20) {
                clearBuffer(opbuffer, sizeof(opbuffer));
                strcpy(opbuffer, tmpICCID);
                printc(MinDelay, "Valid ICCID: %s\n", opbuffer);
                return true;
            } else {
                printc(MinDelay, "ERROR: ICCID length is invalid: %d\n", i);
            }
        } else {
            printc(MinDelay, "ERROR: +QCCID response not found.\n");
        }
    } else {
        printc(MinDelay, "ERROR: No response for AT+QCCID command.\n");
    }
    return false;
}

bool isNumeric(const char *str, int length) {
	for (int i = 0; i < length; i++) {
		if (str[i] < '0' || str[i] > '9') {
			return false;
		}
	}
	return true;
}

bool getIMEI(char *opbuffer) {
	if (!FLAG_IS_EC200_ENABLED) {
		printc(MinDelay, "ERROR: EC200 Not enabled\n");
		return false;
	}
	for (int attempt = 1; attempt <= 3; attempt++) {
		char tmpIMEI[IMEI_LENGTH + 1] = {0};
		if (EC200_Transmit("AT+GSN\r\n", opbuffer, EC200MINRESONSETIME)) {
			char *startPtr = &opbuffer[2];
			char *endPtr = strstr(startPtr, "\r\n");
			if (endPtr != NULL) {
				int length = endPtr - startPtr;

				if (length == IMEI_LENGTH && isNumeric(startPtr, length)) {
					memcpy(tmpIMEI, startPtr, length);
					tmpIMEI[IMEI_LENGTH] = '\0';
					clearBuffer(tmpbuff, sizeof(tmpbuff));
					strcpy(opbuffer, tmpIMEI);
					printc(MinDelay, "Valid IMEI: %s\n", opbuffer);
					return true;
				}
			}
		}
		HAL_Delay(1000);
	}
	printc(MinDelay, "ERROR: Failed to retrieve IMEI after %d attempts.\n", 3);
	FLAG_IS_EC200_ENABLED = false;
	return false;
}

int rtcday=0,rtcmonth=0,rtcyear=0,rtchour=0,rtcminute=0,rtcsec=0;
bool get_gnss_time_date() {
	uint8_t resbuff[200] = { }; // Increased buffer size to handle the response
	memset(resbuff, 0, sizeof(resbuff));
	if(EC200_Transmit("AT+QLTS=1\r\n", resbuff, 5000)) {
		uint8_t split[5][100] = { };
		splitString(resbuff, "\"", split, 5);
		// Extract date and time from the second split part
		if (split[1][0] != '\0') {
			char *date_time = (char*) split[1];
			char *comma_pos = strchr(date_time, ',');
			if (comma_pos != NULL) {
				// Extract date components
				char date[11] = {0};
				size_t date_length = comma_pos - date_time;
				strncpy(date, date_time, date_length);
				sscanf(date, "%d/%d/%d", &rtcyear,&rtcmonth,&rtcday);
				rtcyear %= 100; // Convert year to two digits

				char time[9] = {0};
				char *timezone_pos = strchr(comma_pos + 1, '+');
				if (timezone_pos != NULL) {
					size_t time_length = timezone_pos - (comma_pos + 1);
					strncpy(time, comma_pos + 1, time_length);
					sscanf(time, "%d:%d:%d",&rtchour, &rtcminute, &rtcsec);
				} else {
					strcpy(time, comma_pos + 1);
					sscanf(time, "%d:%d:%d",&rtchour, &rtcminute, &rtcsec);
				}
				if(setTime(rtchour, rtcminute, rtcsec))
				{	isTimeSet=true;
					if(setDate(rtcday, rtcmonth, rtcyear)){
						isDateSet=true;
						return true;
					}
				}
			}
		}
	}
	return false;
}
//int getSignal(){
//	char resbuff[100]={};
//	uint8_t signal=-1;
//	memset(resbuff,0,sizeof(resbuff));
//	if(EC200_Transmit("AT+CSQ\r\n",resbuff,EC200MINRESONSETIME)){
//		sscanf(resbuff,"\r\n+CSQ: %d",&signal);
//		printc(MinDelay,"RSSI For AT+CSQ %s\n",resbuff);
//		return signal;
//	}
//	return -1;
//}
int getSignal() {
    char resbuff[100] = {0};  // Response buffer
    uint8_t signal = 99;      // Default to 99 (invalid or unknown signal)
    memset(resbuff, 0, sizeof(resbuff));
    if (EC200_Transmit("AT+CSQ\r\n", resbuff, EC200MINRESONSETIME)) {
        char *ptr = strstr(resbuff, "+CSQ: ");
        if (ptr) {
            ptr += 6;  // Move past "+CSQ: "
            // Convert the numeric value manually (avoiding sscanf)
            signal = 0;
            while (*ptr >= '0' && *ptr <= '9') {
                signal = signal * 10 + (*ptr - '0');
                ptr++;
            }
            printc(MinDelay, "RSSI For AT+CSQ %s\n", resbuff);
            return signal;
        }
    }
    return -1;  // Return -1 if no valid signal found
}

bool getOperatorName(char *opbuffer){
	static char split[5][100]={};
	static char resbuff[1024]={};
	memset(resbuff,0,sizeof(resbuff));
	if(EC200_Transmit("AT+COPS?\r\n",resbuff,2000)){
		clearBuffer(split, sizeof(split));
		splitString(resbuff,",",split,4);
		memcpy(opbuffer,split[2],strlen(split[2]));
		printc(MinDelay,"Res For AT+COPS? %s\n",resbuff);
		return true;
	}
	return false;
}
bool get_MCC_MNC_CellID() {
	uint8_t split[19][100] = { };
	uint8_t resbuff[100] = { };
	memset(resbuff, 0, sizeof(resbuff));
	if (EC200_Transmit("AT+QENG=\"servingcell\"\r\n", resbuff,
			5000)) {
		char *qeng_start = strstr((char*) resbuff, "+QENG:");
		if (qeng_start != NULL) {
			// Move the pointer to the start of the actual data
			//			        qeng_start += strlen("+QENG:");
			if (splitString((uint8_t*) qeng_start, ",", split, 13)) {
				clearBuffer(MCC, sizeof(MCC));
				clearBuffer(MNC, sizeof(MNC));
				clearBuffer(LAC, sizeof(LAC));
				clearBuffer(Celid, sizeof(Celid));
				if (strcmp((char*) split[2], "\"GSM\"") == 0) {

					strcpy(MCC, (char*) split[3]);
					strcpy(MNC, (char*) split[4]);
					strcpy(LAC, (char*) split[5]);
					strcpy(Celid, (char*) split[6]);
					return true;
				}
				else if (strcmp((char*) split[2], "\"LTE\"") == 0) {
					strcpy(MCC, (char*) split[4]);
					strcpy(MNC, (char*) split[5]);
					strcpy(LAC, (char*) split[12]);
					strcpy(Celid, (char*) split[6]);
					return true;
				}
			}
			char *ok_start = strstr(qeng_start, "OK");
			if (ok_start != NULL) {
				ok_start += strlen("OK"); // Move past "OK"
				// Shift the rest of the string to the position of qeng_start
				memmove(qeng_start, ok_start, strlen(ok_start) + 1);
			}
		}
		return false;
	}
	return false;
}
bool setupEc200(UART_HandleTypeDef *uart){
	FLAG_IS_EC200_ENABLED=true;
	ec200MqttUart=uart;
	//GPIO_3V3_ON();
	//GPS_LIGHT_ON();
	enableIdle();
	if(!EC200_Init()){
		FLAG_IS_EC200_ENABLED=false;
		return false;
	}
	gsmParameterUpdate();
	if(getIMEI(tmpbuff)){
		setIMEI(tmpbuff);
	}
	else{
		printc(MinDelay,"Don't get IMEI\n");
		FLAG_IS_EC200_ENABLED=false;
		return false;
	}
	FLAG_IS_EC200_ENABLED=true;
	return true;
}

void E200_RxHAlfCpltCallback(void){
	printc(MinDelay,"Half Callback\n %s \n",EC200_ResponseBuff);
}

void Ec200_RxEventCallback(uint16_t size){
	receivedvalue=0;
	FLAG_EC200RESONSE=true;
	receivedvalue=size;
	check_ble_response();
	check_tcp_response();
	check_sms_response();
}
void E200_RXCpltCallback(void){
	printc(MinDelay,"Complete Callback %s \n",EC200_ResponseBuff);
}
/*
 *ISSUE: call switch idle 1 cmd before only*/
void switchIdle(bool idleFlag){
	enable_Idle = idleFlag;
	if(enable_Idle){
		HAL_NVIC_EnableIRQ(USART1_IRQn);
	}
	enableIdle();
}
void enableIdle(){
	if(enable_Idle)
		HAL_UARTEx_ReceiveToIdle_IT(&huart1,EC200_ResponseBuff,RESBUFF_SIZE);
}
bool compare_ec200_response(char *buffer){
	if(strstr((char *)EC200_ResponseBuff,buffer)!=NULL){
		return true;
	}
	else
		return false;
}
