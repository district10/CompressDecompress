#ifndef UTILS_H
#define UTILS_H

enum {
    BLOCK_BYTES = 1024 * 8,
//  BLOCK_BYTES = 1024 * 64,
};

size_t write_int(FILE* fp, int i) {
    return fwrite(&i, sizeof(i), 1, fp);
}

size_t write_bin(FILE* fp, const void* array, size_t arrayBytes) {
    return fwrite(array, 1, arrayBytes, fp);
}

size_t read_int(FILE* fp, int* i) {
    return fread(i, sizeof(*i), 1, fp);
}

size_t read_bin(FILE* fp, void* array, size_t arrayBytes) {
    return fread(array, 1, arrayBytes, fp);
}

void test_compress(FILE* outFp, FILE* inpFp)
{
    LZ4_stream_t lz4Stream_body;
    LZ4_stream_t* lz4Stream = &lz4Stream_body;

    char inpBuf[2][BLOCK_BYTES];
    int  inpBufIndex = 0;

    LZ4_resetStream(lz4Stream);

    for(;;) {
        char* const inpPtr = inpBuf[inpBufIndex];
        const int inpBytes = (int) read_bin(inpFp, inpPtr, BLOCK_BYTES);
        if(0 == inpBytes) {
            break;
        }

        {
            char cmpBuf[LZ4_COMPRESSBOUND(BLOCK_BYTES)];
            const int cmpBytes = LZ4_compress_fast_continue(
                lz4Stream, inpPtr, cmpBuf, inpBytes, sizeof(cmpBuf), 1);
            if(cmpBytes <= 0) {
                break;
            }
            write_int(outFp, cmpBytes);
            write_bin(outFp, cmpBuf, (size_t) cmpBytes);
        }

        inpBufIndex = (inpBufIndex + 1) % 2;
    }

    write_int(outFp, 0);
}



void test_decompress(FILE* outFp, FILE* inpFp)
{
    LZ4_streamDecode_t lz4StreamDecode_body;
    LZ4_streamDecode_t* lz4StreamDecode = &lz4StreamDecode_body;

    char decBuf[2][BLOCK_BYTES];
    int  decBufIndex = 0;

    LZ4_setStreamDecode(lz4StreamDecode, NULL, 0);

    for(;;) {
        char cmpBuf[LZ4_COMPRESSBOUND(BLOCK_BYTES)];
        int  cmpBytes = 0;

        {
            const size_t readCount0 = read_int(inpFp, &cmpBytes);
            if(readCount0 != 1 || cmpBytes <= 0) {
                break;
            }

            const size_t readCount1 = read_bin(inpFp, cmpBuf, (size_t) cmpBytes);
            if(readCount1 != (size_t) cmpBytes) {
                break;
            }
        }

        {
            char* const decPtr = decBuf[decBufIndex];
            const int decBytes = LZ4_decompress_safe_continue(
                lz4StreamDecode, cmpBuf, decPtr, cmpBytes, BLOCK_BYTES);
            if(decBytes <= 0) {
                break;
            }
            write_bin(outFp, decPtr, (size_t) decBytes);
        }

        decBufIndex = (decBufIndex + 1) % 2;
    }
}

int compare(FILE* fp0, FILE* fp1)
{
    int result = 0;

    while(0 == result) {
        char b0[65536];
        char b1[65536];
        const size_t r0 = read_bin(fp0, b0, sizeof(b0));
        const size_t r1 = read_bin(fp1, b1, sizeof(b1));

        result = (int) r0 - (int) r1;

        if(0 == r0 || 0 == r1) {
            break;
        }
        if(0 == result) {
            result = memcmp(b0, b1, r0);
        }
    }

    return result;
}
#endif // UTILS_H

