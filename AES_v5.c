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

/* ====================== �[�K��Function ======================*/

void encrypt_AES_main(void);                                                                // �[�K�D�{��
int  pictureFormatAnalysis(FILE *input);                                                    // �P�_�Ϥ��ɮ׮榡 
void outputName(char *fileName);                                                            // �B�z output �ɦW   
void encrypt_Round(BYTE *dataBlock, BYTE *W, int Nb, int Nk, int Nr);                       // �[�K�{�� 
void keyGenerator(BYTE *W, int Nk, int round);                                              // ���_���;� 
void byte_Sub(BYTE *dataBlock, int Nb);                                                     // substitude byte Function
void shift_Row(BYTE *dataBlock, int Nb);                                                    // shift row Function
void mix_Column(BYTE *dataBlock, int Nb, int type);                                         // Mix Cloumn Function

/* ====================== �ѱK��Function ======================*/

void decrypt_AES_main(void);                                                                // �ѱK�D�{�� 
void decrypt_Round(BYTE *dataBlock, BYTE *W, int Nb, int Nk, int Nr);                       // �ѱK�{��
void Import_Key(BYTE *W, int Nk, int round);                                                // �פJ KEY �� Function 
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
		printf("\n�аݱz�n�[�K�٬O�ѱK?\n\n");
		printf("[1] �[�K\n"
		       "[2] �ѱK\n"
			   "[3] ���}\n\n> ");
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
/* =====================  �[�KFunction  =======================*/
/* ============================================================*/




/* �[�K Main Function */

void encrypt_AES_main(void){
	
	char fileName[64] = {0};
	int Nb = 0, Nk = 0, Nr = 0, length = 0;
	int i, j, formatOption, k;
	
	
	/* ��ܭn�[�K���榡 */
	
	do{
		printf("�п�ܧA�n�[�K���覡 : \n\n"
		       "[1] �Ϥ��ɤ��e�[�K (�ثe�u�䴩 BMP)\n"
			   "[2] ����ɮץ[�K(�]�t��r��)\n"
			   "[0] �����{��\n" 
			   "\n�п�J0-2 >)");
		scanf("%d", &formatOption);
		
		if(formatOption == 0)
		   exit(1);
		   	   
	} while(formatOption < 0 || formatOption > 2);
	
	/* �ɮ׿�J�M��X��� */	
	
	printf("�п�J�A�n�[�K���ɮצW�١A�άO�N�ɮש즲�ܵ����� : ");
	scanf("%s", fileName);
	
	
	if((input = fopen(fileName, "rb")) == NULL){

		printf("File \"%s\" can't be open...\n\n", fileName);
		exit(0);
	}
	
	/* �P�_�[�K�M��J���ɮ׮榡�O�_�۲� */
	
	if(formatOption == 1){
		formatOption =  pictureFormatAnalysis(input);	
	}
	
    
    /* �ק��X�ɦW */
    
    outputName(fileName);
	    
	
	if((output = fopen(fileName, "wb")) == NULL){

		printf("File \"%s\" can't be create...\n\n");
		exit(1);
	}
	
	
	do{
		printf("�п�ܧA�n�[�K���϶��j�p : \n\n"
		       "[1] 128 bits\n"
			   "[2] 192 bits\n"
			   "[3] 256 bits\n"
			   "[0] �����{��\n" 
			   "\n�п�J0-3 >)");
		scanf("%d", &Nb);
		
		if(Nb == 0)
		   exit(1);
		   	   
	} while(Nb < 0 || Nb > 3);
	

	/* ��ܪ��_���� */	
	
	do{
		printf("\n\n�п�ܪ��_������ : \n\n"
		       "[1] 128 bits\n"
			   "[2] 192 bits\n"
			   "[3] 256 bits\n"
			   "[0] �����{��\n" 
			   "\n�п�J0-3 >)");
		scanf("%d", &Nk);
		
		if(Nk == 0)
		   exit(1);
		   	   
	} while(Nk < 0 || Nk > 3);
	

    /* �}�Ҫ��_�ɮ� */

    if((keyFile = fopen("AES_key.txt", "wb")) == NULL){
		
		printf("�L�k�إߪ��_...");
		exit(3); 
	}
   
	/* �����[�K��T�� file header */
	
	keyHeader(input, keyFile, &Nb, &Nk, formatOption - 1);
	
	
	/* �M�wNr */
	
	Nr = NrTable(Nb, Nk);
	
	
	/* �ŧi�һݰ}�C */
	 
	BYTE *dataBlock = calloc(Nb * 4, sizeof(BYTE));
	BYTE *W = calloc(Nb * (Nr + 1) * 4, sizeof(BYTE)); 
	
	
	/* ���ͥ[�K���_ */
    
    keyGenerator(W, Nk, Nb * (Nr + 1)); 
	
	
	/* Expanded Key */
	
	key_Expanded(W, Nb, Nk, Nr);
	
	
	/* �B�z file header ���D */
	
	fseek(input, 0L, SEEK_SET);
	
	if(formatOption == 1){
		fread(dataBlock, 1, 20, input);
		fwrite(dataBlock, 1, 20, output);
		fread(dataBlock, 1, 20, input);
		fwrite(dataBlock, 1, 20, output);
		fread(dataBlock, 1, 14, input);
		fwrite(dataBlock, 1, 14, output);
	}
	
	
	/* ��J��dataBlock, �i��[�K*/
	
	BYTE *tempBlock = calloc(1, 4 * Nb);
	
	while((length = fread(tempBlock, 1, Nb * 4, input)) != 0){
        
        /* �̫�@�^�X Block �Ѿl����l�� 0 */
        
		if(length < Nb * 4){
			for(i = length; i < Nb * 4; i++){
				tempBlock[i] = 0;
			}	
		}
		
		/* �\�񦨫��� Block */
		
		for(i = 0, k = 0; i < Nb; i++){
        	for(j = 0; j < 4; j++, k++){
        		dataBlock[i + Nb * j] = tempBlock[k];
        	}
        }
		
		encrypt_Round(dataBlock, W, Nb, Nk, Nr);
		
		
		/* �\�񦨤��� Block */
		
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

/* ========================= Function���j�u ========================= */

/* �P�_�Ϥ��榡�� Function */

int pictureFormatAnalysis(FILE *input){
    
    
    BYTE format[2] = {0};
    
    fread(format, 1, 2, input);
    
    
    /* �P�_�O�_�� BMP ���榡 */ 
    
	if((format[0] == 0x42) && (format[1] == 0x4D))
	   return 1;	
	else{
	    printf("�ɮ׮榡���O�Ϥ���, �L�k���TŪ�� \n\n!!! �}�l�R���q����...\n\n");
        exit(0);	
	}
	 
    return ; 

}
 

/* ========================= Function���j�u ========================= */

/* Output File Function */

void outputName(char *fileName){
	
	char str1[] = "_AES_";
	
	strcat(str1, fileName);	
	strcpy(fileName, str1);
	
	return ;
}

/* ========================= Function���j�u ========================= */

/* �[�K�{�� Function */

void encrypt_Round(BYTE *dataBlock, BYTE *W, int Nb, int Nk, int Nr){
	
	int i;
	
	for(i = 0; i <= Nr; i++){
		
		/* i���^�X�� */
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

/* ========================= Function���j�u ========================= */

/* Key Generation */

void keyGenerator(BYTE *W, int Nk, int round){
	
	int i, j;
	
	/*
	if((keyFile = fopen("AES_key.txt", "wt")) == NULL){
		
		printf("�L�k�إߪ��_...");
		exit(3); 
	}
	*/
	
	
	
	printf("************ ���_�妨�� ************\n");
	printf("\n\n************ ���_ AES_key.txt ���� ************\n\n");
	printf("�A�����_�� : > "); 
	srand(time(NULL));
	
	/* i ����C�� */
	
	for(j = 0; j < Nk ; j++){
		
		/* j ������ */
		
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

/* ========================= Function���j�u ========================= */

/* Shift Byte Function */

void byte_Sub(BYTE *dataBlock, int Nb){
	
	int j;
	
	for(j = 0; j < Nb * 4; j++)
	   dataBlock[j] = box_array[(int)dataBlock[j]];

	return ;
}

/* ========================= Function���j�u ========================= */

/* Shift Row Function */

void shift_Row(BYTE *dataBlock, int Nb){
	
	int C[3] = {Nb - 1,Nb - 2,Nb - 3};
	int j, k;
	BYTE *temp = calloc(Nb, sizeof(BYTE));
	
	if(Nb == 8){
		C[1] = Nb - 3;
		C[2] = Nb - 4;
	}
	   
	
	/* j����C�� */
	   
	for(j = 1; j < 4; j++){
		
		/* k������ */ 
		
		for(k = 0; k < Nb; k++){
			
			temp[(k + C[j - 1]) % Nb] = dataBlock[k + Nb * j];					
		}
		
		for(k = 0; k < Nb; k++)
		   dataBlock[k + Nb * j] = temp[k];
	}    

    free(temp);
	return ;
}

/* ========================= Function���j�u ========================= */

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
	
	/* �̥~����loop ����C�� */ 
	
	for(j = 0; j < Nb; j++){
		
		/* �ĤG��loop ����ثe���*/
		
		for(i = 0; i < 4; i++){
			
			/* �� column mixed �A�å��Ȧs��temp�� (�ȼv�T��Ӫ���)*/
			
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
/* =====================  �ѱKFunction  =======================*/
/* ============================================================*/




/* �ѱK Main Function */

void decrypt_AES_main(void){
	char inputName[64] = {0};
	char outputName[64] = {0};
	int Nb = 0, Nk = 0, Nr = 0, length = 0;
	int i, j, k, counter = 0, formatOption = 0;
	unsigned int blockNumber = 0;
	BYTE remain = 0;
	BYTE headerBuffer[54] = {0};
	
	
	/* �ɮ׿�J�M��X��� */	
	
	printf("�п�J�A�n�ѱK���ɮצW�١A�άO�N�ɮש즲�ܵ����� : ");
	scanf("%s", inputName);
	printf("�п�J�A�n��X���ɮצW�� : ");
	scanf("%s", outputName);
	
	if((input = fopen(inputName, "rb")) == NULL){

		printf("File \"%s\" can't be open...\n\n", inputName);
		exit(0);
	}

	if((output = fopen(outputName, "wb")) == NULL){

		printf("File \"%s\" can't be create...\n\n");
		exit(1);
	}
	
	
	/* �}�Ҫ��_�ɮ� */

    if((keyFile = fopen("AES_key.txt", "rb")) == NULL){
		
		printf("******�䤣��[�K���_******");
		exit(3); 
	}
	
	/* Ū���[�K�T�� */
	
	formatOption = readHeader(keyFile, &Nb, &Nk, &remain, &blockNumber);     
	
	
	/* �M�wNr */
	
	Nr = NrTable(Nb, Nk);
	
	
	/* �ŧi�һݰ}�C */
	 
	BYTE *dataBlock = calloc(Nb * 4, sizeof(BYTE));
	BYTE *W = calloc(Nb * (Nr + 1) * 4, sizeof(BYTE)); 
	
	
	/* �פJ�ѱK���_ */

    Import_Key(W, Nk, Nb * (Nr + 1)); 
	 
	
	/* Expanded Key */
	
	key_Expanded(W, Nb, Nk, Nr);
	
	
	/* �Nbox_array��^ Inverse Sbox */
	
	create_ISbox(box_array);
	
	/* �B�z BMP file header ���D */
	
	if(formatOption == 0){
		fread(headerBuffer, 1, 54, input);
		fwrite(headerBuffer, 1, 54, output);
	}
	
	/* ��J��dataBlock, �i��ѱK */
	
	BYTE *tempBlock = calloc(1, 4 * Nb);
	
	while((length = fread(tempBlock, 1, Nb * 4, input)) != 0){
        
		counter++;
        
        /* �\�񦨫��� Block */
		
		for(i = 0, k = 0; i < Nb; i++){
        	for(j = 0; j < 4; j++, k++){
        		dataBlock[i + Nb * j] = tempBlock[k];
        	}
        }
        
        decrypt_Round(dataBlock, W, Nb, Nk, Nr);
        
        /* �\�񦨤��� Block */
		
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

/* ========================= Function���j�u ========================= */

/* �ѱK�{�� Function */

void decrypt_Round(BYTE *dataBlock, BYTE *W, int Nb, int Nk, int Nr){
	
	int i;
	
	/* i���^�X�� */
	
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

/* ========================= Function���j�u ========================= */

/* Import Key Function */

void Import_Key(BYTE *W, int Nk, int round){
	
	int i, j;
	
	/* i ����C�� */
	
	for(j = 0; j < Nk; j++){
		
		/* j ������ */
		
	   for(i = 0; i < 4; i++){
	   	
	       fread(&W[j + round * i], 1, 1, keyFile);
	   	
	   } 
	} 
	
	return ;
}

/* ========================= Function���j�u ========================= */

/* Inverse Shift Byte Function */

void byte_InvSub(BYTE *dataBlock, int Nb){
	
	int j;
	
	for(j = 0; j < Nb * 4; j++)
	   dataBlock[j] = I_SBox((int)dataBlock[j]);
	
	return ;
}

/* ========================= Function���j�u ========================= */

/* Inverse Shift Row Function */

void shift_InvRow(BYTE *dataBlock, int Nb){
	
	int C[3] = {1,2,3};
	int j, k;
	BYTE *temp = calloc(Nb, sizeof(BYTE));
	
	if(Nb == 8){
		C[1] = 3;
		C[2] = 4;
	}
	   
	
	/* j����C�� */
	   
	for(j = 1; j < 4; j++){
		
		/* k������ */ 
		
		for(k = 0; k < Nb; k++){
			
			temp[(k + C[j - 1]) % Nb] = dataBlock[k + Nb * j];					
		}
		
		for(k = 0; k < Nb; k++)
		   dataBlock[k + Nb * j] = temp[k];
	}    
	
	free(temp);
	return ;
}



