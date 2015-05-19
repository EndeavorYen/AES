/*****************************
 ** AES Code v.5   by Simon **
 *****************************/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h> 
#include <string.h>
#include "common.c"

typedef  unsigned char  BYTE; 
typedef  unsigned short u_short;
typedef  unsigned int   u_int;

/* ====================== 加密用Function ======================*/

void encrypt_AES_main(void);                                                                // 加密主程式
int  pictureFormatAnalysis(FILE *input);                                                    // 判斷圖片檔案格式 
void outputName(char *fileName);                                                            // 處理 output 檔名   
void encrypt_Round(BYTE *dataBlock, BYTE *W, int Nb, int Nk, int Nr);                       // 加密程序 
void keyGenerator(BYTE *W, int Nk, int round);                                              // 金鑰產生器 
void byte_Sub(BYTE *dataBlock, int Nb);                                                     // substitude byte Function
void shift_Row(BYTE *dataBlock, int Nb);                                                    // shift row Function
void mix_Column(BYTE *dataBlock, int Nb, int type);                                         // Mix Cloumn Function

/* ====================== 解密用Function ======================*/

void decrypt_AES_main(void);                                                                // 解密主程式 
void decrypt_Round(BYTE *dataBlock, BYTE *W, int Nb, int Nk, int Nr);                       // 解密程序
void Import_Key(BYTE *W, int Nk, int round);                                                // 匯入 KEY 的 Function 
void byte_InvSub(BYTE *dataBlock, int Nb);                                                  // Inverse Byte Substitude
void shift_InvRow(BYTE *dataBlock, int Nb);                                                 // shift row Function

/* ====================== Global Variable ======================*/

FILE *input, *output, *keyFile;
extern BYTE box_array[256];




/* ============================================================*/
/* ====================== Main Fuction ========================*/
/* ============================================================*/




int main(void){
	
	int option = 0;
	
	do{
		printf("\n請問您要加密還是解密?\n\n");
		printf("[1] 加密\n"
		       "[2] 解密\n"
			   "[3] 離開\n\n> ");
		scanf("%d", &option);	   
	} while(option < 0 || option > 3);
	
	switch(option){
		
		case 1 :
			create_Sbox(box_array);
			encrypt_AES_main();
			break;
		case 2 :
			create_Sbox(box_array);
		    decrypt_AES_main();
			break;
		default :
		    break;		
	}
	 
	
	system("PAUSE");
	return 0;
}




/* ============================================================*/
/* =====================  加密Function  =======================*/
/* ============================================================*/




/* 加密 Main Function */

void encrypt_AES_main(void){
	
	char fileName[64] = {0};
	int Nb = 0, Nk = 0, Nr = 0, length = 0;
	int i, j, formatOption, k;
	
	
	/* 選擇要加密的格式 */
	
	do{
		printf("請選擇你要加密的方式 : \n\n"
		       "[1] 圖片檔內容加密 (目前只支援 BMP)\n"
			   "[2] 整個檔案加密(包含文字檔)\n"
			   "[0] 結束程式\n" 
			   "\n請輸入0-2 >)");
		scanf("%d", &formatOption);
		
		if(formatOption == 0)
		   exit(1);
		   	   
	} while(formatOption < 0 || formatOption > 2);
	
	/* 檔案輸入和輸出選擇 */	
	
	printf("請輸入你要加密的檔案名稱，或是將檔案拖曳至視窗中 : ");
	scanf("%s", fileName);
	
	
	if((input = fopen(fileName, "rb")) == NULL){

		printf("File \"%s\" can't be open...\n\n", fileName);
		exit(0);
	}
	
	/* 判斷加密和輸入的檔案格式是否相符 */
	
	if(formatOption == 1){
		formatOption =  pictureFormatAnalysis(input);	
	}
	
    
    /* 修改輸出檔名 */
    
    outputName(fileName);
	    
	
	if((output = fopen(fileName, "wb")) == NULL){

		printf("File \"%s\" can't be create...\n\n");
		exit(1);
	}
	
	
	do{
		printf("請選擇你要加密的區塊大小 : \n\n"
		       "[1] 128 bits\n"
			   "[2] 192 bits\n"
			   "[3] 256 bits\n"
			   "[0] 結束程式\n" 
			   "\n請輸入0-3 >)");
		scanf("%d", &Nb);
		
		if(Nb == 0)
		   exit(1);
		   	   
	} while(Nb < 0 || Nb > 3);
	

	/* 選擇金鑰長度 */	
	
	do{
		printf("\n\n請選擇金鑰的長度 : \n\n"
		       "[1] 128 bits\n"
			   "[2] 192 bits\n"
			   "[3] 256 bits\n"
			   "[0] 結束程式\n" 
			   "\n請輸入0-3 >)");
		scanf("%d", &Nk);
		
		if(Nk == 0)
		   exit(1);
		   	   
	} while(Nk < 0 || Nk > 3);
	

    /* 開啟金鑰檔案 */

    if((keyFile = fopen("AES_key.txt", "wb")) == NULL){
		
		printf("無法建立金鑰...");
		exit(3); 
	}
   
	/* 紀錄加密資訊至 file header */
	
	keyHeader(input, keyFile, &Nb, &Nk, formatOption - 1);
	
	
	/* 決定Nr */
	
	Nr = NrTable(Nb, Nk);
	
	
	/* 宣告所需陣列 */
	 
	BYTE *dataBlock = calloc(Nb * 4, sizeof(BYTE));
	BYTE *W = calloc(Nb * (Nr + 1) * 4, sizeof(BYTE)); 
	
	
	/* 產生加密金鑰 */
    
    keyGenerator(W, Nk, Nb * (Nr + 1)); 
	
	
	/* Expanded Key */
	
	key_Expanded(W, Nb, Nk, Nr);
	
	
	/* 處理 file header 問題 */
	
	fseek(input, 0L, SEEK_SET);
	
	if(formatOption == 1){
		fread(dataBlock, 1, 20, input);
		fwrite(dataBlock, 1, 20, output);
		fread(dataBlock, 1, 20, input);
		fwrite(dataBlock, 1, 20, output);
		fread(dataBlock, 1, 14, input);
		fwrite(dataBlock, 1, 14, output);
	}
	
	
	/* 輸入至dataBlock, 進行加密*/
	
	BYTE *tempBlock = calloc(1, 4 * Nb);
	
	while((length = fread(tempBlock, 1, Nb * 4, input)) != 0){
        
        /* 最後一回合 Block 剩餘的格子填滿 0 */
        
		if(length < Nb * 4){
			for(i = length; i < Nb * 4; i++){
				tempBlock[i] = 0;
			}	
		}
		
		/* 擺放成垂直 Block */
		
		for(i = 0, k = 0; i < Nb; i++){
        	for(j = 0; j < 4; j++, k++){
        		dataBlock[i + Nb * j] = tempBlock[k];
        	}
        }
		
		encrypt_Round(dataBlock, W, Nb, Nk, Nr);
		
		
		/* 擺放成水平 Block */
		
		for(i = 0, k = 0; i < Nb; i++){
        	for(j = 0; j < 4; j++, k++){
        		tempBlock[k] = dataBlock[i + Nb * j];
        	}
        }
	
		fwrite(tempBlock, 1, 4 * Nb, output);

	}

    
	fclose(input);
	fclose(output);
	free(W);
	free(dataBlock);
	
	
	return ;
}

/* ========================= Function分隔線 ========================= */

/* 判斷圖片格式的 Function */

int pictureFormatAnalysis(FILE *input){
    
    
    BYTE format[2] = {0};
    
    fread(format, 1, 2, input);
    
    
    /* 判斷是否為 BMP 的格式 */ 
    
	if((format[0] == 0x42) && (format[1] == 0x4D))
	   return 1;	
	else{
	    printf("檔案格式不是圖片檔, 無法正確讀取 \n\n!!! 開始摧毀電腦中...\n\n");
        exit(0);	
	}
	 
    return ; 

}
 

/* ========================= Function分隔線 ========================= */

/* Output File Function */

void outputName(char *fileName){
	
	char str1[] = "_AES_";
	
	strcat(str1, fileName);	
	strcpy(fileName, str1);
	
	return ;
}

/* ========================= Function分隔線 ========================= */

/* 加密程序 Function */

void encrypt_Round(BYTE *dataBlock, BYTE *W, int Nb, int Nk, int Nr){
	
	int i;
	
	for(i = 0; i <= Nr; i++){
		
		/* i為回合數 */
		if(i == 0)
		   add_RoundKey(dataBlock, W, i, Nb, Nb * (Nr + 1));	
		
		else if(i == Nr){
		   byte_Sub(dataBlock, Nb);
		   shift_Row(dataBlock, Nb);
		   add_RoundKey(dataBlock, W, i, Nb, Nb * (Nr + 1));
		}
		   
		else{		
		   byte_Sub(dataBlock, Nb);
		   shift_Row(dataBlock, Nb);
		   mix_Column(dataBlock, Nb, 0);
		   add_RoundKey(dataBlock, W, i, Nb, Nb * (Nr + 1));			
		}   
	}
	
	return;
}

/* ========================= Function分隔線 ========================= */

/* Key Generation */

void keyGenerator(BYTE *W, int Nk, int round){
	
	int i, j;
	
	/*
	if((keyFile = fopen("AES_key.txt", "wt")) == NULL){
		
		printf("無法建立金鑰...");
		exit(3); 
	}
	*/
	
	
	
	printf("************ 金鑰鍊成中 ************\n");
	printf("\n\n************ 金鑰 AES_key.txt 產生 ************\n\n");
	printf("你的金鑰為 : > "); 
	srand(time(NULL));
	
	/* i 控制列數 */
	
	for(j = 0; j < Nk ; j++){
		
		/* j 控制行數 */
		
		for(i = 0; i < 4; i++){
			
			do{
			
			    W[j + round * i] = (BYTE)(rand() % 75 + '0');
			
		    } while((W[j + round * i] > 57 && W[j + round * i] < 65) || (W[j + round * i] > 90 && W[j + round * i] < 97));
			
			printf("%c", (W[j + round * i]));
			fprintf(keyFile,"%c",W[j + round * i]);
		}
		
	}
	
	printf("\n\n");
	
	fclose(keyFile);
	
	return ;
	
}

/* ========================= Function分隔線 ========================= */

/* Shift Byte Function */

void byte_Sub(BYTE *dataBlock, int Nb){
	
	int j;
	
	for(j = 0; j < Nb * 4; j++)
	   dataBlock[j] = box_array[(int)dataBlock[j]];

	return ;
}

/* ========================= Function分隔線 ========================= */

/* Shift Row Function */

void shift_Row(BYTE *dataBlock, int Nb){
	
	int C[3] = {Nb - 1,Nb - 2,Nb - 3};
	int j, k;
	BYTE *temp = calloc(Nb, sizeof(BYTE));
	
	if(Nb == 8){
		C[1] = Nb - 3;
		C[2] = Nb - 4;
	}
	   
	
	/* j控制列數 */
	   
	for(j = 1; j < 4; j++){
		
		/* k控制行數 */ 
		
		for(k = 0; k < Nb; k++){
			
			temp[(k + C[j - 1]) % Nb] = dataBlock[k + Nb * j];					
		}
		
		for(k = 0; k < Nb; k++)
		   dataBlock[k + Nb * j] = temp[k];
	}    

    free(temp);
	return ;
}

/* ========================= Function分隔線 ========================= */

/* Mix Column Function */

void mix_Column(BYTE *dataBlock, int Nb, int type){
	
	BYTE C1[4][4] ={2,3,1,1,
	                1,2,3,1,
				    1,1,2,3,
				    3,1,1,2};
				    
	BYTE C2[4][4]= {14,11,13, 9,
	                 9,14,11,13,
				    13, 9,14,11,
				    11,13, 9,14};
				       
	BYTE temp[4][8] = {0,0,0};			    
	int i, j, k;
	
	/* 最外面的loop 控制列數 */ 
	
	for(j = 0; j < Nb; j++){
		
		/* 第二個loop 控制目前行數*/
		
		for(i = 0; i < 4; i++){
			
			/* 做 column mixed ，並先暫存到temp中 (怕影響原來的值)*/
			
			for(k = 0; k < 4; k++){
			
				if(type == 0)
				   temp[i][j] = temp[i][j] ^ (BYTE)GF2_Multiply((u_short)C1[i][k],(u_short)dataBlock[j + k * Nb]);
				else
				   temp[i][j] = temp[i][j] ^ (BYTE)GF2_Multiply((u_short)C2[i][k],(u_short)dataBlock[j + k * Nb]);   
		    }
		}		   
	}
	
	for(i = 0; i < 4; i++){
			for(j = 0; j < Nb; j++){
				
				dataBlock[j + i * Nb] = temp[i][j];
	        }
    }
    

	return ;
}





/* ============================================================*/
/* =====================  解密Function  =======================*/
/* ============================================================*/




/* 解密 Main Function */

void decrypt_AES_main(void){
	char inputName[64] = {0};
	char outputName[64] = {0};
	int Nb = 0, Nk = 0, Nr = 0, length = 0;
	int i, j, k, counter = 0, formatOption = 0;
	unsigned int blockNumber = 0;
	BYTE remain = 0;
	BYTE headerBuffer[54] = {0};
	
	
	/* 檔案輸入和輸出選擇 */	
	
	printf("請輸入你要解密的檔案名稱，或是將檔案拖曳至視窗中 : ");
	scanf("%s", inputName);
	printf("請輸入你要輸出的檔案名稱 : ");
	scanf("%s", outputName);
	
	if((input = fopen(inputName, "rb")) == NULL){

		printf("File \"%s\" can't be open...\n\n", inputName);
		exit(0);
	}

	if((output = fopen(outputName, "wb")) == NULL){

		printf("File \"%s\" can't be create...\n\n");
		exit(1);
	}
	
	
	/* 開啟金鑰檔案 */

    if((keyFile = fopen("AES_key.txt", "rb")) == NULL){
		
		printf("******找不到加密金鑰******");
		exit(3); 
	}
	
	/* 讀取加密訊息 */
	
	formatOption = readHeader(keyFile, &Nb, &Nk, &remain, &blockNumber);     
	
	
	/* 決定Nr */
	
	Nr = NrTable(Nb, Nk);
	
	
	/* 宣告所需陣列 */
	 
	BYTE *dataBlock = calloc(Nb * 4, sizeof(BYTE));
	BYTE *W = calloc(Nb * (Nr + 1) * 4, sizeof(BYTE)); 
	
	
	/* 匯入解密金鑰 */

    Import_Key(W, Nk, Nb * (Nr + 1)); 
	 
	
	/* Expanded Key */
	
	key_Expanded(W, Nb, Nk, Nr);
	
	
	/* 將box_array改回 Inverse Sbox */
	
	create_ISbox(box_array);
	
	/* 處理 BMP file header 問題 */
	
	if(formatOption == 0){
		fread(headerBuffer, 1, 54, input);
		fwrite(headerBuffer, 1, 54, output);
	}
	
	/* 輸入至dataBlock, 進行解密 */
	
	BYTE *tempBlock = calloc(1, 4 * Nb);
	
	while((length = fread(tempBlock, 1, Nb * 4, input)) != 0){
        
		counter++;
        
        /* 擺放成垂直 Block */
		
		for(i = 0, k = 0; i < Nb; i++){
        	for(j = 0; j < 4; j++, k++){
        		dataBlock[i + Nb * j] = tempBlock[k];
        	}
        }
        
        decrypt_Round(dataBlock, W, Nb, Nk, Nr);
        
        /* 擺放成水平 Block */
		
		for(i = 0, k = 0; i < Nb; i++){
        	for(j = 0; j < 4; j++, k++){
        		tempBlock[k] = dataBlock[i + Nb * j];
        	}
        }
        
        if((counter == blockNumber) && (remain != 0)){
        	//printf(">>length = %d\n", remain);
        	fwrite(tempBlock, 1, remain, output);
        }
        else{
        	//printf("length = %d\n", length);
		    fwrite(tempBlock, 1, length, output);	
        }
        
	}

	fclose(input);
	fclose(output);
	free(W);
	free(dataBlock);
	
	return ;
}

/* ========================= Function分隔線 ========================= */

/* 解密程序 Function */

void decrypt_Round(BYTE *dataBlock, BYTE *W, int Nb, int Nk, int Nr){
	
	int i;
	
	/* i為回合數 */
	
	for(i = Nr; i >= 0; i--){
		
		
		
		if(i == Nr)
		   add_RoundKey(dataBlock, W, i, Nb, Nb * (Nr + 1));	
		
		else if(i == 0){
		   shift_InvRow(dataBlock, Nb);
		   byte_InvSub(dataBlock, Nb);
		   add_RoundKey(dataBlock, W, i, Nb, Nb * (Nr + 1));
		}
		   
		else{		
		   shift_InvRow(dataBlock, Nb);
		   byte_InvSub(dataBlock, Nb);		   
		   add_RoundKey(dataBlock, W, i, Nb, Nb * (Nr + 1));			
		   mix_Column(dataBlock, Nb, 1);
		}   
	}
	
	return;
}

/* ========================= Function分隔線 ========================= */

/* Import Key Function */

void Import_Key(BYTE *W, int Nk, int round){
	
	int i, j;
	
	/* i 控制列數 */
	
	for(j = 0; j < Nk; j++){
		
		/* j 控制行數 */
		
	   for(i = 0; i < 4; i++){
	   	
	       fread(&W[j + round * i], 1, 1, keyFile);
	   	
	   } 
	} 
	
	return ;
}

/* ========================= Function分隔線 ========================= */

/* Inverse Shift Byte Function */

void byte_InvSub(BYTE *dataBlock, int Nb){
	
	int j;
	
	for(j = 0; j < Nb * 4; j++)
	   dataBlock[j] = I_SBox((int)dataBlock[j]);
	
	return ;
}

/* ========================= Function分隔線 ========================= */

/* Inverse Shift Row Function */

void shift_InvRow(BYTE *dataBlock, int Nb){
	
	int C[3] = {1,2,3};
	int j, k;
	BYTE *temp = calloc(Nb, sizeof(BYTE));
	
	if(Nb == 8){
		C[1] = 3;
		C[2] = 4;
	}
	   
	
	/* j控制列數 */
	   
	for(j = 1; j < 4; j++){
		
		/* k控制行數 */ 
		
		for(k = 0; k < Nb; k++){
			
			temp[(k + C[j - 1]) % Nb] = dataBlock[k + Nb * j];					
		}
		
		for(k = 0; k < Nb; k++)
		   dataBlock[k + Nb * j] = temp[k];
	}    
	
	free(temp);
	return ;
}



