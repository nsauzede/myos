#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main( int argc, char *argv[])
{
	char *name = NULL, *name2 = NULL;
	FILE *in;

	if (argc > 1)
	{
		name = argv[1];
		if (argc > 2)
		{
			name2 = argv[2];
		}
	}
	if (!name2 || !name)
	{
		printf( "usage: mkrom in out\n");
		exit( 1);
	}
	printf( "inpu is [%s] output is [%s]\n", name, name2);
	in = fopen( name, "rb+");
	if (in)
	{
		long size, i;
		unsigned char *buf;
		int len;
		unsigned char sizeread, sizecomp;
		
		fseek( in, 0, SEEK_END);
		size = ftell( in);
		rewind( in);
		len = (size + 5) / 512 + 1;
//		if (!len)
//			len++;
		printf( "file size=%ld => len=%d\n", size, len);
//		size = len * 512 - 5;
		buf = malloc( size);
		if (buf)
		{
			int sum = 0x00;

			fread( buf, size, 1, in);
			if ((buf[0] != 0x55) || (buf[1] != 0xAA))
			{
				printf( "file is not a ROM extension !\n");
				exit( 42);
			}
			sizeread = buf[2];
			printf( "stored size=%d\n", sizeread);
			sizecomp = (size / 512) + 1;
			printf( "computed size=%d\n", sizecomp);
			buf[2] = sizecomp;
			for (i = 0; i < size; i++)
			{
				unsigned char c;
				
				c = buf[i];
				sum += c;
			}
			printf( "computed sum=%04x : ", sum);
			unsigned char pad = 0;
			if (sum % 256)
			{
				pad = -sum;
				printf( "mismatch should be %04X\n", (unsigned short)pad);
			}
			else
			{
				printf( "match\n");
			}

			FILE *out = fopen( name2, "wb");
			if (out)
			{
				unsigned short w;
				fwrite( buf, size, 1, out);
				int padsize = sizecomp * 512 - size - 2;
				if (padsize > 0)
				{
					char *buf2 = malloc( padsize);

					printf( "padding %d bytes\n", padsize);
					memset( buf2, 0x00, padsize);
					fwrite( buf2, padsize, 1, out);
					free( buf2);
				}
				w = pad;
				fwrite( &w, sizeof( w), 1, out);
				fclose( out);
			}

			free( buf);
		}
		fclose( in);
	}

	return 0;
}

