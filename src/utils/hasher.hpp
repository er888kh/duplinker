#pragma once

#include <cstdint>
#include <fstream>
#include <ios>
#include <string>

#include <cryptopp/hex.h>
#include <cryptopp/cryptlib.h>
#include <cryptopp/sha.h>

#include "../constants.hpp"

#define HASH_SIZE_GOOD (CryptoPP::SHA256::DIGESTSIZE * 2)

using string = std::string;

string
get_file_hash(const string &filename)
{
    std::streamsize ssz;
    std::ifstream inp(filename);
    string digest;
    string result;

    CryptoPP::SHA256 hash;
    CryptoPP::HexEncoder enc;

    std::uint64_t ret_len;
    char buf[BUF_SIZE];

    while ((ssz = inp.readsome(buf, BUF_SIZE)) > 0) {
        hash.Update((const CryptoPP::byte*) buf, ssz);
    }

    digest.resize(CryptoPP::SHA256::DIGESTSIZE);
    hash.Final((CryptoPP::byte*) &digest[0]);

    enc.Put((const CryptoPP::byte*) digest.c_str(), digest.size());
    enc.MessageEnd();

    ret_len = enc.TotalBytesRetrievable();
    if (ret_len) {
        result.resize(ret_len);
        enc.Get((CryptoPP::byte*) &result[0], result.size());
    }

    return result;
}