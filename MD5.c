#include <stdio.h>
#include <math.h>

void GetLen(char* filename);
void GetData(char* filename);
long ByteData = 0;

void GetMD5(char * Data);
unsigned char md5[16] = { 0x01,0x23,0x45,0x67,0x89,0xab,0xcd,0xef,0xfe,0xdc,0xba,0x98,0x76,0x54,0x32,0x10 };
unsigned int* pMd5 = (unsigned int*)md5;

void CS64_Init(void);
unsigned int CS_64[64] = { 0 };
unsigned char CS_16[16] = { 7,12,17,22,5,9,14,20,4,11,16,23,6,10,15,21 };

unsigned int F(unsigned int x, unsigned int y, unsigned int z);
unsigned int G(unsigned int x, unsigned int y, unsigned int z);
unsigned int H(unsigned int x, unsigned int y, unsigned int z);
unsigned int I(unsigned int x, unsigned int y, unsigned int z);
unsigned int FF(unsigned int a, unsigned int b, unsigned int c, unsigned int d, unsigned int data, unsigned int cs16, unsigned int cs64);
unsigned int GG(unsigned int a, unsigned int b, unsigned int c, unsigned int d, unsigned int data, unsigned int cs16, unsigned int cs64);
unsigned int HH(unsigned int a, unsigned int b, unsigned int c, unsigned int d, unsigned int data, unsigned int cs16, unsigned int cs64);
unsigned int II(unsigned int a, unsigned int b, unsigned int c, unsigned int d, unsigned int data, unsigned int cs16, unsigned int cs64);


void Save(char* filename);


int main(int argc,char* argv[]) 
{
	GetLen(argv[1]);
	//printf("%d",ByteData);
	CS64_Init();
	GetData(argv[1]);
	Save(argv[2]);

	return 0;
}

void GetLen(char* filename) 
{
	//打开文件
	FILE* pFile = NULL;
	errno_t res = fopen_s(&pFile, filename,"r");
	if (0 != res || pFile == NULL) 
	{
		return;
	}
	//设置文件指针位置
	fseek(pFile,0L,SEEK_END);
	//返回当前位置即字节数
	ByteData = ftell(pFile);
	//关闭文件
	fclose(pFile);
}


void GetData(char* filename) 
{
	int man = ByteData / 64;
	int final = ByteData % 64;
	//打开文件
	FILE* pFile = NULL;
	errno_t res = fopen_s(&pFile, filename, "r");
	if (0 != res || pFile == NULL)
	{
		return;
	}
	//满数据处理
	////获取满数据，MD5计算
	for (int i=0;i<man;i++) 
	{
		char buff[64] = {0};
		fread(buff,1,64,pFile);
		//MD5计算
		GetMD5(buff);
	}
	//未满数据处理
	////小于56字节
	if (56>=final&& final >0)
	{
		//读数据
		char buff[64] = { 0 };
		int count =fread(buff, 1, 64, pFile);
		//最后8字节赋值
		*(unsigned long long*)(buff + 56) = ByteData * 8;
		//补1000 0000
		if (count < 56) 
		{
			buff[count] = 0x80;
			for (int i=count+1;i<56;i++) 
			{
				buff[i] = 0;
			}
		}
		//MD5计算
		GetMD5(buff);
	}
	else if (0==final)
	{
		//读数据
		char buff[64] = { 0 };
		*(unsigned long long*)(buff + 56) = ByteData * 8;
		buff[0] = 0x80;
		for (int i=1;i<56;i++) 
		{
			buff[i] = 0;
		}
		//MD5计算
		GetMD5(buff);
	}
	else if (56 < final && final < 64)
	{
		//读数据
		char buff[64] = { 0 };
		int count = fread(buff, 1, 64, pFile);
		buff[count] = 0x80;
		for (int i = count + 1; i < 64; i++) 
		{
			buff[i] = 0;
		}

		//MD5计算
		GetMD5(buff);
		char Newbuff[64] = {0};
		*(unsigned long long*)(Newbuff + 56) = ByteData * 8;
		Newbuff[0] = 0x80;
		for (int i = 1; i < 56; i++) 
		{
			Newbuff[i]=0;
		}

		//MD5计算
		GetMD5(Newbuff);
	}

	fclose(pFile);

}

void GetMD5(char * Data) 
{
	unsigned int* pData=(unsigned int*)Data;
	unsigned int a = pMd5[0];
	unsigned int b = pMd5[1];
	unsigned int c = pMd5[2];
	unsigned int d = pMd5[3];
//	printf("%x,%x,%x,%x\n", a,b,c,d);
//	for(int j=0;j<16;j++)
//		printf("%d\n", CS_16[j]);
	//第一轮计算
	a = FF(a, b, c, d, pData[0], CS_16[0], CS_64[0]);
	d = FF(d, a, b, c, pData[1], CS_16[1], CS_64[1]);
	c = FF(c, d, a, b, pData[2], CS_16[2], CS_64[2]);
	b = FF(b, c, d, a, pData[3], CS_16[3], CS_64[3]);
	a = FF(a, b, c, d, pData[4], CS_16[0], CS_64[4]);
	d = FF(d, a, b, c, pData[5], CS_16[1], CS_64[5]);
	c = FF(c, d, a, b, pData[6], CS_16[2], CS_64[6]);
	b = FF(b, c, d, a, pData[7], CS_16[3], CS_64[7]);
	a = FF(a, b, c, d, pData[8], CS_16[0], CS_64[8]);
	d = FF(d, a, b, c, pData[9], CS_16[1], CS_64[9]);
	c = FF(c, d, a, b, pData[10], CS_16[2], CS_64[10]);
	b = FF(b, c, d, a, pData[11], CS_16[3], CS_64[11]);
	a = FF(a, b, c, d, pData[12], CS_16[0], CS_64[12]);
	d = FF(d, a, b, c, pData[13], CS_16[1], CS_64[13]);
	c = FF(c, d, a, b, pData[14], CS_16[2], CS_64[14]);
	b = FF(b, c, d, a, pData[15], CS_16[3], CS_64[15]);


	//第二轮计算
	a = GG(a, b, c, d, pData[1], CS_16[4], CS_64[16]);
	d = GG(d, a, b, c, pData[6], CS_16[5], CS_64[17]);
	c = GG(c, d, a, b, pData[11], CS_16[6], CS_64[18]);
	b = GG(b, c, d, a, pData[0], CS_16[7], CS_64[19]);
	a = GG(a, b, c, d, pData[5], CS_16[4], CS_64[20]);
	d = GG(d, a, b, c, pData[10], CS_16[5], CS_64[21]);
	c = GG(c, d, a, b, pData[15], CS_16[6], CS_64[22]);
	b = GG(b, c, d, a, pData[4], CS_16[7], CS_64[23]);
	a = GG(a, b, c, d, pData[9], CS_16[4], CS_64[24]);
	d = GG(d, a, b, c, pData[14], CS_16[5], CS_64[25]);
	c = GG(c, d, a, b, pData[3], CS_16[6], CS_64[26]);
	b = GG(b, c, d, a, pData[8], CS_16[7], CS_64[27]);
	a = GG(a, b, c, d, pData[13], CS_16[4], CS_64[28]);
	d = GG(d, a, b, c, pData[2], CS_16[5], CS_64[29]);
	c = GG(c, d, a, b, pData[7], CS_16[6], CS_64[30]);
	b = GG(b, c, d, a, pData[12], CS_16[7], CS_64[31]);


	//第三轮计算
	a = HH(a, b, c, d, pData[5], CS_16[8], CS_64[32]);
	d = HH(d, a, b, c, pData[8], CS_16[9], CS_64[33]);
	c = HH(c, d, a, b, pData[11], CS_16[10], CS_64[34]);
	b = HH(b, c, d, a, pData[14], CS_16[11], CS_64[35]);
	a = HH(a, b, c, d, pData[1], CS_16[8], CS_64[36]);
	d = HH(d, a, b, c, pData[4], CS_16[9], CS_64[37]);
	c = HH(c, d, a, b, pData[7], CS_16[10], CS_64[38]);
	b = HH(b, c, d, a, pData[10], CS_16[11], CS_64[39]);
	a = HH(a, b, c, d, pData[13], CS_16[8], CS_64[40]);
	d = HH(d, a, b, c, pData[0], CS_16[9], CS_64[41]);
	c = HH(c, d, a, b, pData[3], CS_16[10], CS_64[42]);
	b = HH(b, c, d, a, pData[6], CS_16[11], CS_64[43]);
	a = HH(a, b, c, d, pData[9], CS_16[8], CS_64[44]);
	d = HH(d, a, b, c, pData[12], CS_16[9], CS_64[45]);
	c = HH(c, d, a, b, pData[15], CS_16[10], CS_64[46]);
	b = HH(b, c, d, a, pData[2], CS_16[11], CS_64[47]);



	//第四轮计算
	a = II(a, b, c, d, pData[0], CS_16[12], CS_64[48]);
	d = II(d, a, b, c, pData[7], CS_16[13], CS_64[49]);
	c = II(c, d, a, b, pData[14], CS_16[14], CS_64[50]);
	b = II(b, c, d, a, pData[5], CS_16[15], CS_64[51]);
	a = II(a, b, c, d, pData[12], CS_16[12], CS_64[52]);
	d = II(d, a, b, c, pData[3], CS_16[13], CS_64[53]);
	c = II(c, d, a, b, pData[10], CS_16[14], CS_64[54]);
	b = II(b, c, d, a, pData[1], CS_16[15], CS_64[55]);
	a = II(a, b, c, d, pData[8], CS_16[12], CS_64[56]);
	d = II(d, a, b, c, pData[15], CS_16[13], CS_64[57]);
	c = II(c, d, a, b, pData[6], CS_16[14], CS_64[58]);
	b = II(b, c, d, a, pData[13], CS_16[15], CS_64[59]);
	a = II(a, b, c, d, pData[4], CS_16[12], CS_64[60]);
	d = II(d, a, b, c, pData[11], CS_16[13], CS_64[61]);
	c = II(c, d, a, b, pData[2], CS_16[14], CS_64[62]);
	b = II(b, c, d, a, pData[9], CS_16[15], CS_64[63]);

	pMd5[0] += a;
	pMd5[1] += b;
	pMd5[2] += c;
	pMd5[3] += d;
}

void CS64_Init(void)
{
	//4294967296乘abs(sin(i))后的值的整数部分
	for (int i = 1; i <= 64; i++) {
	CS_64[i - 1] = (unsigned int)(4294967296 * fabs(sin(i)));
//	printf("%x\n", CS_64[i - 1]);
	}
}

unsigned int F(unsigned int x, unsigned int y, unsigned int z)
{
	return (x & y) | ((~x) & z);
}
unsigned int G(unsigned int x, unsigned int y, unsigned int z)
{
	return (x & z) | (y & (~z));
}
unsigned int H(unsigned int x, unsigned int y, unsigned int z)
{
	return x ^ y ^ z;
}
unsigned int I(unsigned int x, unsigned int y, unsigned int z)
{
	return y ^ (x | (~z));
}

unsigned int FF(unsigned int a, unsigned int b, unsigned int c, unsigned int d, unsigned int data, unsigned int cs16, unsigned int cs64)
{

	unsigned int md5_Temp = a + F(b, c, d) + data + cs64;
	md5_Temp = (md5_Temp << cs16) | (md5_Temp >> (32 - cs16));
	a = b + md5_Temp;
	return a;
}

unsigned int GG(unsigned int a, unsigned int b, unsigned int c, unsigned int d, unsigned int data, unsigned int cs16, unsigned int cs64)
{

	unsigned int md5_Temp = a + G(b, c, d) + data + cs64;
	md5_Temp = (md5_Temp << cs16) | (md5_Temp >> (32 - cs16));
	a = b + md5_Temp;
	return a;
}

unsigned int HH(unsigned int a, unsigned int b, unsigned int c, unsigned int d, unsigned int data, unsigned int cs16, unsigned int cs64)
{

	unsigned int md5_Temp = a + H(b, c, d) + data + cs64;
	md5_Temp = (md5_Temp << cs16) | (md5_Temp >> (32 - cs16));
	a = b + md5_Temp;
	return a;
}

unsigned int II(unsigned int a, unsigned int b, unsigned int c, unsigned int d, unsigned int data, unsigned int cs16, unsigned int cs64)
{

	unsigned int md5_Temp = a + I(b, c, d) + data + cs64;
	md5_Temp = (md5_Temp << cs16) | (md5_Temp >> (32 - cs16));
	a = b + md5_Temp;
	return a;
}


void Save(char* filename) 
{
	//打开文件
	FILE* pFile = NULL;
	errno_t res = fopen_s(&pFile, filename, "w");
	if (0 != res || pFile == NULL)
	{
		return;
	}
	for (int i = 0; i < 16; i++)
	{
		if (md5[i] <= 0x0f)
		{
		fprintf(pFile, "0%x ", md5[i]);
		printf("0%x ", md5[i]);
		}
		else
		{
			fprintf(pFile, "%x ", md5[i]);
			printf("%x ", md5[i]);
		}

	}
	//关闭文件
	fclose(pFile);
}