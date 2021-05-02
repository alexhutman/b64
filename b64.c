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
	// Using a multiple of 3 because 3 = lcm(6,8) / 8
	// i.e. using 3 chars (octets), we can extract 4 b64 sextets with no wasted space.
	char in_buf[] = {'h', 'i', ' ', 't', 'h', 'e', 'r', 'e', '!'};
	size_t buf_size = (sizeof (in_buf))/(sizeof (in_buf[0]));
	char out_buf[50] = {0};

	// The following representation of the buffer makes it easier to see why these operations are performed
	// The number represents which sextet the bit is in, the grouping represents each octet
	// 11111122 | 22223333 | 33444444

	unsigned int num_triples = buf_size / 3;
	unsigned int num_extra_bytes = buf_size % 3;

	int first_oct, second_oct, third_oct;
	int out_idx = 0;
	for (int i=0; i<num_triples; i++) {
		first_oct = i*3;
		second_oct = first_oct+1;
		third_oct = first_oct+2;

		// First sextet
		tmp = (in_buf[first_oct] >> 2) & 0x3f;
		out_buf[out_idx] = b64_lookup(tmp);
		out_idx++;

		// Second sextet
		tmp = (in_buf[first_oct] << 4) & 0x30 | (in_buf[second_oct] >> 4);
		out_buf[out_idx] = b64_lookup(tmp);
		out_idx++;

		// Third sextet
		tmp = (in_buf[second_oct] << 2) & 0x3c | ((in_buf[third_oct] >> 6) & 0x03);
		out_buf[out_idx] = b64_lookup(tmp);
		out_idx++;

		// Fourth sextet
		tmp = in_buf[third_oct] & 0x3f;
		out_buf[out_idx] = b64_lookup(tmp);
		out_idx++;
	}

	if (num_extra_bytes > 0) {
		first_oct += 3;
		second_oct = first_oct+1;
		third_oct = first_oct+2;

		// First sextet is the same if we have 1 or 2 extra bytes
		tmp = (in_buf[first_oct] >> 2) & 0x3f;
		out_buf[out_idx] = b64_lookup(tmp);
		out_idx++;

		if (num_extra_bytes == 1) {
			// Second sextet if only 1 extra byte
			tmp = (in_buf[first_oct] << 4) & 0x30;
			out_buf[out_idx] = b64_lookup(tmp);
			out_idx++;

			// 2 padding characters
			out_buf[out_idx] = '=';
			out_buf[out_idx + 1] = '=';
		}
		else {
			// Second sextet if 2 extra bytes
			tmp = (in_buf[first_oct] << 4) & 0x30 | (in_buf[second_oct] >> 4);
			out_buf[out_idx] = b64_lookup(tmp);
			out_idx++;

			// Third sextet if 2 extra bytes
			tmp = (in_buf[second_oct] << 2) & 0x3c;
			out_buf[out_idx] = b64_lookup(tmp);
			out_idx++;

			// 1 padding character
			out_buf[out_idx] = '=';
		}
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

