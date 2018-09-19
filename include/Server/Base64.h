/**
 *  Copyright to polfosol
 *  https://raw.githubusercontent.com/gaspardpetit/base64/master/src/polfosol/polfosol.h
 */
#pragma once
#ifndef PARSER_BASE64_H
#define PARSER_BASE64_H

#include <string>
#include <cstring>
#include <utility>

namespace Base64 {

static constexpr const unsigned char B64index[256] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 62, 63, 62, 62, 63,
    52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 3, 4,
    5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24,
    25, 0, 0, 0, 0, 63, 0, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38,
    39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51
};

std::string decode(const void* data, const size_t len) {
  auto p = (unsigned char*) data;
  int pad = len > 0 && (len % 4 || p[len - 1] == '=');
  const size_t L = ((len + 3) / 4 - pad) * 4;
  std::string str(L / 4 * 3 + pad, '\0');

  for (size_t i = 0, j = 0; i < L; i += 4) {
    int n = B64index[p[i]] << 18 | B64index[p[i + 1]] << 12 |
            B64index[p[i + 2]] << 6 | B64index[p[i + 3]];
    str[j++] = n >> 16;
    str[j++] = n >> 8 & 0xFF;
    str[j++] = n & 0xFF;
  }
  if (pad) {
    int n = B64index[p[L]] << 18 | B64index[p[L + 1]] << 12;
    str[str.size() - 1] = n >> 16;

    if (len > L + 2 && p[L + 2] != '=') {
      n |= B64index[p[L + 2]] << 6;
      str.push_back(n >> 8 & 0xFF);
    }
  }
  return str;
}


std::string
decode_mod(const unsigned char* table, const void* data, const size_t len) {
  auto p = (unsigned char*) data;
  int pad = len > 0 && (len % 4 || p[len - 1] == '=');
  const size_t L = ((len + 3) / 4 - pad) * 4;
  std::string str;
  str.resize(3 * ((len + 3) / 4));

  int j = 0;
  for (size_t i = 0; i < L; i += 4) {
    int n = table[p[i]] << 18 | table[p[i + 1]] << 12 | table[p[i + 2]] << 6 |
            table[p[i + 3]];
    str[j++] = n >> 16;
    str[j++] = n >> 8 & 0xFF;
    str[j++] = n & 0xFF;
  }
  if (pad) {
    int n = table[p[L]] << 18 | table[p[L + 1]] << 12;
    str[j++] = n >> 16;

    if (len > L + 2 && p[L + 2] != '=') {
      n |= table[p[L + 2]] << 6;
      str[j++] = n >> 8 & 0xFF;
    }
  }

  str.resize(j);
  return std::move(str);
}

std::string decode(const std::string& str64) {
  return decode(str64.c_str(), str64.size());
}
}
#endif //PARSER_BASE64_H