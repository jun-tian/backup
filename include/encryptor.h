#ifndef INCLUDE_AES_H_
#define INCLUDE_AES_H_

#include "filebase.h"
#include <openssl/md5.h>
#include <openssl/aes.h>

#define FILE_SUFFIX_ENCRYPT ".ept"

class Encryptor
{
private:
    unsigned char key[MD5_DIGEST_LENGTH];
    std::filesystem::path file_path;


public:
    Encryptor(std::string file_path_, std::string password_);
    ~Encryptor();

    bool Encrypt(); // 加密

    /// @brief 解密
    /// @return 0成功 -1密码错误 -2文件错误
    int Decrypt();
};

#endif // INCLUDE_AES_H_