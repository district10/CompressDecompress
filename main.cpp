#ifdef _MSC_VER    /* Visual Studio */
#  define _CRT_SECURE_NO_WARNINGS
#  define snprintf sprintf_s
#endif

#include "lz4/lz4.h"
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "utils.h"
#include <QDebug>
#include <QDateTime>

int main(int argc, char *argv[])
{
    quint64 t = 0;
    char *inpFilename = "D:/img.bmp";
    char *lz4Filename = "D:img.lz4";
    char *decFilename = "D:/img-lz4.bmp";

    // compress
    {
        FILE *inpFp = fopen(inpFilename, "rb");
        FILE *outFp = fopen(lz4Filename, "wb");

        printf("compress : %s -> %s\n", inpFilename, lz4Filename);
        t = QDateTime::currentMSecsSinceEpoch();
        test_compress(outFp, inpFp);
        qDebug() << "Compress: " << (QDateTime::currentMSecsSinceEpoch() - t) << " ms";
        printf("compress : done\n");

        fclose(outFp);
        fclose(inpFp);
    }

    // decompress
    {
        FILE *inpFp = fopen(lz4Filename, "rb");
        FILE *outFp = fopen(decFilename, "wb");

        printf("decompress : %s -> %s\n", lz4Filename, decFilename);
        t = QDateTime::currentMSecsSinceEpoch();
        test_decompress(outFp, inpFp);
        qDebug() << "Decompress: " << (QDateTime::currentMSecsSinceEpoch() - t) << " ms";
        printf("decompress : done\n");

        fclose(outFp);
        fclose(inpFp);
    }

    // verify
    {
        FILE *inpFp = fopen(inpFilename, "rb");
        FILE *decFp = fopen(decFilename, "rb");

        printf("verify : %s <-> %s\n", inpFilename, decFilename);
        const int cmp = compare(inpFp, decFp);
        if(0 == cmp) {
            printf("verify : OK\n");
        } else {
            printf("verify : NG\n");
        }

        fclose(decFp);
        fclose(inpFp);
    }
    return 0;
}
