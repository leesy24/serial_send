#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>
#include "typedefs.h"
#include "serial.h"

#define MAX_DATA_LENGTH 256


/* Convert C from hexadecimal character to integer.  */
static int hextobin (unsigned char c)
{
	switch (c)
	{
		default: return c - '0';
		case 'a': case 'A': return 10;
		case 'b': case 'B': return 11;
		case 'c': case 'C': return 12;
		case 'd': case 'D': return 13;
		case 'e': case 'E': return 14;
		case 'f': case 'F': return 15;
	}
}

int main(int argc, char* argv[])
{
	//If only 0 or 1 parameter is entered, prompt for the missing parameter(s)
	if(argc < 3)
	{
		printf("serial_send v1.0\n");
		printf("\n");
		printf("MISSING A PARAMETER. Enter both PORT and data string.\n");
		printf("\n");
		printf("Data string can be ....\n");
		printf("\\\\\n");
		printf("\tbackslash\n");
		printf("\\xHH\n");
		printf("\tbyte with hexadecimal value HH (1 to 2 digits)\n");
		printf("\\dDDD\n");
		printf("\tbyte with decimal value DDD (1 to 3 digits)\n");
		printf("\\0NNN\n");
		printf("\tbyte with octal value NNN (1 to 3 digits)\n");
		printf("\n");
		printf("Examples\n");
		printf("\tserial_send /dev/ttyUSB0 \"\\x1B\\x43\"\n");
		printf("\tserial_send /dev/ttyUSB0 \"\\x1B\\x4C\\x00\\x0012345678901234567890\"\n");
		printf("\tserial_send /dev/ttyUSB0 \"\\x1B\\x4C\\x0E\\x01abcdefg\"\n");
		printf("\tserial_send /dev/ttyUSB0 \"\\x1B\\x4C\\d014\\d001abcdefg\"\n");
		printf("\n");
		return(-1);
	}

	int baud;
	//default the baud to 115200
	baud=115200;

	//printf("a1:%s\n", argv[1]);
	//printf("a2:%s\n", argv[2]);

	char const *s = argv[2];
	unsigned char c;
	ubyte data[MAX_DATA_LENGTH];
	int index = 0;

	while ((c = *s++))
	{
		if (c == '\\' && *s)
		{
			switch (c = *s++)
			{
				case 'x':
					if (! isxdigit (*s))
						goto not_an_escape;
					c = hextobin (*s++);
					if (isxdigit (*s))
						c = c * 16 + hextobin (*s++);
					break;
				case 'd':
					if (! isdigit (*s))
						goto not_an_escape;
					c = *s++ - '0';
					if (isdigit (*s))
						c = c * 10 + (*s++ - '0');
					if (isdigit (*s))
						c = c * 10 + (*s++ - '0');
					break;
				case '0':
					c = 0;
					if (! ('0' <= *s && *s <= '7'))
						break;
					c = *s++;
				/* Fall through.  */
				case '1': case '2': case '3':
				case '4': case '5': case '6': case '7':
					c -= '0';
					if ('0' <= *s && *s <= '7')
						c = c * 8 + (*s++ - '0');
					if ('0' <= *s && *s <= '7')
						c = c * 8 + (*s++ - '0');
					break;
				case '\\':
					break;

				not_an_escape:
				default:
					if (index < MAX_DATA_LENGTH) // data
					{
						//printf("%d:0x%02x,%3d,%c\n", index, '\\', '\\', '\\');
						data[index++] = '\\';
					}
					break;
			}
		}
		if (index < MAX_DATA_LENGTH) // data
		{
			//printf("%d:0x%02x,%3d,%c\n", index, c, c, c);
			data[index++] = c;
		}
	}

	if(Serial_Init(argv[1],baud))
	{
		printf("serial_send: Could not open port \"%s\" at \"%d\" baud.\n",argv[1],baud);
		return(-1);
	}
	//else
	//	printf("\"%s\" opened at \"%d\" baud.\n\n",argv[1],baud);

	//For some reason, Linux seems to buffer up data from the serial, and they are sometimes
	//dumped at the start of the program. Clear the serial buffer.
	while(BytesAvail())
		GetByte();

	SendData((ubyte *)&data,index);

	Uninit_Serial();

	return 0;
}
//============================================================================
