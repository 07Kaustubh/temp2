/*
 * E&D.c
 *
 *  Created on: Apr 6, 2024
 *      Author: Manky
 */
#include "main.h"

extern CRYP_HandleTypeDef hcryp;

int encrypt(char *Data, uint8_t En_data[]) {
	size_t string_length = strlen(Data);

	addPadding((uint8_t*) Data, string_length);
//	printc(MinDelay,"padded data = %s,%d \r\n", &Data, strlen(Data));
	string_length = strlen(Data);
	size_t num_uint32 = (string_length + 3) / 4;

	uint32_t SWData[num_uint32];
	for (size_t i = 0; i < num_uint32; i++) {
		SWData[i] = (uint32_t) (Data[i * 4 + 3])
				| (uint32_t) (Data[i * 4 + 2]) << 8
				| (uint32_t) (Data[i * 4 + 1]) << 16
				| (uint32_t) (Data[i * 4 + 0]) << 24;
	}

	// Encrypt the string
	uint32_t encryptedStr[num_uint32]; // Declaration
	memset(encryptedStr, 0, num_uint32 * sizeof(uint32_t)); // Initialization
	memset(En_data, 0, sizeof(En_data));
	HAL_CRYP_Encrypt(&hcryp, SWData, num_uint32, encryptedStr, 2000);

	// Convert encryptedStr to char array
	for (int i = 0; i < num_uint32; i++) {
		En_data[i * 4 + 0] = (encryptedStr[i] >> 24) & 0xFF;
		En_data[i * 4 + 1] = (encryptedStr[i] >> 16) & 0xFF;
		En_data[i * 4 + 2] = (encryptedStr[i] >> 8) & 0xFF;
		En_data[i * 4 + 3] = encryptedStr[i] & 0xFF;
	}

	printc(MinDelay,"Encrypted data String: ");
	for (int i = 0; i < string_length; i++) {
		printc(MinDelay,"%02X", En_data[i]);
	}
	printc(MinDelay,"\n\r");
	return string_length;
}

/*******************De-cryption Function************************/

void Decrypt(char *Data, uint8_t *De_data, int Key, int KeyType) {
	printc(100,"%s\n",Data);
	size_t string_length = strlen(Data);
	size_t num_uint32 = ((string_length /2) + 3) / 4;
	uint8_t E_Data[string_length/2];
//	ReadF_Loc(2, Key);
	if(KeyType ==3||KeyType ==4||KeyType ==6)
		cKey_update(Key - 1);
	else if(KeyType ==1)
		sKey_update(Key - 1);
	else
		printc(MinDelay,"Keytpe is Wrong");
	// Convert Data to uint32_t array
 	string_to_hex_array(Data, E_Data);
	uint32_t encryptedStr[num_uint32];
	memset(encryptedStr,0,num_uint32*sizeof(uint32_t));
	for (int i = 0; i < num_uint32; i++) {
		encryptedStr[i] = (uint32_t) (E_Data[i * 4 + 0]) << 24
				| (uint32_t) (E_Data[i * 4 + 1]) << 16
				| (uint32_t) (E_Data[i * 4 + 2]) << 8
				| (uint32_t) (E_Data[i * 4 + 3]);
	}

	// Decrypt the string
	uint32_t decryptedStr[num_uint32]; // Declaration
	memset(decryptedStr, 0, num_uint32 * sizeof(uint32_t)); // Initialization
	HAL_CRYP_Decrypt(&hcryp, encryptedStr, num_uint32, decryptedStr, 2000);

	// Convert decryptedStr to char array
	for (int i = 0; i < num_uint32; i++) {
		// Since char is 8 bits and uint32_t is 32 bits,
		// we can use bitwise AND operations to extract
		// individual bytes from the uint32_t and store them in De_data
		De_data[i * 4 + 0] = (decryptedStr[i] >> 24) & 0xFF;
		De_data[i * 4 + 1] = (decryptedStr[i] >> 16) & 0xFF;
		De_data[i * 4 + 2] = (decryptedStr[i] >> 8) & 0xFF;
		De_data[i * 4 + 3] = decryptedStr[i] & 0xFF;
	}
	string_length = strlen(De_data);
	// Null-terminate the decrypted string to print it properly
	De_data[string_length] = '\0';

//	 Print the decrypted string
//	printc(MinDelay,"Decrypted data String: ");
////	for (int i = 0; i < string_length; i++) {
//	print(1000,De_data,sizeof(De_data));
////	}
//	printc(MinDelay,"\n\r");
}

uint8_t calculateChecksum(uint8_t *dataBuffer) {
	uint8_t checksum = 0;
	while (*dataBuffer != '\0') {
		checksum ^= *dataBuffer; // XOR operation with each character
		dataBuffer++;
	}
	return checksum;
}

void addPadding(uint8_t *dataBody, size_t num_elements) {
	uint8_t padding_size = 16 - (num_elements % 16);
	//if padding_size should be <17
	if (padding_size < 16) {
		for (size_t i = num_elements; i < num_elements + padding_size; i++) {
			dataBody[i] = padding_size;
		}
	}
}

void string_to_hex_array(const char hex_string [],uint8_t hex_array[]) {
	// Calculate the length of the hex string
	size_t hex_string_length = strlen(hex_string);
	size_t array_length1;

	// Check if the length is even
	if (hex_string_length % 2 != 0) {
		fprintf(stderr, "Input string length must be even.\n");
		return;
	}

	// Calculate the length of the resulting array
	array_length1 = hex_string_length / 2;

	// Allocate memory for the array

	// Convert the hex string to hex values and store in the array
	for (size_t i = 0; i < array_length1; ++i) {
		sscanf(hex_string + 2 * i, "%2X", &hex_array[i]);
	}
}
