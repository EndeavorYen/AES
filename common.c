
/* ============================================================*/
/* =====================  �@��Function  =======================*/
/* ============================================================*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h> 

typedef  unsigned char  BYTE; 
typedef  unsigned short u_short;
typedef  unsigned int   u_int;


int NrTable(int Nb, int Nk);                                                                // �^�X�ƨM�w table 
void key_Expanded(BYTE *W, int Nb, int Nk, int Nr);                                         // ���_�X�R Function 
void add_RoundKey(BYTE *dataBlock, BYTE *W, int i, int Nb, int round);                      // Add Round Key Function 
BYTE RCON(int i, int Nk);                                                                   // RCON Function  
u_short GF2_Multiply(u_short num1, u_short num2);                                           // GF2  Multiply  Function
BYTE GF2_Inverse(u_short num1);                                                             // GF2  Inverse   Function
BYTE S_Box(u_short num1);                                                                   // S-Box Function
BYTE I_SBox(u_short num1);                                                                  // S-Box Inverse Function
void keyHeader(FILE *input, FILE *keyFile, int *ptrNb, int *ptrNk, int Option);             // �B�z File Header�� Fuction 
int readHeader(FILE *keyFile, int *ptrNb, int *ptrNk, BYTE *ptrA, u_int *blockNumber);      // Ū�� Header �� information
void create_Sbox(BYTE box_array[]);                                                         // ���� S_box �� arrary
void create_ISbox(BYTE box_array[]);                                                        // ���� S_box �� arrary

        
BYTE box_array[256] = {0};		                                                         
                                                                 
/* ========================= Function���j�u ========================= */

/* Nr Table */

int NrTable(int Nb, int Nk){
	
	int table[3][3] = {10,12,14,12,12,14,14,14,14};
	
	return table[Nk / 2 - 2][Nb / 2 - 2];
	
}

/* ========================= Function���j�u ========================= */

/* Key Expanded Function */

void key_Expanded(BYTE *W, int Nb, int Nk, int Nr){
	
	int i;
	int round = Nb * (Nr + 1);
	BYTE temp;
	
	/* i �N�� W ���C�@�� �A round * 1 �N��U�@�C */
	
	
	for(i = Nk; i < round; i++){
		
		if(i % Nk == 0){
			W[i]             = W[i - Nk] ^ (temp = box_array[((u_short)W[i + round * 1 - 1])]) ^ RCON(i, Nk);
			W[i + round * 1] = W[i + round * 1 - Nk] ^ box_array[(BYTE)W[i + round * 2 - 1]];
			W[i + round * 2] = W[i + round * 2 - Nk] ^ box_array[(BYTE)W[i + round * 3 - 1]];
			W[i + round * 3] = W[i + round * 3 - Nk] ^ box_array[(BYTE)W[i - 1]];				
		}
		else if((Nk > 6) && (i % 4) == 0){
			W[i] = W[i - Nk] ^ box_array[(BYTE)W[i - 1]];
			W[i + round * 1] = W[i + round * 1 - Nk] ^ box_array[(BYTE)W[i + round * 1 - 1]];
			W[i + round * 2] = W[i + round * 2 - Nk] ^ box_array[(BYTE)W[i + round * 2 - 1]];
			W[i + round * 3] = W[i + round * 3 - Nk] ^ box_array[(BYTE)W[i + round * 3 - 1]];
		}
		else{
			W[i] = W[i - Nk] ^ W[i - 1];
			W[i + round * 1] = W[i + round * 1 - Nk] ^ W[i + round * 1 - 1];
			W[i + round * 2] = W[i + round * 2 - Nk] ^ W[i + round * 2 - 1];
			W[i + round * 3] = W[i + round * 3 - Nk] ^ W[i + round * 3 - 1];
		}	   
	}  
    
	return ;
}

/* ========================= Function���j�u ========================= */

/* Add Round Key Function */

void add_RoundKey(BYTE *dataBlock, BYTE *W, int i, int Nb, int round){
	
	int j;

	/* j �� dataBlock ����� */
		
    for(j = 0; j < Nb; j++){ 
    
	   dataBlock[j]          = dataBlock[j]          ^ W[i * Nb + j];
	   dataBlock[j + Nb * 1] = dataBlock[j + Nb * 1] ^ W[i * Nb + j + round * 1];
	   dataBlock[j + Nb * 2] = dataBlock[j + Nb * 2] ^ W[i * Nb + j + round * 2];
	   dataBlock[j + Nb * 3] = dataBlock[j + Nb * 3] ^ W[i * Nb + j + round * 3];
      
	}
	
    
	return ;
}

/* ========================= Function���j�u ========================= */

/* RCON Function */

BYTE RCON(int i, int Nk){
	
	static rcon = 1;
	i -= Nk;
	
	if(i != 0)
		return (rcon = (rcon<<1) ^ (0x11b & -(rcon>>7)));
	   
	else
		return rcon;   
    
}

/* ========================= Function���j�u ========================= */

/* GF2 Multiply Function */

u_short GF2_Multiply(u_short num1, u_short num2)
{
	u_short A[2] = {0,0};
	int i;
	u_short num3 = num2;

	/* 
	   �A���O�N A �V������ (�̤j�u���� 16 bits, 2 Bytes)
	   �A��C�Ӽư� XOR 
	   A[1] ��@�@�ӼȦs���A�s��C���Q���������G�A�M��MA[0]��XOR 
	*/
	

	for(i = 0; i < 8; i++){

          if((num3 % 2 ) != 0){                   // �Y�̫�@�� bit �� 1 
			  if(i == 0)                          // �Y�O�Ĥ@�^�X 
			     A[0] = num1 << i;
			  else
			     A[1] = num1 << i;	              // �h A[i] �� num1 �V���� i �� 
          }
          if(i > 0){                              // �ĤG�^�X�}�l���� 
		      A[0] = A[0] ^ A[1];                 // ��ۭ������GXOR�_�� 
		      A[1] = 0;
		  } 
		   
		  num3 = num3 >> 1;                       // ��B�⧹��bit������ 
    }
	
	/* �B�z overflow */ 
	
	for(i = 14; i > 7; i--){
		if(A[0] >= (1 << i)){
			A[0] = A[0] ^ (283 << (i - 8));
		}
	}
	
	return A[0];
}

/* ========================= Function���j�u ========================= */

/* S-Box Inverse Function */

BYTE I_SBox(u_short num1)
{
	BYTE y[8] = {0,0,0,0,0,0,0,0};
	BYTE x[8] = {0,0,0,0,0,0,0,0};
	BYTE z[8] = {1,0,1,0,0,0,0,0};
	BYTE matrix[8][8] = {0,0,1,0,0,1,0,1,
	                              1,0,0,1,0,0,1,0,
								  0,1,0,0,1,0,0,1,
								  1,0,1,0,0,1,0,0,
								  0,1,0,1,0,0,1,0,
								  0,0,1,0,1,0,0,1,
								  1,0,0,1,0,1,0,0,
								  0,1,0,0,1,0,1,0};
						 
	int i, j;
	
	
	for(i = 0; i < 8; i++)
	{
		if((num1 % 2) == 1)
		   x[i] = 1;
		
		num1 /= 2;   
	}

	
	for(i = 0; i < 8; i++)
	{
		for(j = 0; j < 8; j++)
		   y[i] = y[i] ^ (matrix[i][j] & x[j]);
		
		y[i] = y[i] ^ z[i];		
	} 
	
	for(i = 1; i < 8; i++)
	   y[0] += (y[i] << i);
	
	return GF2_Inverse(y[0]);
	
}

/* ========================= Function���j�u ========================= */

/* S-Box Function */

BYTE S_Box(u_short num1)
{
	BYTE x_inverse;
	BYTE y[8] = {0,0,0,0,0,0,0,0};
	BYTE x[8] = {0,0,0,0,0,0,0,0};
	BYTE z[8] = {1,1,0,0,0,1,1,0};
	BYTE matrix[8][8] = {1,0,0,0,1,1,1,1,
	                              1,1,0,0,0,1,1,1,
						          1,1,1,0,0,0,1,1,
					         	 1,1,1,1,0,0,0,1,
					         	 1,1,1,1,1,0,0,0,
					         	 0,1,1,1,1,1,0,0,
					         	 0,0,1,1,1,1,1,0,
						          0,0,0,1,1,1,1,1};
						 
	int i, j;
	
	x_inverse = GF2_Inverse(num1);
	
	
	for(i = 0; i < 8; i++)
	{
		if((x_inverse % 2) == 1)
		   x[i] = 1;
		
		x_inverse /= 2;   
	}

	
	for(i = 0; i < 8; i++)
	{
		for(j = 0; j < 8; j++)
		   y[i] = y[i] ^ (matrix[i][j] & x[j]);
		
		y[i] = y[i] ^ z[i];		
	} 
	
	for(i = 1; i < 8; i++)
	   y[0] += (y[i] << i);
	
	return y[0];
	
}



/* ========================= Function���j�u ========================= */

/* GF2 Inverse Function */

BYTE GF2_Inverse(u_short num1)
{
	u_short temp[8][2] = {1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0};
	u_short a, n, r, A, B;
	int i, j, k;
	
	n = 283;
	a = num1;
	r = 2;
	
	if(num1 == 1)
	   return 1;
	else if(num1 == 0)
	   return 0;   
	
	/* �}�l����۰��k */
	/* �p��^�X�A�B�l�Ƶ���1���ɭԵ��� */ 
	
	for(i = 1; r > 1; i++){          
		/* �D n = q1 * a + r1 */
		/* �O�o�B�z a = 1 �ɪ��S���p(���έp��) */
		/* ���P�_ n ���̰���� , ���ۦA�N a ���ܸӦ�ƫ� mod*/ 
		
		/* ���D a����� */
		
		for(k = 7; k >= 0; k--){
			if(a >= (1 << k))
			   break;
		}
		
		for(j = 8; j >= k ; j--){
			if(n >= (1 << j)){
				n = n ^ (a << (j - k));
				temp[i][1] = (temp[i][1]) + (1 << (j - k));		
			}
		}
		
		r = n;
		n = a;
		a = r;		
	}
	
	/* �N�Y�ƥN�^�h */
	
	i--;
		
	for(;i > 1; i--){
		temp[0][0] = temp[i][0];   // temp[0][x] ���ƭȪ��Ȧs�� 
		temp[0][1] = temp[i][1];
		temp[i - 1][0] = temp[0][1];
		temp[i - 1][1] = GF2_Multiply(temp[0][1], temp[i - 1][1]) ^ temp[0][0] ;
	}
	
	/*
	if(num1 == 1)
	printf("1 �� inverse �� %x\n\n", temp[1][1]);
	*/
	
	return (BYTE)temp[1][1];   
	      
}

/* ========================= Function���j�u ========================= */

/* �B�z File Header �� Function */

void keyHeader(FILE *input, FILE *keyFile, int *ptrNb, int *ptrNk, int Option){
	
	/*
	   header�榡 : 
	   
	   1 Byte - Encrypt Format(0 : �Ϥ��[�K, 1 : �ɮץ[�K), DataBlock Size, Key Size (1 : 128bits, 2 : 192bits, 3 : 256bits)
	   1 Byte - �̫�@�檺Length 
	   4 Byte - File Block Number (File size / Block Size) 
	   
	   ex. 113 15 00 45 67 89  --> �ĥΤ@���ɮץ[�K, Data Block ��128bits, Key �� 256bits, 
	   �̫�@�� block �� 15 ��ƭ� ,����ɮצ� 456789 �� Block 
	   
	*/
	
	
	BYTE format01 = Option * 100 + *ptrNb * 10 + *ptrNk;    
	unsigned int format02 = 0;
	BYTE format03 = 0;           
	
	fseek(input, 0L, SEEK_END);
	
	
	/* �ഫ��Nb, 128 -> 4, 192 ->6, 256 -> 8 */
	
	*ptrNb = 2 * (*ptrNb + 1);
	
	
	/* �ഫ��Nk, 128 -> 4, 192 ->6, 256 -> 8 */
	
	*ptrNk = 2 * (*ptrNk + 1);
	
	
	/* �`�Ӽ� = �` Byte �� / �@�� block �� Byte �� */ 
	
	format02 = (unsigned int)(ftell(input) / (4 * *ptrNb));    	
	
	/* ���㰣���ɭԭn�[ 1 */
	
	if((ftell(input) % (4 * *ptrNb)) != 0)
	   format02++;
		
	/* �̫�@��Ѿl��Byte�� = �` Byte �� % �@�� Block �� Byte �� */ 
	
	format03 = (BYTE)(ftell(input) % (4 * *ptrNb));                      
	
	
	/* �g�J�[�K��T�� Key �� */
	
	fwrite(&format01, 1, 1, keyFile);
	fwrite(&format03, 1, 1, keyFile);
	fwrite(&format02, 4, 1, keyFile);
	fseek(input, 0L, SEEK_SET);
	
	return ;
}

/* ========================= Function���j�u ========================= */

/* Ū�� File Header �� Information */

int readHeader(FILE *keyFile, int *ptrNb, int *ptrNk, BYTE *ptrA, u_int *blockNumber){
	
	BYTE format01 = 0;
	
	
	/* Ū�� �[�ѱK�榡 */
	
	fread(&format01, 1, 1, keyFile);
	
	/* Ū�� �̫�@�ӳѾl byte �� */
	
	fread(ptrA, 1, 1, keyFile);
	
	/* Ū���` block �� */
	                      
	fread(blockNumber, 4, 1, keyFile);
	
	
	
	*ptrNb = ((format01 % 100) / 10 + 1) * 2;
	*ptrNk = (format01 % 10 + 1) * 2;
	
	return (int)(format01 / 100);
} 

/* ========================= Function���j�u ========================= */

/* Create SBox */ 

void create_Sbox(BYTE box_array[]){
	
	int i;
	
	for(i = 0; i < 256; i++)
	   box_array[i] = S_Box((u_short)i);
	
	
	return ;
}

/* ========================= Function���j�u ========================= */

/* Create SBox */ 

void create_ISbox(BYTE box_array[]){
	
	int i;
	
	for(i = 0; i < 256; i++)
	   box_array[i] = I_SBox((u_short)i);
	
	
	return ;
}                                                                 
