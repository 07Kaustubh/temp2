/*
 * BLE.c
 * Created on: Sep 9, 2024
 * Author: Shekhar Verma
 */
/*
 * BLE THINGS WE SHOULD KNOW
 * init -> add service -> add characteristic & descriptor -> save service -> advertise BLE ->
 */
#include "BLE.h"

static char bleBuff[2048]={0};
extern bool FLAG_IS_EC200_ENABLED;
extern TIM_HandleTypeDef htim6;
bool TIMER_DIABLE_FLAG=false;
CharacteristicConfig CharacteristicParamenter = DisableConfig;
extern int blesleepcounter;
bool setBleRead = false;
bool setBleFlag = false;

void set_ble_read() {
	setBleRead = true;
}
bool get_ble_read() {
	return setBleRead;
}
void set_ble_status(bool setBLE){
	setBleFlag=setBLE;
}
bool get_ble_status(){
	return setBleFlag;
}
bool ble_init() {
	if(!ble_switch(START)){
		ble_switch(STOP);
		ble_switch(START);
	}
	set_ec200_response_check("OK");
	if(!EC200_Transmit("AT+QBTGATADV=1,128,160,0,0,7,0\r\n", bleBuff, EC200MAXRESONSETIME)) {
		printc(MinDelay, "Set advertising parameters%s\n",bleBuff);
		return false;
	}
	memset(bleBuff, 0, sizeof(bleBuff));
	set_ec200_response_check("OK");
	if(!EC200_Transmit("AT+QBTADVDATA=9,\"020106050938393130\"\r\n",bleBuff, EC200MAXRESONSETIME)) {
		printc(MinDelay, "Set 9-byte advertising data%s\n",bleBuff);
		return false;
	}
	memset(bleBuff, 0, sizeof(bleBuff));
	set_ec200_response_check("OK");
	if(!EC200_Transmit("AT+QBTADVRSPDATA=6,\"050938393130\"\r\n", bleBuff, EC200MAXRESONSETIME)) {
		printc(MinDelay, "Set 6-byte scan response data%s\n",bleBuff);
		return false;
	}
	ble_add_service(START);
	ble_configure_characteristic_descriptors();
	ble_service_save();
	ble_advertise_switch(START);
	memset(bleBuff, 0, sizeof(bleBuff));
	set_ec200_response_check("OK");
	if(!EC200_Transmit("AT+QBTLERCVM=1,10000\r\n", bleBuff, EC200MINRESONSETIME)) {
		return false;
	}
	printc(MinDelay,"%s\n",bleBuff);
	return true;
}
void ble_sleep_init(){
	GPIO_LTE_ON();
	HAL_Delay(3000);
	HAL_NVIC_EnableIRQ(USART1_IRQn);
	HAL_NVIC_SetPriority(USART1_IRQn, 0, 0);
	enableIdle();
	TIMER_DIABLE_FLAG=true;
	FLAG_IS_EC200_ENABLED=true;
	HAL_TIM_Base_Start_IT(&htim6);

	EC200_Transmit("AT\r\n",bleBuff,EC200MINRESONSETIME);
	memset(bleBuff,0,100);
	EC200_Transmit("AT+QURCCFG=\"urcport\",\"uart1\"\r\n",bleBuff,EC200MINRESONSETIME);
	memset(bleBuff,0,100);
	EC200_Transmit("ATV1\r\n",bleBuff,EC200MINRESONSETIME);
	memset(bleBuff,0,100);
	EC200_Transmit("ATE0\r\n",bleBuff,EC200MINRESONSETIME);
	memset(bleBuff,0,100);
	EC200_Transmit("AT+CMEE=2\r\n",bleBuff,EC200MINRESONSETIME);
	memset(bleBuff,0,100);
	ble_init();
	if (!EC200_Transmit("AT+QCFG=\"urc/ri/other\",\"pulse\",120,1\r\n",bleBuff, EC200MINRESONSETIME)) {
	}
	FLAG_IS_EC200_ENABLED=false;
	HAL_TIM_Base_Stop_IT(&htim6);
	TIMER_DIABLE_FLAG=false;
}
bool ble_service_save(){
	set_ec200_response_check("OK");
	if (!EC200_Transmit("AT+QBTGATSSC=1,1\r\n", bleBuff, EC200MINRESONSETIME)) {
		printc(MinDelay, "Finish adding service, and reserve the default GAP and GATT services%s\n",bleBuff);
		return false;
	}
	return true;
}
bool ble_advertise_switch(int start){
	char command[50]={0};
	sprintf(command,"AT+QBTADV=%d\r\n",start);
	set_ec200_response_check("OK");
	if (!EC200_Transmit(command, bleBuff, EC200MINRESONSETIME)) {
		printc(MinDelay, "Start advertising, waiting for the BLE GATT client to connect%s\n",bleBuff);
		return false;
	}
	return true;
}
bool ble_switch(int on_off){
	char command[100]={0};
	sprintf(command,"AT+QBTPWR=%d\r\n",on_off);
	set_ec200_response_check("OK");
	if (!EC200_Transmit(command, bleBuff, 2000)) {
		printc(100, "Turn BLE GATT server on%s\n",bleBuff);
		return false;
	}
	return true;
}

bool ble_add_service(int serviceId){
	char command[100]={0};
	sprintf(command,"AT+QBTGATSS=%d,0,\"f5899b5f8000008000100000FE180000\",1\r\n",serviceId);
	set_ec200_response_check("OK");
	if (!EC200_Transmit(command,bleBuff, EC200MAXRESONSETIME)) {
		printc(MinDelay, "Add a service%s\n",bleBuff);
		return false;
	}
	return true;
}

bool ble_configure_characteristic_descriptors(){
	char command[100]={0};
	set_ec200_response_check("OK");
	sprintf(command,"AT+QBTGATSC=1,0,%d,0,\"f5899b5f8000008000100000FEFF1111\"\r\n",readWriteNotiIndi);
	if (!EC200_Transmit(command, bleBuff, EC200MAXRESONSETIME)) {
		printc(MinDelay, "Add the first characteristic\n properties are Read, Write, Notify and Indicate\n");
		return false;
	}
	memset(bleBuff, 0, sizeof(bleBuff));
	memset(command, 0, sizeof(command));
	set_ec200_response_check("OK");
	sprintf(command,"AT+QBTGATSCV=1,0,%d,0,\"f5899b5f8000008000100000FEFF1111\",244,\"1234\"\r\n",readWriteNotiIndi);
	if (!EC200_Transmit(command, bleBuff, EC200MAXRESONSETIME)) {
		printc(MinDelay, "Configure characteristic value\n");
		return false;
	}
	memset(command, 0, sizeof(command));
	memset(bleBuff, 0, sizeof(bleBuff));
	set_ec200_response_check("OK");
	sprintf(command,"AT+QBTGATSCD=1,0,%d,1,10498,2,\"0300\"\r\n",readWriteNotiIndi);
	if (!EC200_Transmit(command, bleBuff, EC200MAXRESONSETIME)) {
		printc(MinDelay, "Add 2-byte descriptor with value\n");
		return false;
	}
	return true;
}
bool ble_read(uint8_t bleLength){
	setBleRead=false;
	char command[100]={0};
	uint8_t bleSplit[5][100]={0};
	uint8_t resbuff[1024] = { 0};
	memset(bleBuff, 0, sizeof(bleBuff));
	set_ec200_response_check("OK");
	sprintf(command,"AT+QBTLEREAD=0,%d\r\n",bleLength);
	printc(MinDelay,"%s\n",command);
	if (EC200_Transmit(command, bleBuff, EC200MAXRESONSETIME)) {
		splitString(bleBuff, ",",bleSplit,5);
		if (bleSplit[0][0] != '\0') {
			extractBetweenQuotes(bleSplit[2],resbuff);
			hexToString(resbuff,bleBuff);
			memset(resbuff,0,sizeof(resbuff));
			parseData(bleBuff,resbuff);
//			memset(bleBuff, 0,sizeof(bleBuff));
			hexArrayToString(resbuff, strlen(resbuff), bleBuff);
			ble_write(bleBuff);
		}
	}
	return true;
}
void asciiToHex(const char *ascii, char *hex) {
	while (*ascii) {
		sprintf(hex, "%02X", (unsigned char)*ascii);
		ascii++;
		hex += 2;
	}
	*hex = '\0'; // Null-terminate the string
}
void check_ble_response(){
	if(compare_ec200_response("QBTLEVALDATI")){
		set_ble_read();
	}
}
bool ble_write(char*sendData){
	char command[2098]={0};
	set_ec200_response_check("OK");
	sprintf(command,"AT+QBTLESEND=0,1,%d,\"%s\"\r\n",(strlen(sendData))/2,sendData);
	if (!EC200_Transmit(command, bleBuff, EC200MAXRESONSETIME)) {
		printc(MinDelay,"Send ble not working\n");
		return false;
	}
	return true;
}
void parse_ble_message(){
	blesleepcounter=0;
	uint8_t bleSplit[5][100]={0};
	uint8_t resbuff[2048] = { 0}; // Increased buffer size to handle the response
	memcpy(resbuff,bleBuff,sizeof(bleBuff));
	clearBuffer(bleBuff, sizeof(bleBuff));
	splitString(resbuff, ",",bleSplit,5);
	if (bleSplit[0][0] != '\0') {
		uint8_t length = atoi(bleSplit[2]);
		ble_read(length);
	}
}

void extractBetweenQuotes(const char *input, char *output) {
	const char *start = NULL, *end = NULL;
	// Find the first opening quote
	while (*input) {
		if (*input == '"') {
			start = ++input; // Move past the opening quote
			break;
		}
		input++;
	}
	// Find the corresponding closing quote
	if (start) {
		end = start;
		while (*end) {
			if (*end == '"') {
				break; // Found closing quote
			}
			end++;
		}
	}
	// Extract the content if valid quotes were found
	if (start && end && end > start) {
		while (start < end) {
			*output++ = *start++;
		}
	}
	*output = '\0'; // Null-terminate the output
}
void hexToString(const char *hex, char *output) {
	size_t len = strlen(hex);
	size_t j = 0;

	for (size_t i = 0; i < len; i += 2) {
		char byteString[3] = {hex[i], hex[i + 1], '\0'}; // Take two hex digits
		uint8_t byte = (uint8_t)strtol(byteString, NULL, 16); // Convert to byte
		output[j++] = (char)byte; // Append to output string
	}
	output[j] = '\0'; // Null-terminate the string
}
//void parse_ble_message(void) {
//    uint8_t resBuffer[1024] = {0}; // Buffer for intermediate data
//    uint8_t parsedData[1024] = {0};
//
//    // Extract the second field (length)
//    const char *start = strstr((char *)bleBuff, ",");
//    if (!start) return; // No delimiter found, exit early
//    start++; // Move to the next character after the comma
//
//    const char *end = strstr(start, ",");
//    if (!end) return; // No second delimiter found, exit early
//
//    size_t fieldLength = end - start;
//    if (fieldLength > 3) return; // Sanity check for field length (max 3 chars for length)
//
//    char lengthField[4] = {0}; // Temporary buffer to extract the length field
//    strncpy(lengthField, start, fieldLength);
//    uint8_t bleLength = (uint8_t)atoi(lengthField); // Convert to integer
//
//    // Construct BLE Read Command
//    char command[50] = {0};
//    snprintf(command, sizeof(command), "AT+QBTLEREAD=0,%d\r\n", bleLength);
//
//    set_ec200_response_check("OK");
//    if (!EC200_Transmit(command, bleBuff, EC200MAXRESONSETIME)) {
//        printc(MinDelay, "BLE Read Command Failed\n");
//        return;
//    }
//
//    // Extract Data Field (Inside Quotes)
//    start = strchr((char *)bleBuff, '"');
//    if (!start) return; // No opening quote found
//    end = strchr(start + 1, '"');
//    if (!end) return; // No closing quote found
//
//    size_t dataLength = end - start - 1;
//    if (dataLength >= sizeof(resBuffer)) {
//        printc(MinDelay, "Response Too Large\n");
//        return;
//    }
//    strncpy((char *)resBuffer, start + 1, dataLength);
//    resBuffer[dataLength] = '\0'; // Null-terminate extracted data
//
//    // Convert HEX to String
//    for (size_t i = 0, j = 0; i < dataLength; i += 2, j++) {
//        char byteStr[3] = {resBuffer[i], resBuffer[i + 1], '\0'};
//        parsedData[j] = (uint8_t)strtol(byteStr, NULL, 16);
//    }
//
//    // Construct BLE Write Command
//    snprintf(command, sizeof(command), "AT+QBTLESEND=0,1,%ld,\"%s\"\r\n", strlen((char *)parsedData) / 2, parsedData);
//
//    set_ec200_response_check("OK");
//    if (!EC200_Transmit(command, bleBuff, EC200MAXRESONSETIME)) {
//        printc(MinDelay, "BLE Write Command Failed\n");
//        return;
//    }
//}
