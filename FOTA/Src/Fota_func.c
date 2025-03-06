// all FoTA Functionality
/*________________Written by Manky_and optimize by shekhar__________________________*/
#include"FOTA.h"

extern char Firmware_Version[],MODEL_NAME[],Hardware_Version[],Vehicle_Reg_No[];//, Config_Version[];
uint8_t intuchrRedo = 0;
extern char updateVersion[];
extern CRC_HandleTypeDef hcrc;
extern UART_HandleTypeDef huart5;
extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart6;
FOTA_Status status;
char fotaTempBuffer[2048] = {0};
extern long int currentEpochTime, lastFotaTime;
char *Skey1, *Skey2, *Skey3;
char *Ckey1, *Ckey2, *Ckey3, *Ckey4, *Ckey5, *Ckey6, *Ckey7, *Ckey8;
extern uint64_t CP, CD, SP;
extern bool enable_Idle, updateStatus;
extern uint8_t StrtFotaTmbuf[2], getFotaTmbuf[2];
extern uint32_t L1, L2, L3;
bool Fotaflags[3] = {false};
char tmpStr[1000] = {0};
int TOTAL_PACKETS = 200;
bool setStaticKeys(char *key1, char *key2, char *key3) {
	// Now you can access the keys by index (1, 2, 3)
	Skey1 = key1; // Index 0 corresponds to the first key
	Skey2 = key2; // Index 1 corresponds to the second key
	Skey3 = key3; // Index 2 corresponds to the third key
}
bool setComKeys(char *key1, char *key2, char *key3, char *key4, char *key5,char *key6, char *key7, char *key8) {
	Ckey1 = key1;
	Ckey2 = key2;
	Ckey3 = key3;
	Ckey4 = key4;
	Ckey5 = key5;
	Ckey6 = key6;
	Ckey7 = key7;
	Ckey8 = key8;
}
bool reset_fota_temp_buff() {
	clearBuffer(fotaTempBuffer, sizeof(fotaTempBuffer));
}
// Define callback functions for each message type
bool handleKeysRequest() {
	bool status=true;
	reset_fota_temp_buff();
	sprintf(fotaTempBuffer, "{\"imei\":\"%s\"}", getConfigValue(Config_IMEI));
	printc(100, "String =%s \r\n", fotaTempBuffer);
	sKey_update(0);
	if(!handleIntouchPost(1,Skey1)){
		status = false;
	}
	if(extractKeys(fotaTempBuffer)){
		saveConfigToFlash();
	}
	return status;
}
bool handleFotaDetails() {
	bool status=true;
	reset_fota_temp_buff();
	sprintf(fotaTempBuffer,"{\"imei\":\"%s\",\"version\":\"%s\",\"configVersion\":\"%s\",\"ICCID\":\"%s\",\"hwVer\":\"%s\",\"model\":\"%s\",\"vin\":\"%s\"}",
			getConfigValue(Config_IMEI),
			&Firmware_Version,
			getConfigValue(Config_CNF_VER),
			getConfigValue(Config_ICCID),
			&Hardware_Version,
			&MODEL_NAME,
			getConfigValue(Config_VIN));
	printc(100, "String =%s \r\n", fotaTempBuffer);
	cKey_update(0);
	if(!handleIntouchPost(3,Ckey1))
		status= false;
	Extract_FOTA_status();
	return status;
}

bool handleFotaData() {
	bool status = true;
	char command[250] = {0};
	int F_Data;
	char file_name[]="FILE_EC.bin";
	char opbuffer[5][100]={};
	long int length=0;
	reset_fota_temp_buff();
	sprintf(fotaTempBuffer,"{\"version\":\"%s\"}", getUpdateVersion());
	printc(100, "String=%s\r\n", fotaTempBuffer);
	cKey_update(0);
	if(!handleIntouchPost(6,Ckey1))
		return false;
	F_Data = 0;
	F_Data = strlen(fotaTempBuffer);
	char DURL[F_Data];
	memset(DURL, 0, sizeof(DURL));
	uint8_t paddingVal = fotaTempBuffer[F_Data - 1];
	printc(1000, "Download-URL = %s \r\n", fotaTempBuffer);
	memcpy(DURL, fotaTempBuffer, F_Data - paddingVal);
	print(1000, DURL, strlen(DURL));
	memset(command, 0, 250);
	reset_fota_temp_buff();
	sprintf(command, "AT+QHTTPURL=%d,%d\r\n", strlen(DURL), 30);
	HAL_Delay(100);
	if (EC200_Transmit(command, fotaTempBuffer, 10000)) {
		printc(100, "%s \n\r", fotaTempBuffer);
	}
	clearBuffer(fotaTempBuffer, sizeof(fotaTempBuffer));
	if (EC200_Transmit(DURL, fotaTempBuffer, 20000)) {
		printc(100, "URL%s \n\r", fotaTempBuffer);
	}
	memset(command, 0, 250);
	reset_fota_temp_buff();
	if(!http_fota_get(fotaTempBuffer))
		return false;
	if(splitString(fotaTempBuffer, ",", opbuffer, 5)){
		length = atol(opbuffer[2]);
	}
	else{
		printc(100,"\nError in Get\n");
		status=false;
	}
	reset_fota_temp_buff();
	if(status&&(length>140000)){
		http_fota_read_file(fotaTempBuffer,file_name);
		if(!EC200_File_save(length,file_name)){
			status = false;
		}
	}
	file_delete(file_name);
	return status;
}

bool handleFotaACK(int fota_status){
	bool status = true;
	reset_fota_temp_buff();
	sprintf(fotaTempBuffer,"{\"imei\":\"%s\",\"version\":\"%s\",\"configVersion\":\"%s\",\"versionStatus\":%d,\"configStatus\":0}"
			,getConfigValue(Config_IMEI),&Firmware_Version, getConfigValue(Config_CNF_VER),fota_status);
	printc(2000,"String = %s\n",fotaTempBuffer);
	cKey_update(0);
	if(!handleIntouchPost(4,Ckey1))
		status = false;
	reset_fota_temp_buff();
	return status;
}

void handleVersionStatus() {
	printc(100, "Received Version Status\n");
}

void handleErrorResponse() {
	printc(100, "Received Error Response\n");
}

bool Extract_FOTA_status() {
	if (ParseJSON(fotaTempBuffer, &status) == 0) {
		// Successfully parsed JSON, print values
		printc(100, "Version: %s\n", status.version);
		printc(100, "Config Version: %s\n", status.configVersion);
		printc(100, "Update Version: %s\n", status.updateVersion);
		printc(100, "Update Config Version: %s\n",status.updateConfigVersion);
		printc(100, "Config Update Required: %d\n",status.configUpdateRequired);
		printc(100, "Version Update Required: %d\n",status.versionUpdateRequired);
		printc(100, "Error Response: %s\n", status.errorResponse);
		printc(100, "Status: %d\n", status.status);
		printc(100, "Log Type: %d\n", status.logType);
	} else {
		printc(MinDelay, "JSON Parsing Failed\n");
	}
	if (status.versionUpdateRequired != 0) {
		updateStatus = true;
		lastFotaTime = currentEpochTime;
		strcpy(updateVersion, status.updateVersion);
	} else {
		lastFotaTime = currentEpochTime;
	}
	return true;
}
void Extract_Data_from_Buffer(int Start_loc, int End_loc, char Buff[],
		char *extractedData, int *size) {
	*size = End_loc - Start_loc + 1;
	strncpy(extractedData, &Buff[Start_loc], *size);
	extractedData[*size] = '\0'; // Null-terminate the string
	//	printc(100,"Extracted data: %s\n", extractedData);
}

int Hex_Check(char Buff[], int hexValue) {
	char target_char[3]; // To hold the hexadecimal representation of hexValue
	sprintf(target_char, "%02X", hexValue); // Convert hexValue to hexadecimal string
	char *result_OK = strstr(Buff, target_char); // Search for the hexadecimal string
	if (result_OK != NULL) {
		printc("Hexadecimal value '%s' found at index %ld\n", target_char,
				result_OK - Buff);
		return result_OK - Buff; // Return index of the found hexadecimal value
	} else {
		printc(100, "Hexadecimal value '%s' not found\n", target_char);
		return -1; // Indicate that the hexadecimal value was not found
	}
}
// Function to extract keys
bool extractKeys(const char *inputString) {
	const char *start = strstr(inputString, "{\"comMap\":{"); // Find the start of the comMap section
	char CoKeys[33];
	if (start == NULL) {
		printc(100, "comMap section not found in the input string.\n");
		return false;
	}
	start += strlen("{\"comMap\":{"); // Move start pointer to the beginning of the comMap data
	// Loop to extract values
	for (int i = 0; i < 8; i++) {
		// Find the start of the value
		const char *valueStart = strstr(start, ":\""); // Find the start of the value
		if (valueStart == NULL) {
			printc(100, "Value start not found.\n");
			return;
		}
		valueStart += 2; // Move past the ":"
		// Find the end of the value
		const char *valueEnd = strstr(valueStart, "\"");
		if (valueEnd == NULL) {
			printc(100, "Value end not found.\n");
			return;
		}
		//removed ComKeys and maintaning the value in the config array
		memset(CoKeys, 0, sizeof(CoKeys));
		strncpy(CoKeys, valueStart, valueEnd - valueStart);
		CoKeys[valueEnd - valueStart] = '\0'; // Null-terminate the value
		setConfigValue(Config_CKEY1 + i, CoKeys);
		// Move start pointer to the next value
		start = valueEnd + 1;
	}
	return true;
}

bool fota_seprate_string(char Buff[],int index,char *data[], char Encpt_Data[]) {
	bool fotaSeparateStatus=true;
	char *resultMMI;
	int startLocation=0;
	if ((resultMMI = strstr(fotaTempBuffer,"$MMI"))!=NULL) {
		printc(MinDelay, "Substring '$MMI' found at index %d\n", resultMMI - fotaTempBuffer);
		startLocation = resultMMI - fotaTempBuffer;}
	else{
		printc(MinDelay,"<<<<<$MMI NOT FOUND ERROR>>>>>\n");
		return false;
	}
	char *line;
	int len = strlen(Buff) - startLocation;
	char tmpBuffer[len + 1]; // Allocate enough space for the string
	memcpy(tmpBuffer, &Buff[startLocation], len); // Copy Buff to tmpBuffer
	tmpBuffer[len] = '\0'; // Null-terminate the string
	int currentLine = 0;
	line = strtok(tmpBuffer, ",");
	while (line != NULL && currentLine <= index) {
		data[currentLine++] = line;
		if (currentLine == 5)
			memcpy(Encpt_Data, line, strlen(line));
		if (currentLine == index) {
			printc(100, "Line %d: %s\n", currentLine, line);
			break; // Stop parsing once the desired line is found
		}
		printc(100, "Line %d: %s\n", currentLine, line);
		line = strtok(NULL, ",");
	}
	if (line == NULL) {
		printc(100, "Line %d does not exist.\n", index);
	}
	if (data[5] == NULL) {
		printc(MinDelay,"<<<<<<<<<<<<<NO CHECKSUM ERROR>>>>>>>>>>>>\n");
		fotaSeparateStatus = false;
	}
	return fotaSeparateStatus;
}

uint8_t Calculate_CRC8(uint8_t *data, uint32_t size) {
	uint8_t arr[size];
	memset(arr, 0, sizeof(arr));
	memcpy(arr, data, size);
	return HAL_CRC_Calculate(&hcrc, arr, size);
}

void hexArrayToString(const unsigned char *hexArray, size_t length, char *str) {
	for (size_t i = 0; i < length; i++) {
		sprintf(str + i * 2, "%02X", hexArray[i]);
	}
	str[length * 2] = '\0'; // Null-terminate the string
}
bool handleIntouchPost(int fotaMsgType,char *keyValue){
	bool IntouchPostStatus=false;
	int fotaLength=0;

	int arrayLength = encrypt(fotaTempBuffer, tmpStr);
	reset_fota_temp_buff();
	for(int i = 0; i < arrayLength; i++) {
		sprintf(fotaTempBuffer + (i * 2), "%02X", tmpStr[i]);
	}
	memset(tmpStr,0,sizeof(tmpStr));
	uint8_t checksum = Calculate_CRC8((uint8_t*) fotaTempBuffer,strlen(fotaTempBuffer));
	snprintf((char*)tmpStr,sizeof(tmpStr),"%s,%d,%s,%d,%s,%02X%c\0",header, fotaMsgType, getConfigValue(Config_IMEI), 1, fotaTempBuffer, checksum, Delimiter);
	reset_fota_temp_buff();
	tmpStr[strlen(tmpStr)] = '\0';
	if(http_post_data(tmpStr, fotaTempBuffer,keyValue,&fotaLength)){
		reset_fota_temp_buff();
		if(http_post_read_data(fotaTempBuffer,fotaLength))
		{
			memset(tmpStr,0,sizeof(tmpStr));
			char reCheckSum[3] = {0};
			const char *extractedData[6] = {0};
			if(fota_seprate_string(fotaTempBuffer,7, extractedData, tmpStr)){
				memcpy(reCheckSum, extractedData[5],2);
				reCheckSum[2] = '\0';
				int KeyID = atoi(extractedData[3]);
				uint8_t CSUM = Calculate_CRC8(tmpStr, strlen(tmpStr));
				char Cksum[3] = {0};
				sprintf(Cksum,"%02X",CSUM); // Ensure the checksum is always two characters, including leading zeros
				reset_fota_temp_buff();
				if(memcmp(reCheckSum, Cksum, 2) == 0) { // Use memcmp for binary comparison
					Decrypt(tmpStr, fotaTempBuffer, KeyID, fotaMsgType);
					IntouchPostStatus=true;
					printc(2000,"RESPONSE:%s\n",fotaTempBuffer);
				}
			}
		}
	}
	else{
		set_http_SSL_Status(false);
	}
	return IntouchPostStatus;
}
bool check_fota_retry(){//need to check if 1 fail then what
	if(Fotaflags[PACKET_ONE_FAIL] && EC200_HTTP_POST(1)){
		Fotaflags[PACKET_ONE_FAIL]=false;
		Fotaflags[PACKET_THREE_FAIL]=true;
	}
	if(Fotaflags[PACKET_THREE_FAIL] && EC200_HTTP_POST(3)){
		Fotaflags[PACKET_ONE_FAIL]=false;
		Fotaflags[PACKET_THREE_FAIL]=false;
	}
	else
		Fotaflags[PACKET_ONE_FAIL]=true;

	return Fotaflags[PACKET_THREE_FAIL]||Fotaflags[PACKET_ONE_FAIL];
}
bool EC200_HTTP_POST(int type) {
	switch (type) {
	case 1:
		if(handleKeysRequest()){
			Fotaflags[PACKET_ONE_FAIL] =false;
			return true;
		}
		else{
			Fotaflags[PACKET_ONE_FAIL] =true;
			return false;
		}
		break;
	case 2:
		//handleKeysUpdate();
		break;
	case 3:
		if(handleFotaDetails()){
			Fotaflags[PACKET_ONE_FAIL] =false;
			Fotaflags[PACKET_THREE_FAIL] =false;
			return true;
		}
		else{
			Fotaflags[PACKET_THREE_FAIL] =true;
			return false;
		}
		break;
	case 4:
		handleFotaACK(12);
		break;
	case 5:
		//		handleCotaData();
		break;
	case 6:
		if(!handleFotaData()){
			if(!handleFotaData())
				return false;
			else
				return true;
		}
		else
			return true;
		break;
	case 9:
		handleErrorResponse();
		break;
	default:
		printc(100, "Invalid Message Type\n");
		break;
	}
	return true;
}

bool EC200_File_save(long int length,char *fotaFileName) {
	bool fotaDownloadStatus=true;
	uint8_t rx_Code[1024]={0};
	uint8_t rx_Code_buffer[1090]={0};
	int ec_packet_size = 1024;
	TOTAL_PACKETS= (length/ec_packet_size)+1;
	uint8_t packet_index = 0;
	uint32_t Addr = 0;
	char snum[100];
	gnss_data_off();

	Addr = L3;
	Erase_sectors(L3);
	int reclength=4;
	int16_t fileHandle=0;
	if(fileHandle=file_open(fotaFileName,2)){
		switchIdle(false);
		file_seek(fileHandle);
		for(packet_index = 0; packet_index < TOTAL_PACKETS; packet_index++) {
			if (packet_index == TOTAL_PACKETS-1 ) {
				ec_packet_size = length % ec_packet_size;
				sprintf(snum, "%d", ec_packet_size);
				reclength = strlen(snum);
			}
			memset(rx_Code,0,sizeof(rx_Code));
			if(!file_read_non_it(fileHandle, ec_packet_size,rx_Code_buffer)){
				fotaDownloadStatus = false;
				break;
			}
			memcpy(rx_Code,&rx_Code_buffer[12+reclength],1024);
			clearBuffer(rx_Code_buffer, sizeof(rx_Code_buffer));
			if (!HAL_FLASH_Write_Buff(Addr, &rx_Code, ec_packet_size)){
//				printc(100, "\nflash write fail\n");
				fotaDownloadStatus = false;
				break;
			}
			Addr += 1024;
		}
	}
	file_close(fileHandle);
	if(fotaDownloadStatus){
		writeF_CP((uint64_t) 0, (uint64_t) 1, (uint64_t) 1);
		CP = 0;
		CD = 1;
		SP = 1;
	}
	else{
		writeF_CP((uint64_t) 0, (uint64_t) 0, (uint64_t) 0);
		CP=0;
		CD=0;
		SP=0;
	}
	switchIdle(true);
	lastFotaTime = currentEpochTime;
	updateStatus = false;
	saveConfigToFlash();
	gnss_data_on();

	return fotaDownloadStatus;
}
// Function to parse JSON string into FOTA_Status structure
int ParseJSON(const char *jsonString, FOTA_Status *status) {
	cJSON *json = cJSON_Parse(jsonString);
	if (json == NULL) {
		return -1;
	}
	cJSON *version = cJSON_GetObjectItemCaseSensitive(json, "version");
	if (cJSON_IsString(version) && (version->valuestring != NULL)) {
		strcpy(status->version, version->valuestring);
	}
	cJSON *configVersion = cJSON_GetObjectItemCaseSensitive(json,
			"configVersion");
	if (cJSON_IsString(configVersion) && (configVersion->valuestring != NULL)) {
		strcpy(status->configVersion, configVersion->valuestring);
	}
	cJSON *updateVersion = cJSON_GetObjectItemCaseSensitive(json,
			"updateVersion");
	if (cJSON_IsString(updateVersion) && (updateVersion->valuestring != NULL)) {
		strcpy(status->updateVersion, updateVersion->valuestring);
	}
	cJSON *updateConfigVersion = cJSON_GetObjectItemCaseSensitive(json,
			"updateConfigVersion");
	if (cJSON_IsString(updateConfigVersion)
			&& (updateConfigVersion->valuestring != NULL)) {
		strcpy(status->updateConfigVersion, updateConfigVersion->valuestring);
	}
	cJSON *configUpdateRequired = cJSON_GetObjectItemCaseSensitive(json,
			"configUpdateRequired");
	if (cJSON_IsNumber(configUpdateRequired)) {
		status->configUpdateRequired = configUpdateRequired->valueint;
	}
	cJSON *versionUpdateRequired = cJSON_GetObjectItemCaseSensitive(json,
			"versionUpdateRequired");
	if (cJSON_IsNumber(versionUpdateRequired)) {
		status->versionUpdateRequired = versionUpdateRequired->valueint;
	}
	cJSON *errorResponse = cJSON_GetObjectItemCaseSensitive(json,
			"errorResponse");
	if (cJSON_IsString(errorResponse) && (errorResponse->valuestring != NULL)) {
		strcpy(status->errorResponse, errorResponse->valuestring);
	}
	cJSON *statusItem = cJSON_GetObjectItemCaseSensitive(json, "status");
	if (cJSON_IsNumber(statusItem)) {
		status->status = statusItem->valueint;
	}
	cJSON *logType = cJSON_GetObjectItemCaseSensitive(json, "logType");
	if (cJSON_IsNumber(logType)) {
		status->logType = logType->valueint;
	}
	cJSON_Delete(json);
	return 0;
}
// Function to Check the Response Of the At commands
//int Response_Check(char Buff[]) {
//	char *result_OK = strstr(Buff, "OK");
//	char *result_error = strstr(Buff, "\"errorResponse\":\"succesful\"");
//	char *Result_MMI = strstr(Buff, "$MMI");
//	char *Result_CkeyError = strstr(Buff, "\"status\":102");
//	char *Result_ChecksumError = strstr(Buff, "\"status\":107");
//	char *Result_DecryptError = strstr(Buff, "\"status\":104");
//
//	if (Result_MMI != NULL) {
//		printc(100, "Substring '$MMI' found at index %d\n", Result_MMI - Buff);
//		return Result_MMI - Buff; // Return index of 'errorResponse':'succesful'
//	} else if (result_OK != NULL) {
//		printc(100, "Substring 'OK' found at index %d\n", result_OK - Buff);
//		return result_OK - Buff; // Return index of 'OK'
//	} else if (result_error != NULL) {
//		printc(100, "Substring 'errorResponse':'succesful' found at index %d\n",
//				result_error - Buff);
//		return -1; // Return index of 'errorResponse':'succesful'
//	} else if (Result_CkeyError != NULL) {
//		printc(100, "Substring 'errorStatus found  %d\n",
//				Result_CkeyError - Buff);
//		return 102; // Return index of 'errorResponse':'succesful'
//	} else if (Result_ChecksumError != NULL) {
//		printc(100, "Substring 'errorStatus found  %d\n",
//				Result_ChecksumError - Buff);
//		return 107; // Return index of 'errorResponse':'succesful'
//	} else if (Result_DecryptError != NULL) {
//		printc(100, "Substring 'errorStatus found  %d\n",
//				Result_DecryptError - Buff);
//		return 104; // Return index of 'errorResponse':'succesful'
//	} else {
//		printc(100, "Neither substring found\n");
//		return -1; // Indicate that neither substring was found
//	}
//	return -1;
//}

//	uint8_t str[300] = { 0 };
//	char str1[1000] = { 0 };
//	char tmpStr[1000]={0};
//	int array_length = encrypt(dataBody, str);
//	printc(100, "array length of encrepted data - %d \n\r", array_length);
//	char hex_string[array_length * 2 + 1];
//
//	for (int i = 0; i < array_length; i++) {
//		sprintf(hex_string + (i * 2), "%02X", str[i]);
//	}
//	hex_string[array_length * 2] = '\0';
//	reset_fota_temp_buff();
//		http_post_data(tmpStr, fotaTempBuffer);
//		HAL_Delay(2000);
//		reset_fota_temp_buff();
//		http_post_read_data(fotaTempBuffer,60);
//		clearBuffer(tmpStr, sizeof(tmpStr));
//}
//strlen doesnt memset the full array
//	memset(dataBody, 0, strlen(dataBody));
//	memcpy(dataBody, hex_string, strlen(hex_string));
//	memset(hex_string, 0, sizeof(hex_string));
//
//	//Calculate the checksum
//	uint8_t checksum = Calculate_CRC8((uint8_t*) dataBody, strlen(dataBody));
//	printc(MinDelay,"checksum is %0X\n",checksum);
//	memset(str1, 0, sizeof(str1));
//
//	//Construct the string
//	snprintf((char*) str1, sizeof(str1), "hexKey=%s%s,%d,%s,%d,%s,%02X%c",keyValue,header, fotaMsgType, &IMEI, 1, dataBody, checksum, Delimiter);
//	printc(100, "Command -  %s \n\r", str1);
//	printc(100, "databody -  %s \n\r", dataBody);
//	memset(dataBody, 0, strlen(dataBody));
//
//	//Create the AT command string with the length of dataBody inserted
//	sprintf(command, "AT+QHTTPURL=%d,%d\r\n", strlen(URL), EC_TOUT);
//	reset_keyresBuffer();
//	if (EC200_Transmit(command, key_resBuffer, EC200MINRESONSETIME)) {
//		printc(100, "AT+QHTTPURL -  %s \n\r", key_resBuffer);
//	}
//	memset(command, 0, 250);
//
//	// Send HTTP URL
//	reset_keyresBuffer();
//	if (EC200_Transmit(URL, key_resBuffer, EC200MINRESONSETIME)) {
//		printc(100, "URL -  %s \n\r", key_resBuffer);
//	}
//	int F_Data = strlen(str1);
//
//	// Construct the AT command for HTTP POST
//	sprintf(command,"AT+QHTTPPOST=%d\r\n", F_Data);
//	// Transmit the command string
//	//	EC200_Data_Fetch(command);
//	reset_keyresBuffer();
//	if (EC200_Transmit(command, key_resBuffer, 10000)) {
//		printc(100, "AT+QHTTPPOST -  %s \n\r", key_resBuffer);
//	}
//	memset(command, 0, sizeof(command));
//	// Transmit the dataBody
//	if (EC200_Transmit(str1, key_resBuffer, EC200MINRESONSETIME)) {
//		printc(100, "AT+QHTTPPOST -  %s \n\r", key_resBuffer);
//	}
//	memset(str1, 0, sizeof(str1));
//	HAL_Delay(3000);
//	sprintf(command, "AT+QHTTPREAD=%d\r\n", EC_TOUT);
//	printc(100, "keyres_buffer -  %s \n\r", key_resBuffer);
//}

//void Seprate_String(char Buff[], int Start_Location, int index, char *data[]) {
//	char *line;
//	int len = strlen(Buff) - Start_Location;
//	char tmpBuffer[len + 1]; // Allocate enough space for the string
//
//	memcpy(tmpBuffer, &Buff[Start_Location], len); // Copy Buff to tmpBuffer
//	tmpBuffer[len] = '\0'; // Null-terminate the string
//
//	int currentLine = 0;
//
//	line = strtok(tmpBuffer, ",");
//	while (line != NULL && currentLine <= index) {
//		data[currentLine++] = line;
//
//		if (currentLine == index) {
//			printc(100, "Line %d: %s\n", currentLine, line);
//			break; // Stop parsing once the desired line is found
//		}
//		printc(100, "Line %d: %s\n", currentLine, line);
//		line = strtok(NULL, ",");
//	}
//
//	if (line == NULL) {
//		printc(100, "Line %d does not exist.\n", index);
//	}
//}
//void handleCotaData() {
//	char CVer[30] = { "Test_1" }; // Fetched FRom Flash
//	reset_fota_temp_buff();
//	sprintf(fotaTempBuffer, "{\"imei\":\"%s\",\"configVersion\":\"%s\"}", &IMEI,&CVer, sizeof(fotaTempBuffer));
//	printc(100, "String =%s \r\n", fotaTempBuffer);
//	cKey_update(0);
//	if(!handleIntouchPost(5,Ckey1))
//		return false;
//	reset_fota_temp_buff();
//	//	EC200_Config_Update(command);
//}
