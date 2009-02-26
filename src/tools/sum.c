#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main( int argc, char *argv[])
{
	char *name = NULL;
	FILE *in;
	int mod = 0;

	if (argc > 1)
	{
		name = argv[1];
		if (argc > 2)
		{
			if (!strcmp(argv[2],"mod"))
				mod = 1;
		}
	}
	in = fopen( name, "rb+");
	if (in)
	{
		long size, i;
		unsigned char *buf;
		
		fseek( in, 0, SEEK_END);
		size = ftell( in);
		printf( "file size is %ld\n", size);
		rewind( in);
		buf = malloc( size);
		if (buf)
		{
			int sum = 0;
			char sizeread, sizecomp;

			fread( buf, size, 1, in);
			if ((buf[0] != 0x55) || (buf[1] != 0xAA))
			{
				printf( "file is not a ROM extension !\n");
				exit( 42);
			}
			sizeread = buf[2];
			printf( "stored size=%d\n", sizeread);
			sizecomp = size / 512;
			if (size % 512)
				size++;
			printf( "computed size=%d : ", sizecomp);
			for (i = 0; i < size; i++)
			{
				unsigned char c;

				c = buf[i];
				sum += c;
			}
			printf( "computed sum=%04x : ", sum);
			if (sum % 256)
			{
				unsigned char pad;

//				pad = sum & 0xFF;
				pad = -sum;
				printf( "mismatch should be %04X\n", (unsigned short)pad);
				if (mod)
				{
					rewind(in);
					buf[2] = sizecomp;
//					buf[size - 2] = 0;
//					buf[size - 1] = pad;
					printf( "moded\n");
					fwrite(buf,size,1,in);
					int padsize = sizecomp * 512 - size - 2;
					if (padsize > 0)
					{
						char *buf2 = malloc( padsize);
						
						printf( "padding %d bytes\n", padsize);
						memset( buf2, 0x00, padsize);
						fwrite( buf2, padsize, 1, in);
						free( buf2);
					}
//					unsigned short w = pad;
//					fwrite( &w, sizeof( w), 1, in);
					unsigned char b = 0;
					fwrite( &b, sizeof( b), 1, in);
					fwrite( &pad, sizeof( pad), 1, in);
				}
			}
			else
			{
				printf( "match\n");
			}
			free( buf);
		}
		fclose( in);
	}

	return 0;
}

