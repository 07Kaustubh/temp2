/*
 * LC86.c
 *
 *  Created on: Apr 9, 2024
 *      Author: Nk
 */
#include"LC86.h"
UART_HandleTypeDef *L86_uart;
extern LedHendler ledHendler;
uint8_t L86Buffer[L86_BUFFER_SIZE] = { };
uint8_t tmpBuffer[200] = { };
char tmpData[L86_BUFFER_SIZE / 2] = { '\0' };
bool hasHalfData = false;
struct GPSData *GPSDataTmp = NULL;
bool isTimeSet = false;
bool isDateSet = false;
extern bool PowerOnFlag, powerOnCount;
char hourStr[3], minuteStr[3], secondStr[3];
uint8_t data[200] = { };
void reset_l86_time(){
	isTimeSet=false;
	isDateSet=false;
}
void L86_setup(UART_HandleTypeDef *uart) {
	L86_uart = uart;
	//#ifndef V2
	//	GPIO_3V3_ON();
	//#endif
	//POWER_3V3_ON();
	GPS_LIGHT_ON();
	GPIO_3V3_ON();
	L86_uart_Init();
}
void gnss_data_off(){

	uint8_t lcoff[] = "$PAIR003*39\r\n";
	HAL_UART_Transmit(L86_uart,(uint8_t*) lcoff, strlen(lcoff), 1000);

}
void gnss_data_on(){

	uint8_t lcON[] = "$PAIR002*38\r\n";
	HAL_UART_Transmit(L86_uart,(uint8_t*) lcON, strlen(lcON), 1000);

}

void gnss_data(char*l89_buffer){
	clearBuffer(data, sizeof(data));
	HAL_UART_Transmit(L86_uart,(uint8_t*) l89_buffer, strlen(l89_buffer), 1000);
	HAL_UART_Receive(L86_uart, data, 50, 500);
	printc(100, data, 200);
	printc(MinDelay, "\n");
}

void L86_uart_Init(void) {
	HAL_Delay(1000);
	HAL_UART_Receive(L86_uart, data, 50, 1000);

	gnss_data("$PAIR003*39\r\n");
	gnss_data("$PAIR004*3E\r\n");
	gnss_data("$PAIR070,2*23\r\n");
	gnss_data("$PAIR062,0,2*3C\r\n");
	gnss_data("$PAIR062,1,0*3F\r\n");
	gnss_data("$PAIR062,2,2*3E\r\n");
	gnss_data("$PAIR062,4,2*38\r\n");
	gnss_data("$$PAIR062,5,0*3B\r\n");
	gnss_data("$PAIR513*3D\r\n");
	gnss_data("$PAIR002*38\r\n");
	HAL_UART_Receive_DMA(L86_uart, (uint8_t*) L86Buffer, L86_BUFFER_SIZE);


}
int splitWithComma(char *data, char **output) {
	char *token = strtok((char*) data, ",");
	int i = 0;
	while (token != NULL && i < 20) {
		output[i++] = token;
		token = strtok(NULL, ",");
	}
	return i;
}
double convertToDegrees(double input) {
	int degrees = input / 100; // Extracting degrees
	double minutes = fmod(input, 100); // Extracting decimal minutes
	double decimalDegrees = degrees + (minutes / 60); // Converting to decimal degrees
	return decimalDegrees;
}
bool init_GPSData(GPSData *GPSDataTmp) {
	//    GPSData* GPSDataTmp = (GPSData*)malloc(sizeof(GPSData));
	if (GPSDataTmp == NULL) {
		// Handle memory allocation failure
		return false;
	}

	// Initialize members with default values
	GPSDataTmp->latitude = 0.0f;
	GPSDataTmp->lat_dir = 'N'; // Null terminator
	GPSDataTmp->longitude = 0.0f;
	GPSDataTmp->lon_dir = 'E'; // Null terminator
	GPSDataTmp->speed = 0.0f;
	GPSDataTmp->heading = 0;
	GPSDataTmp->sat_num = 0;
	GPSDataTmp->alt = 0;
	GPSDataTmp->PDOP = 0.0f;
	GPSDataTmp->HDOP = 0.0f;
	GPSDataTmp->gps_fix = false;
	GPSDataTmp->nextData = NULL;
	// Null terminator for date string
	return true;
}
void ProcessData(uint8_t *data) {
	int tmplen = 0;
	//		char *line = strtok((char *)data,"\n");
	//	printc(MinDelay, "Data size is %d\n", strlen(tmpData));
	uint8_t *line = strstr(data, "\r\n");
	char lineBuff[20][255] = { };
	int i = 0;
	while (line != NULL) {
		uint8_t length = line - data;
		char substring[length];
		memset(substring, 0, length);
		if (substring == NULL) {
			printc(MinDelay, "Memory allocation failed\n");
			return;
		}
		memcpy(substring, data, length);
		data = line + 2;
		tmplen += length + 2;
		//		printc(MinDelay, "sub string \n %s\n", substring);
		if (hasHalfData) {
			size_t total_length = strlen(tmpBuffer) + strlen(substring) + 1;
			char tmp[total_length];
			memset(tmp, 0, total_length);
			if (tmp != NULL) {
				//				 Copy tmpBuffer and line into tmp
				sprintf(tmp, "%s%s", tmpBuffer, substring);
				strcpy(&lineBuff[i++], tmp);
				//				printc(MinDelay, "Merged data is %s\n", tmp);
				// Free the memory allocated for tmp
				memset(tmpBuffer, 0, 200);
				hasHalfData = false;
			} else {
				// Handle memory allocation failure
				printf("Failed to allocate memory for tmp.\n");
			}
		} else {
			strcpy(&lineBuff[i++], substring);
		}
		line = strstr(data, "\r\n");
		if (line == NULL) {
			//printc(MinDelay, "last line %s\n", data);
			if (tmplen >= L86_BUFFER_SIZE / 2) {

			} else {
				sprintf(tmpBuffer, "%s", data);
				hasHalfData = true;
				tmplen += strlen(data);
			}

		}
	}
	printc(MinDelay, "Length %d \n", tmplen);
	if (tmplen < L86_BUFFER_SIZE / 2) {
		//		printc(MinDelay, "exit because Not able to parce all data  %d \n",
		//				tmplen);
		return;
	}
	for (int x = 0; x < i; x++) {
		char *values[20] = { };
		uint8_t size = 0;
		if (strncmp(lineBuff[x], "$GNRMC", 6) == 0) {
			if (GPSDataTmp != NULL) {
				GPSDataCallback(GPSDataTmp);
				free(GPSDataTmp);
				GPSDataTmp = NULL;
			}
			GPSDataTmp = malloc(sizeof(GPSData));
			if (!init_GPSData(GPSDataTmp)) {
				return;
			}

			//printc(MinDelay, "$GNRMC is %s\n", lineBuff[x]);
			int len = splitWithComma(lineBuff[x], values);

			if (values[1] == NULL)
				continue;

			if (values[2] == NULL)
				continue;
			if (values[2][0] == 'A') {
				GPSDataTmp->gps_fix = true;
				ledHendler.gpsLedState=HEIGH;
				if (!isTimeSet) {
					strncpy(hourStr, values[1], 2);
					hourStr[2] = '\0';
					strncpy(minuteStr, values[1] + 2, 2);
					minuteStr[2] = '\0';
					strncpy(secondStr, values[1] + 4, 2);
					secondStr[2] = '\0';
					if (setTime(atoi(hourStr), atoi(minuteStr),atoi(secondStr)))
						isTimeSet = true;
					//printc(MinDelay, "$UTC Time %s \n", values[1]);
				}
			} else {
				GPSDataTmp->gps_fix = false;
				ledHendler.gpsLedState=LOW;
			}
			if (values[3] == NULL)
				continue;
			GPSDataTmp->latitude = convertToDegrees(atof(values[3]));
			//printc(MinDelay, "$Latitude %s \n", values[3]);
			if (values[4] == NULL)
				continue;
			GPSDataTmp->lat_dir = *values[4];
			//printc(MinDelay, "$lat_Dir%s \n", values[4]);
			if (values[5] == NULL)
				continue;
			GPSDataTmp->longitude = convertToDegrees(atof(values[5]));
			//printc(MinDelay, "$Longitude %s \n", values[5]);
			if (values[6] == NULL)
				continue;
			if(GPSDataTmp->gps_fix == false){
				GPSDataTmp->lon_dir = 'E';
			}
			else{
				GPSDataTmp->lon_dir = *values[6];
			}
			//printc(MinDelay, "$log_Dir %s \n", values[6]);
			if (values[7] == NULL)
				continue;
			GPSDataTmp->speed = (atof(values[7])) * 1.852;
			//printc(MinDelay, "$Speed %s \n", values[7]);
			if (values[8] == NULL)
				continue;
			GPSDataTmp->heading = (int) atof(values[8]);
			//printc(MinDelay, "Heading %s\n", values[8]);
			if (values[9] == NULL)
				continue;
			//				  setting up date
			if (!isDateSet) {
				clearBuffer(hourStr, sizeof(hourStr));
				clearBuffer(minuteStr, sizeof(minuteStr));
				clearBuffer(secondStr, sizeof(secondStr));
				strncpy(hourStr, values[9], 2);
				hourStr[2] = '\0';
				strncpy(minuteStr, values[9] + 2, 2);
				minuteStr[2] = '\0';
				strncpy(secondStr, values[9] + 4, 2);
				secondStr[2] = '\0';
				if (setDate(atoi(hourStr), atoi(minuteStr), atoi(secondStr))){
					isDateSet = true;
				}
			}
		} else if (strncmp(lineBuff[x], "$GNGGA", 6) == 0) {
			if (GPSDataTmp == NULL) {
				continue;
			}
			//printc(MinDelay, "$GNGGA is %s\n", lineBuff[x]);
			splitWithComma(lineBuff[x], values);
			if (values[7] == NULL)
				continue;
			GPSDataTmp->sat_num = atoi(values[7]);
			//printc(MinDelay, "$sat_num%s \n", values[7]);
			if (values[9] != NULL) {
				GPSDataTmp->alt = atof(values[9]);
				//printc(MinDelay, "alt %s \n", values[9]);
			}
		} else if (strncmp(lineBuff[x], "$GNGSA", 6) == 0) {
			if (GPSDataTmp == NULL) {
				continue;
			}
			//printc(MinDelay, "$GNGSA is %s\n", lineBuff[x]);
			size = splitWithComma(lineBuff[x], values);
			if (size - 4 > 1)
				GPSDataTmp->PDOP = atof(values[size - 4]);
			else
				continue;
			//printc(MinDelay, "$PDOP %s \n", values[size - 4]);
			GPSDataTmp->HDOP = atof(values[size - 3]);
			//printc(MinDelay, "$HDOP %s \n", values[size - 3]);
		}
	}
}
void L86_RxHalfCpltCallback() {
	memset(tmpData, L86Buffer, L86_BUFFER_SIZE / 2 + 1);
	memcpy(tmpData, L86Buffer, L86_BUFFER_SIZE / 2);
	ProcessData(tmpData);

}

void L86_RxCpltCallback() {
	memset(tmpData, L86Buffer, L86_BUFFER_SIZE / 2 + 1);
	memcpy(tmpData, &L86Buffer[L86_BUFFER_SIZE / 2], L86_BUFFER_SIZE / 2);
	ProcessData(tmpData);

}

