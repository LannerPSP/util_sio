
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include <time.h>
#include <ctype.h>
#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/io.h>
#include <fcntl.h>

#define TYPE_256B	0
#define TYPE_512B	1
#define TYPE_1KB	2
#define TYPE_2KB	3
#define TYPE_4KB	4
#define TYPE_8KB	5
#define TYPE_16KB	6
#define TYPE_32KB	7
#define TYPE_64KB	8
#define TYPE_128KB	9



int ascii_to_hex(char ch) 
{ 
char ch_tmp; 
int hex_val = -1; 

	ch_tmp = tolower(ch); 

	if ((ch_tmp >= '0') && (ch_tmp <= '9')) { 
		hex_val = ch_tmp - '0'; 
	} 
	else if ((ch_tmp >= 'a') && (ch_tmp <= 'f')) { 
		hex_val = ch_tmp - 'a' + 10; 
	} 

	return hex_val; 
} 
/********/
int str_to_hex(char *hex_str) 
{
int i, len; 
int hex_tmp, hex_val; 
char *bptr;
int fHEX=0;
	bptr = strstr(hex_str, "0x");
	if ( bptr != NULL ) {
		bptr+=2;
		fHEX=1;
	}
	else 	bptr=hex_str;

	if (fHEX==1 ){
		len = (int)strlen(bptr); 
		hex_val = 0; 
	 	for (i=0; i<len;i++) { 
			hex_tmp = ascii_to_hex(bptr[i]); 
			if (hex_tmp == -1) { return -1; } 
			hex_val = (hex_val) * 16 + hex_tmp; 
		}
	}
	else {
		hex_val = atoi(bptr);
	} 
	return hex_val; 
} 
void __printf_usage(char *argv0)
{
	printf("Usage: %s -nct6776/-ft81866/-ft81865/-nct6116 \n", argv0);
	printf("			\e[1;34m--> Showing SuperIO LDN information\e[m\n");
	printf("       %s -nct6776/-ft81866/-ft81865/-nct6116 -ldn #num\n", argv0);
	printf("			\e[1;34m--> Showing SuperIO all registers content of LDN\e[m\n");
	printf("       %s -nct6776/-ft81866/-ft81865/-nct6116 -ldn #num -reg #reg #data\n", argv0);
	printf("			\e[1;34m--> Write register data\e[m\n");
	printf("       %s -port2E/-port4E -ldn #num \n", argv0);
	printf("			\e[1;34m--> Showing SuperIO all registers content of LDN\e[m\n");
	printf("       %s -port2E/-port4E -ldn #num -reg #reg #data\n", argv0);
	printf("			\e[1;34m--> Write register data\e[m\n");
	printf("       %s -nct6776/-ft81866/-ft81865/-nct6116 -hwmbank #num \n", argv0);
	printf("			\e[1;34m--> List hwm bank data\e[m\n");
	printf("       %s -nct6776/-ft81866/-ft81865/-nct6116 -hwmset #bank #reg #data\n", argv0);
	printf("			\e[1;34m--> Setting hwm register value\e[m\n");
	printf("	parameter:\n");
	printf("	-ldn #num	: Assign SuperIO Logical Device Number\n");
	printf("	-reg #reg #data	: Assign write register and data\n");
}


void _err_printf(char * pbStirng)
{
	printf("\e[1;31m%s\e[m\n",pbStirng);

}
void __error_exit(char *argv0)
{
	printf("\e[1;31m<Error> comamnd error !!!\e[m\n");
	__printf_usage(argv0);
	exit(-1);
}

uint8_t SIO_INDEX =0;
uint8_t SIO_DATA =0;
uint16_t wHWM_INDEX =0;
uint16_t wHWM_DATA =0;

#define DEV_NCT6776	0
#define DEV_FT81866	1
#define DEV_FT81865	2
#define DEV_NCT6116	3
#define DEV_TOGTAL	DEV_NCT6116+1
#define DEV_SKIP	0xFF

#define SEL_LDNLIST	1
#define SEL_LDNSHOW	2
#define SEL_WRITEREG	3
#define SEL_HWMRD	4
#define SEL_HWMWR	5

char dev_name[DEV_TOGTAL][30]= {"NCT-6776", "FT-81866", "FT-81865", "NCT-6116"};
uint16_t DEV_VENDOR[DEV_TOGTAL]={0x5ca3, 0x1934, 0x1934, 0x5ca3};
uint16_t DEV_CHIPID[DEV_TOGTAL]={0x00C1, 0x1010, 0x0704, 0x00C1};

typedef struct DEF_SIO_LDNLIST {
	int	iLDN;
	char	strLdnName[30];

}SIO_LDNLIST;

SIO_LDNLIST ldnlis[DEV_TOGTAL][0x20]= {
{ //NCT-6776
{0x00, "FDC"},
{0x01, "Parallel Port"},
{0x02, "UART A"},
{0x03, "UART B, IR"},
{0x05, "Keyboard Controller"},
{0x06, "CIR"},
{0x07, "GPIO6,7,8,9"},
{0x08, "WDT1, GPIO0,1,A"},
{0x09, "GPIO2,3,4,5"},
{0x0A, "ACPI"},
{0x0B, "Hardware Monitor, Front LED"},
{0x0D, "VID"},
{0x0E, "CIR Wake-Up"},
{0x0F, "GPIO Puah-Pull, Open-Drain"},
{0x14, "SVID"},
{0x16, "Deep Sleep"},
{0x17, "GPIOA"},
{0xFF, "-END-"} },
{ //FT-81866 
{0x00, "FDC"},
{0x03, "Parallel Port"},
{0x04, "Hardware Monitor"},
{0x05, "Keyboard Controller"},
{0x06, "GPIO"},
{0x07, "WDT"},
{0x0A, "PME and ACPI"},
{0x10, "UART1"},
{0x11, "UART2"},
{0x12, "UART3"},
{0x13, "UART4"},
{0x14, "UART5"},
{0x15, "UART6"},
{0xFF, "-END-"} },
{ //FT-81865
{0x00, "FDC"},
{0x03, "Parallel Port"},
{0x04, "Hardware Monitor"},
{0x05, "Keyboard Controller"},
{0x06, "GPIO"},
{0x07, "WDT"},
{0x08, "SPI"},
{0x0A, "PME and ACPI"},
{0x0B, "RTC"},
{0x10, "UART1"},
{0x11, "UART2"},
{0x12, "UART3"},
{0x13, "UART4"},
{0x14, "UART5"},
{0x15, "UART6"},
{0xFF, "-END-"} },
{ //NCT-6116
{0x01, "Parallel Port"},
{0x02, "UART A"},
{0x03, "UART B"},
{0x05, "Keyboard Controller"},
{0x06, "CIR"},
{0x07, "GPIO0,1,2,3,4,5,6,7"},
{0x08, "GPIO, WDT1"},
{0x09, "GPIO8"},
{0x0A, "ACPI"},
{0x0B, "Hardware Monitor, LED"},
{0x0D, "WDT2"},
{0x0E, "CIR Wake-Up"},
{0x0F, "GPIO Push-Pull, Open-Drain"},
{0x10, "UARTC"},
{0x11, "UARTD"},
{0x12, "UARTE"},
{0x13, "UARTF"},
{0x14, "PORT80, IR"},
{0x15, "FADING LED"},
{0x16, "Deep Sleep"},
{0xFF, "-END-"} },
};


int __check_hardware(int iDevice);

/********************************************************************/
/***** SuperIO access functions *************************************/
void __sio_unlock(void)
{
	ioperm(SIO_INDEX, 2, 1);
	outb(0x87 , SIO_INDEX);
	outb(0x87 , SIO_INDEX);
}
/***********/
void __sio_lock(void)
{
	outb(0xaa , SIO_INDEX);
	ioperm(SIO_INDEX, 2, 0);
}
/***********/
void __sio_logic_device(char num)
{
	outb(0x7 , SIO_INDEX);
	outb(num , SIO_DATA);
}
/************/
uint8_t read_sio_reg(uint8_t LDN, uint8_t reg)
{
        outb(0x07, SIO_INDEX); //LDN register
        outb(LDN, SIO_DATA);
        outb(reg , SIO_INDEX);
        return inb(SIO_DATA);
}
/************/
uint8_t write_sio_reg(uint8_t LDN, uint8_t reg, uint8_t value)
{	
        outb(0x07, SIO_INDEX); //LDN register
        outb(LDN, SIO_DATA);
        outb(reg, SIO_INDEX);
        outb(value, SIO_DATA);
        return 0;
}


int main(int argc, char **argv) 
{
int iRet =0, xi,xj, iDevice=-1, iLDN=-1, iSelect=SEL_LDNLIST, iIndex=-1;
int dwData=0x00;
uint8_t ubReg=0xFF, ubData, ubTemp, ubBank;
int fRegInput=0, fSkip=0;
int iHwmBank;

	if ( getuid() != 0 ) {
		_err_printf("<Warning> Please uses root user !!!");
		return -1;
	}
	for ( xi= 1; xi< argc ; xi++ ) {
		if 	( strcmp("-nct6776", argv[xi]) == 0 ) iDevice = DEV_NCT6776;
		else if ( strcmp("-ft81866", argv[xi]) == 0 ) iDevice = DEV_FT81866;
		else if ( strcmp("-ft81865", argv[xi]) == 0 ) iDevice = DEV_FT81865;
		else if ( strcmp("-nct6116", argv[xi]) == 0 ) iDevice = DEV_NCT6116;
		else if ( strcmp("-ldnlist", argv[xi]) == 0 ) iSelect = SEL_LDNLIST;
		else if ( strcmp("-port2E", argv[xi]) == 0 ) {SIO_INDEX = 0x2E; SIO_DATA=0x2F; fSkip=1; }
		else if ( strcmp("-port4E", argv[xi]) == 0 ) {SIO_INDEX = 0x4E; SIO_DATA=0x4F; fSkip=1; }
		else if ( strcmp("-hwmbank", argv[xi]) == 0 ) {
			iSelect = SEL_HWMRD;
			if ( xi == argc-1 ) {
				_err_printf("<Warning> not input HWM base address !");
				return -1;
			}
			iHwmBank =  str_to_hex(argv[xi+1]);
			xi++;
		}
		else if ( strcmp("-hwmset", argv[xi]) == 0 ) {
			//#bank #reg #data
			iSelect = SEL_HWMWR;
			if ( xi == argc-3 ) {
				_err_printf("<Warning> not enough input");
				return -1;
			}
			dwData = str_to_hex(argv[xi+1]);
			if ( dwData > 7 ) {
				_err_printf("<Warning> bank out of range ( 0 ~ 7)");
				__printf_usage(argv[0]);
				return -1;
			}
			ubBank = (uint8_t)(dwData & 0xFF);

			dwData = str_to_hex(argv[xi+2]);
			if ( dwData > 256 ) {
				_err_printf("<Warning> register out of range ( 0 ~ 255)");
				__printf_usage(argv[0]);
				return -1;
			}
			ubReg = (uint8_t)(dwData & 0xFF);
			//----
			dwData = str_to_hex(argv[xi+3]);
			if ( dwData > 256 ) {
				_err_printf("<Warning> data out of range ( 0 ~ 255)");
				__printf_usage(argv[0]);
				return -1;
			}
			ubData = (uint8_t)(dwData & 0xFF);
			//fRegInput =1;
			xi +=3 ;
			
		}
		else if ( strcmp("-ldn", argv[xi]) == 0 ) { //logic device number
			iSelect = SEL_LDNSHOW;
			if ( xi == argc-1 ) {
				_err_printf("<Warning> not input LDN (Logical Device Number) !");
				return -1;
			}
			iLDN =  str_to_hex(argv[xi+1]);
			xi++;
		}
		else if ( strcmp("-reg", argv[xi]) == 0 ) { //logic device number
			if ( xi == argc-2 ) {
				_err_printf("<Warning> not enough input");
				return -1;
			}
			dwData = str_to_hex(argv[xi+1]);
			if ( dwData > 256 ) {
				_err_printf("<Warning> out of range ( 0 ~ 255)");
				__printf_usage(argv[0]);
				return -1;
			}
			ubReg = (uint8_t)(dwData & 0xFF);
			//----
			dwData = str_to_hex(argv[xi+2]);
			if ( dwData > 256 ) {
				_err_printf("<Warning> out of range ( 0 ~ 255)");
				__printf_usage(argv[0]);
				return -1;
			}
			ubData = (uint8_t)(dwData & 0xFF);
			fRegInput =1;
			xi +=2 ;
			
		}
		else {
			_err_printf("<Warning> invalid command or parameter input");
			__printf_usage(argv[0]);
			return -1;
		}
	}

	if ( iDevice==-1 && fSkip==0 ) { //if no assigned device
		__printf_usage(argv[0]);
		return -1;
	}

	if ( fRegInput==1 && iLDN!=-1)  iSelect = SEL_WRITEREG;

	if ( SIO_INDEX == 0x00 ) {
		iRet = __check_hardware(iDevice);
		if ( !iRet && iSelect!= SEL_LDNLIST) { 
			_err_printf("<Error> Not found SuperIO !!!\n");
			return -1;
		}
	}
	/*****************************************************************/
	switch (iSelect) {
	case SEL_LDNLIST:
		if ( fSkip ) { 
			_err_printf("<Error> Assign port not support LDN listing !!!\n");
			return -1;
		}
		//show SIO LDN information
		for ( xi=0 ;xi< 0x20 ; xi++) {
			if ( ldnlis[iDevice][xi].iLDN != 0xFF ) {
				printf("LDN# = 0x%02X, Name = %s\n", ldnlis[iDevice][xi].iLDN, ldnlis[iDevice][xi].strLdnName);
			}
			else {
				xi=0x100;//exit
			}
		}
		
		break;
	case SEL_LDNSHOW:
		if ( !fSkip ) {
			//checking LDN Register Data valid ?
			for ( xi=0 ;xi< 0x20 ; xi++) {
				if ( ldnlis[iDevice][xi].iLDN == iLDN ) { iIndex=xi; xi=0x100;}
			}
			if ( iIndex == -1 ) {
				_err_printf("<Error> This SuperIO not supported the LDN !!!");
				return -1;
			}
			printf("---> Device: %s, LDN=0x%02X, Name=%s\n", dev_name[iDevice], iLDN, ldnlis[iDevice][iIndex].strLdnName);
		}
		printf("===========================================================\n");
		printf("\e[1;36m---------: 00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F\e[m\n");
		for (xi=0 ; xi<16 ; xi++ ) {
			if ( xi==3) printf("\e[1;36m---------: 00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F\e[m\n");
			printf("Reg.0x%02X : ", xi*16);
			for ( xj=0; xj<16; xj++ ) {
				__sio_unlock();		//move here for invalid Reg. access wail auto lock SIO
				__sio_logic_device(iLDN);
				ubReg = (uint8_t)((xi*16+xj) & 0xFF);
				outb(ubReg, SIO_INDEX);
				ubData = inb(SIO_DATA);
				printf("%02X ", ubData);
			}
			printf("\n");
		}
		__sio_lock();
		break;
	case SEL_WRITEREG:
		if ( !fSkip ) {
			//checking LDN Register Data valid ?
			for ( xi=0 ;xi< 0x20 ; xi++) {
				if ( ldnlis[iDevice][xi].iLDN == iLDN ) { iIndex=xi; xi=0x100;}
			}
			if ( iIndex == -1 ) {
				_err_printf("<Error> This SuperIO not supported the LDN !!!");
				return -1;
			}
			
			printf("---> Device: %s, LDN=0x%02X, Name=%s\n", dev_name[iDevice], iLDN, ldnlis[iDevice][iIndex].strLdnName);
		}
		printf("                 Reg=0x%02X, Data=0x%02X\n", ubReg, ubData);
		//===========
		__sio_unlock();		
		__sio_logic_device(iLDN);
		outb(ubReg, SIO_INDEX);
		outb(ubData, SIO_DATA);
		usleep(100000); //wait 10ms

		outb(ubReg, SIO_INDEX);
		ubTemp = inb(SIO_DATA);
		__sio_lock();
		if ( ubTemp == ubData ) printf("--------> Write OK \n");
		else 			_err_printf("<Error> Write/Read nor equal value !!!");
		
		break;
	case SEL_HWMRD:
		printf("===========================================================\n");
		printf("\e[1;36m---------: 00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F\e[m\n");
		ioperm(wHWM_INDEX, 2, 1);
		outb(0x4E, wHWM_INDEX);
		outb(iHwmBank, wHWM_DATA);
		for (xi=0 ; xi<16 ; xi++ ) {
			if ( xi==3) printf("\e[1;36m---------: 00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F\e[m\n");
			printf("Reg.0x%02X : ", xi*16);
			for ( xj=0; xj<16; xj++ ) {
				ubReg = (uint8_t)((xi*16+xj) & 0xFF);
				outb(ubReg, wHWM_INDEX);
				ubData = inb(wHWM_DATA);
				printf("%02X ", ubData);
			}
			printf("\n");
		}
		ioperm(wHWM_INDEX, 2, 0);
		break;
	case SEL_HWMWR:
		ioperm(wHWM_INDEX, 2, 1);
		outb(0x4E, wHWM_INDEX);
		outb(ubBank, wHWM_DATA);
		outb(ubReg, wHWM_INDEX);
		outb(ubData, wHWM_DATA);
		ioperm(wHWM_INDEX, 2, 0);
		break;
	default:
		__error_exit(argv[0]);
		break;
	}

	return iRet;
}


/********************************************/
int __check_hardware(int iDevice)
{
uint8_t xch;
uint8_t xData[4];
uint16_t wVendor, wChipID;
	if ( SIO_INDEX == 0 ) {
		SIO_INDEX = 0x4e;
		SIO_DATA = SIO_INDEX +1;
		__sio_unlock();
		outb(0x02, SIO_INDEX);
		xch = inb(SIO_DATA);
		__sio_lock();
		if ( xch == 0xFF ) {
			SIO_INDEX = 0x2e;
			SIO_DATA = SIO_INDEX +1;
			__sio_unlock();
			outb(0x02, SIO_INDEX);
			xch = inb(SIO_DATA);
			__sio_lock();
			if ( xch == 0xFF ) return 0; //not found
		}
			
	}
	switch (iDevice) {
	case DEV_NCT6776:
		__sio_unlock();
		__sio_logic_device(0x0B); //NCT-6776 HWM 
		outb(0x30, SIO_INDEX);
		xch = inb(SIO_DATA);
		if ( !(xch & 0x01) ) return 0;
		outb(0x60, SIO_INDEX);
		xData[1] = inb(SIO_DATA);
		outb(0x61, SIO_INDEX);
		xData[0] = inb(SIO_DATA);
		__sio_lock();
	//printf("HWM Address=0x%04X\n", *(uint16_t*)xData);
		wHWM_INDEX=(*(uint16_t*)xData) + 0x05;
		wHWM_DATA=(*(uint16_t*)xData) + 0x06;
	//printf("HWM_INDEX=0x%04X\n", wHWM_INDEX);
		ioperm(wHWM_INDEX, 2, 1);
		outb(0x4E, wHWM_INDEX);
		outb(0x80, wHWM_DATA);	//bank 0, high
		outb(0x4F, wHWM_INDEX);
		xData[1] = inb(wHWM_DATA);
		outb(0x4E, wHWM_INDEX);
		outb(0x00, wHWM_DATA);	//bank 0, Low
		outb(0x4F, wHWM_INDEX);
		xData[0] = inb(wHWM_DATA);
		wVendor = *(uint16_t*)xData;
		outb(0x58, wHWM_INDEX);
		xData[0] = inb(wHWM_DATA);
		xData[1] = 0x00;
		wChipID = *(uint16_t*)xData;
	//printf("Vendor=0x%04X\n",wVendor);
	//printf("Chip ID=0x%04X\n",wChipID);
		ioperm(wHWM_INDEX, 2, 0);
		break;
	case DEV_FT81866:
	case DEV_FT81865:
		__sio_unlock();
		ioperm(wHWM_INDEX, 2, 1);
		outb(0x23, SIO_INDEX);
		xData[1] = inb(SIO_DATA);
		outb(0x24, SIO_INDEX);
		xData[0] = inb(SIO_DATA);
		wVendor = *(uint16_t*)xData;
		outb(0x20, SIO_INDEX);
		xData[1] = inb(SIO_DATA);
		outb(0x21, SIO_INDEX);
		xData[0] = inb(SIO_DATA);
		wChipID = *(uint16_t*)xData;	
	//printf("Vendor=0x%04X\n",wVendor);
	//printf("Chip ID=0x%04X\n",wChipID);
		__sio_lock();
		break;
	case DEV_NCT6116:
		__sio_unlock();
		__sio_logic_device(0x0B); //NCT-6776 HWM 
		outb(0x30, SIO_INDEX);
		xch = inb(SIO_DATA);
		if ( !(xch & 0x01) ) return 0;
		outb(0x60, SIO_INDEX);
		xData[1] = inb(SIO_DATA);
		outb(0x61, SIO_INDEX);
		xData[0] = inb(SIO_DATA);
		__sio_lock();
	//printf("HWM Address=0x%04X\n", *(uint16_t*)xData);
		wHWM_INDEX=(*(uint16_t*)xData) + 0x05;
		wHWM_DATA=(*(uint16_t*)xData) + 0x06;
	//printf("HWM_INDEX=0x%04X\n", wHWM_INDEX);
		ioperm(wHWM_INDEX, 2, 1);
		outb(0x4E, wHWM_INDEX);
		outb(0x80, wHWM_DATA);	//bank 0, high
		outb(0xFE, wHWM_INDEX);
		xData[1] = inb(wHWM_DATA);
		outb(0x4E, wHWM_INDEX);
		outb(0x00, wHWM_DATA);	//bank 0, Low
		outb(0xFE, wHWM_INDEX);
		xData[0] = inb(wHWM_DATA);
		wVendor = *(uint16_t*)xData;
		outb(0xFF, wHWM_INDEX);
		xData[0] = inb(wHWM_DATA);
		xData[1] = 0x00;
		wChipID = *(uint16_t*)xData;
	//printf("Vendor=0x%04X\n",wVendor);
	//printf("Chip ID=0x%04X\n",wChipID);
		ioperm(wHWM_INDEX, 2, 0);
		break;
	default:
		return 0;
		break;
	}
	if ( wVendor != DEV_VENDOR[iDevice] || wChipID != DEV_CHIPID[iDevice] ) return 0; //not match
	return 1;


}
