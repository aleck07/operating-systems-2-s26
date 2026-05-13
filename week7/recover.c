#include <stdio.h>
#include <string.h>

#define TEST_COUNT 10

int parity(unsigned char b)
{
    int par = 0;

    for (int i = 0; i < 8; i++) {
        par ^= (b >> i) & 1;
    }
    return par;
}

/**
 * Repair the three bytes of data.
 *
 * Return 1 if the data is correct or has been corrected.
 * Return 0 if the data cannot be corrected.
 */
int repair(unsigned char *data) {
    unsigned char a = data[0];
    unsigned char b = data[1];
    unsigned char c = data[2];
    int corrupted = 0;
    int bad = -1;

    // Extract stored parity bit (bit 7), then clear it
    int sa = (a >> 7) & 1; a &= 0x7F;
    int sb = (b >> 7) & 1; b &= 0x7F;
    int sc = (c >> 7) & 1; c &= 0x7F;

    // If stored parity doesn't match recomputed parity, byte is corrupted
    if (sa != parity(a)) { corrupted++; bad = 0; }
    if (sb != parity(b)) { corrupted++; bad = 1; }
    if (sc != parity(c)) { corrupted++; bad = 2; }

    if (corrupted > 1) return 0;

    // Replace the one bad byte with XOR of the two good bytes
    if (bad == 0) a = b ^ c;
    else if (bad == 1) b = a ^ c;
    else if (bad == 2) c = a ^ b;

    // Recompute and set parity bit at bit 7 for each byte, then store back
    data[0] = a | (parity(a) << 7);
    data[1] = b | (parity(b) << 7);
    data[2] = c | (parity(c) << 7);
    return 1;
}

int main(void)
{
    char *green = "\033[0;92m";  // Green
    char *red = "\033[0;30;101m";  // Black on red
    char *reset = "\033[0m";

    unsigned char trips[TEST_COUNT][3] = {
        {0x26, 0xd9, 0xec},
        {0xcc, 0x71, 0xbd},
        {0xbb, 0xdb, 0x60},
        {0x5e, 0xb0, 0x7e},
        {0x72, 0x04, 0x56},
        {0x8e, 0x10, 0x1e},
        {0xde, 0xd7, 0x09},
        {0x47, 0x66, 0x29},
        {0x05, 0xe2, 0xef},
        {0x29, 0x1e, 0x33}
    };

    unsigned char solutions[TEST_COUNT][4] = {
        {0x24, 0xd8, 0xfc, 0x00},
        {0xcc, 0x71, 0xbd, 0x01},
        {0xbb, 0xdb, 0x60, 0x01},
        {0xde, 0xa0, 0x7e, 0x00},
        {0x72, 0x24, 0x56, 0x01},
        {0x8e, 0x90, 0x1e, 0x01},
        {0xde, 0xd7, 0x09, 0x01},
        {0x47, 0x66, 0x21, 0x01},
        {0x05, 0xe2, 0xe7, 0x01},
        {0x2d, 0x1e, 0x33, 0x01}
    };

    unsigned char temp[3];

    for (int i = 0; i < TEST_COUNT; i++) {
        memcpy(temp, trips[i], sizeof temp);

        int result = repair(temp);
        int expected_result = solutions[i][3];

        if (result != expected_result) {
            printf("%sFAIL%s: [%02x,%02x,%02x] returned %d (expected %d)\n",
                red, reset,
                trips[i][0], trips[i][1], trips[i][2],
                result, expected_result);
            continue;
        }

        if (result == 1) { // corrected it
            int bad_count = 0;

            for (int j = 0; j < 3; j++)
                if (temp[j] != solutions[i][j])
                    bad_count++;

            if (bad_count > 0) {
                printf("%sFAIL%s: [%02x,%02x,%02x] -> [%02x,%02x,%02x] "
                    "(expected [%02x,%02x,%02x])\n", red, reset,
                    trips[i][0], trips[i][1], trips[i][2],
                    temp[0], temp[1], temp[2],
                    solutions[i][0], solutions[i][1], solutions[i][2]);
                continue;
            }

            printf("%sPASS%s: [%02x,%02x,%02x] -> [%02x,%02x,%02x]\n",
                green, reset,
                trips[i][0], trips[i][1], trips[i][2],
                solutions[i][0], solutions[i][1], solutions[i][2]);
        } else {
            // Could not correct it
            printf("%sPASS%s: [%02x,%02x,%02x] could not be corrected\n",
                green, reset,
                trips[i][0], trips[i][1], trips[i][2]);
        }
    }
}
