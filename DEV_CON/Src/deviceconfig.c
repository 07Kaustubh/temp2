/*
 * Device_Config.c
 *
 *  Created on: Sep 19, 2024
 *      Author: Shekhar Verma
 */
#include "deviceconfig.h"

DEV_Config_Status deviceConPacket = {.Header="$ZEN",.Key=0xEE,.Frametype=0xEE,.DataPacket="0xEE",.checksum=0xEE};
uint8_t splitTmpBuffer[25][100]={0};
bool parseData(uint8_t *dataBuffer,uint8_t *dataToSend){
	if(strstr(dataBuffer,deviceConPacket.Header)==NULL){
		return false;
	}
	clearBuffer(splitTmpBuffer, sizeof(splitTmpBuffer));
	int recLength=splitHexString(dataBuffer, '\xd7', splitTmpBuffer, 25);
	if(recLength<3)
		return false;
	if(strstr(splitTmpBuffer[1],"1234")==NULL)
		return false;
	sprintf(dataToSend,"\r\n%s%c%c%c",deviceConPacket.Header,DEV_ID_SEPERATOR,splitTmpBuffer[2][0],DEV_ID_SEPERATOR);
	if(splitTmpBuffer[2][0]==DEV_GET){
		for(int i=0;i<recLength-4;i++){
			int getIndex=splitTmpBuffer[3+i][1];
			generate_get_string(getIndex-1,dataToSend,splitTmpBuffer[3+i][0],splitTmpBuffer[3+i][1]);
		}
	}
	else if (splitTmpBuffer[2][0]==DEV_SET){
		for(int i=0;i<recLength-4;i++){
			int getIndex=splitTmpBuffer[3+i][1];
			char valueToSet[100]={};
			if(splitTmpBuffer[3+i][3]=='\0')
				continue;
			strcpy(valueToSet,&splitTmpBuffer[3+i][3]);
			//if(getIndex-1==Config_Mode){
			//if(valueToSet[0]=='D')
			//enable_configmode(false);
			//else
			//enable_configmode(true);
			//}
			//strcpy(valueToSet,&splitTmpBuffer[3+i][3]);
			setConfigValue(getIndex-1, valueToSet);
			//callbackfunction
			generate_get_string(getIndex-1,dataToSend,splitTmpBuffer[3+i][0],splitTmpBuffer[3+i][1]);
		}
	}
	else
		return false;
	char checksum[10]="\x4\xd1\r\n";
	strcat(dataToSend,checksum);
}
void generate_get_string(int index,char *buffer,char code1,char code2){
	char *value={0};
	value=getConfigValue(index);
	char tmpStr[2024]={};
	if(value[0]=='\0')
		sprintf(tmpStr,"%c%c%c%s%c",code1,code2,DEV_DATA_SEPERATOR,"",DEV_ID_SEPERATOR);
	else
		sprintf(tmpStr,"%c%c%c%s%c",code1,code2,DEV_DATA_SEPERATOR,value,DEV_ID_SEPERATOR);
	strcat(buffer,tmpStr);
}
int splitHexString(uint8_t *string, uint8_t delimiter, uint8_t opbuffer[][100], size_t bufsize) {
	uint8_t *start = string;
	uint8_t *end = NULL;
	int count = 0;

	while ((end = memchr(start, delimiter, strlen((char *)start))) != NULL) {
		size_t length = end - start; // Calculate substring length
		if (length > 0 && count < bufsize) {
			memset(opbuffer[count],0,100); // Clear the target buffer
			memcpy(opbuffer[count],start,length); // Copy the substring
			count++;
		}
		start = end + 1; // Move past the delimiter
	}
	printc(10,"Start: %s\n", start);
//	printc(10,"Delimiter found at: %ld\n", end - string);
//	printc(10,"Substring length: %zu\n", length);
	// Copy the remaining part after the last delimiter
	if (*start != '\0' && count < bufsize) {
		memset(opbuffer[count], 0, 100); // Clear the target buffer
		strcpy((char *)opbuffer[count], (char *)start); // Copy the rest of the string
		count++;
	}

	return count; // Return the number of substrings
}
//	if(splitTmpBuffer[2][0]==DEV_GET){
//		for(int i=0;i<recLength-4;i++){
////			int getIndex=escape_character(splitTmpBuffer[3+i][1],splitTmpBuffer[3+i][2]);
////			if(getIndex==0)
////				continue;
////			int code2 = decrept_chracter(getIndex);
//			generate_get_string(getIndex-1,dataToSend,splitTmpBuffer[3+i][0],splitTmpBuffer[3+i][1]);
//		}
//	}
//	else if (splitTmpBuffer[2][0]==DEV_SET){
//		for(int i=0;i<recLength-4;i++){
////			int getIndex=escape_character(splitTmpBuffer[3+i][1],splitTmpBuffer[3+i][2]);
////			if(getIndex==0)
////				continue;
////			char valueToSet[100]={};
//			if(splitTmpBuffer[3+i][3]=='\0')
//				continue;
//			strcpy(valueToSet,&splitTmpBuffer[3+i][3]);
//			if(getIndex-1==Config_Mode){
//				if(valueToSet[0]=='D')
//					enable_configmode(false);
//				else
//					enable_configmode(true);
//			}
//			//strcpy(valueToSet,&splitTmpBuffer[3+i][3]);
//			int code2 = decrept_chracter(getIndex);
//			setConfigValue(getIndex-1, valueToSet);
//			generate_get_string(getIndex-1,dataToSend,splitTmpBuffer[3+i][0],splitTmpBuffer[3+i][1]);
//		}
//	}
//	else
//		return false;

//uint8_t escape_character(uint8_t escapeValue,uint8_t value){
//	if(escapeValue!=0xC3){
//		return escapeValue;
//	}
//	switch(value)
//	{
//	case 0xBE:
//		return 44;//because it is (44= ,)
//		break;
//	default:
//		return 0;
//	}
//
//}
//uint8_t decrept_chracter(uint8_t value)
//{
//	switch(value)
//	{
//	case 44: return 254;
//	break;
//	default: return value;
//	}
//}
