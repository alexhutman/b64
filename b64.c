#include <stdio.h>


char b64_lookup(char key);
void encode_file(FILE *fp);
void encode_block(char* in, char* out);
void encode_last_block(char* in, char* out, unsigned int num_extra);

// This likely won't work for any configuration where chars are longer than 8 bits.

int main(int argc, char *argv[]) {
	// Read from stdin
	if (argc < 2) {
		encode_file(stdin);
	}

	// Use the specified file
	else {
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
	}

	return 0;
}

void encode_block(char *in, char *out) {
	// First sextet
	char tmp = (in[0] >> 2) & 0x3f;
	out[0] = b64_lookup(tmp);

	// Second sextet
	tmp = (in[0] << 4) & 0x30 | (in[1] >> 4);
	out[1] = b64_lookup(tmp);

	// Third sextet
	tmp = (in[1] << 2) & 0x3c | ((in[2] >> 6) & 0x03);
	out[2] = b64_lookup(tmp);

	// Fourth sextet
	tmp = in[2] & 0x3f;
	out[3] = b64_lookup(tmp);
}

void encode_last_block(char *in, char *out, unsigned int num_extra) {
	// First sextet is the same if we have 1 or 2 extra bytes
	char tmp = (in[0] >> 2) & 0x3f;
	out[0] = b64_lookup(tmp);

	if (num_extra == 1) {
		// Second sextet if only 1 extra byte
		tmp = (in[0] << 4) & 0x30;
		out[1] = b64_lookup(tmp);

		// 2 padding characters
		out[2] = '=';
		out[3] = '=';
	}
	else {
		// Second sextet if 2 extra bytes
		tmp = (in[0] << 4) & 0x30 | (in[1] >> 4);
		out[1] = b64_lookup(tmp);

		// Third sextet if 2 extra bytes
		tmp = (in[1] << 2) & 0x3c;
		out[2] = b64_lookup(tmp);

		// 1 padding character
		out[3] = '=';
	}
}

void encode_file(FILE *fp) {
	// For testing, just using a hardcoded buffer
	// Using a multiple of 3 because 3 = lcm(6,8) / 8
	// i.e. using 3 chars (octets), we can extract 4 b64 sextets with no wasted space.
	char in_buf[] = {'h', 'i', ' ', 't', 'h', 'e', 'r', 'e', '!'};
	char out_buf[50] = {0};

	size_t buf_size = (sizeof (in_buf))/(sizeof (in_buf[0]));

	// The following representation of the buffer makes it easier to see why these operations are performed
	// The number represents which sextet the bit is in, the grouping represents each octet
	// 11111122 | 22223333 | 33444444

	unsigned int num_triples = buf_size / 3;
	unsigned int num_extra = buf_size % 3;

	for (int i=0; i<num_triples; i++) {
		encode_block(&in_buf[i*3], &out_buf[i*4]);
	}

	if (num_extra > 0) {
		encode_last_block(&in_buf[num_triples*3], &out_buf[num_triples*4], num_extra);
	}

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

