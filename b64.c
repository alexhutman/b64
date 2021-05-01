#include <stdio.h>


char b64_lookup(char key);
void encode_file(FILE *fp);

// This likely won't work for any configuration where chars are longer than 8 bits.

int main(int argc, char *argv[]) {
	// Read from stdin
	if (argc < 2) {

	}
	// Use the specified file
	else {
		/*
		char *fname = argv[1];
		FILE *fp = fopen(fname, "r");
		if (fp == NULL) {
			fprintf(stderr, "File %s does not exist!\n", fname);
			return 1;
		}

		encode_file(fp);

		if (fclose(fp) != 0) {
			fprintf(stderr, "Error closing file %s\n", fname);
			return 1;
		}
		*/
		encode_file(NULL);
	}

	return 0;
}

void encode_file(FILE *fp) {
	char tmp = '\0';
	// For testing, just using a hardcoded buffer
	// Using size of 3 because 3 = lcm(6,8) / 8
	// i.e. using 3 chars (octets), we can extract 4 b64 sextets with no wasted space.
	// I will look into using a multiple of 3 in the future, but it's fine for now in order to test.
	char in_buf[] = {'h', 'i', '!'};
	char out_buf[50] = {0};

	// The following representation of the buffer makes it easier to see why these operations are performed
	// The number represents which sextet the bit is in, the grouping represents each octet
	// 11111122 | 22223333 | 33444444

	// First sextet
	tmp = (in_buf[0] >> 2) & 0x3f;
	out_buf[0] = b64_lookup(tmp);

	// Second sextet
	tmp = (in_buf[0] << 4) & 0x30 | (in_buf[1] >> 4);
	out_buf[1] = b64_lookup(tmp);

	// Third sextet
	tmp = (in_buf[1] << 2) & 0x3c | ((in_buf[2] >> 6) & 0x03);
	out_buf[2] = b64_lookup(tmp);

	// Fourth sextet
	tmp = in_buf[2] & 0x3f;
	out_buf[3] = b64_lookup(tmp);

	printf("%s\n", out_buf);
}

// This function assumes it will always be given 0 <= key <= 63
char b64_lookup(char key) {
	if (key < 26) {
		return key + 65;
	}
	if (key < 52) {
		return key + 71;
	}
	if (key < 62) {
		return key - 4;
	}
	if (key == 62) {
		return '+';
	}
	return '/';
}

