#include <stdio.h>

#include <Windows.h>
int main(int argc, char *argv[])
{
    {
        // /// 加密后密文
        // byte XOR = 0x12; /// 密钥
        // byte b[4];       /// 明文
        // b[0] = (byte)'a';
        // b[1] = (byte)'b';
        // b[2] = (byte)'c';
        // b[3] = (byte)'d';
        //
        // byte bDest[4];
        // byte bDestProtect[4];
        // for (int i = 0; i < 4; i++)
        // {
        //     bDest[i] = byte(b[i] ^ XOR);
        // }
        // for (int i = 0; i < 4; i++)
        // {
        //     printf("%c\n", bDest[i]);
        // }
        //
        // printf("***********\n");
        // /// 解密后明文(异或特性)
        // for (int i = 0; i < 4; i++)
        // {
        //     bDestProtect[i] = (byte)(bDest[i] ^ XOR);
        // }
        // for (int i = 0; i < 4; i++)
        // {
        //     printf("%c\n", bDestProtect[i]);
        // }
    }

    FILE *out = NULL, *in = NULL;
    // in  = fopen("assert/001.txt", "rb");
    // in  = fopen("assert/middle.txt", "rb");
    // out = fopen("assert/out.txt", "wb");

    in  = fopen("assert/out.txt", "rb");
    out = fopen("assert/out2.txt", "wb");

    BYTE buf[1024] = { 0 };
    BYTE XOR       = 0x12;
    int  res;
    while ((res = fread(buf, sizeof(unsigned char), 1024, in)) != 0)
    {
        for (int i = 0; i < res; i++)
        {
            buf[i] = (BYTE)(buf[i] ^ XOR); /// 加密
        }
        fwrite(buf, sizeof(unsigned char), res, out);
    }


    fclose(out);
    fclose(in);

    getchar();
    return 0;
}
