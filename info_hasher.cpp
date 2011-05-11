#include <cstring>
#include <cstdio>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <iostream>

#include <cppbencode/bencode.h>
#include <polarssl/sha1.h>

#include "info_hasher.h"

void info_hasher::check_dir(char* dirname)
{
  DIR* dp;
  struct dirent* ep;
  struct stat st;

  dp = opendir(dirname);

  if (dp != NULL)
  {
    fchdir(dirfd(dp));

    while (ep = readdir(dp))
    {
      if (strcmp(ep->d_name, ".") == 0 || strcmp(ep->d_name, "..") == 0)
        continue;

      int rt = stat(ep->d_name, &st);
      if (rt != 0)
      {
        std::cerr << "Couldn't stat " << ep->d_name << std::endl;
        continue;
      }

      if (S_ISDIR(st.st_mode))
      {
        check_dir(ep->d_name);
      }

      if (S_ISREG(st.st_mode))
      {
        process_file(ep->d_name, st.st_size);
      }
    }

    chdir("..");

    closedir(dp);
  }
}

void info_hasher::process_file(char* filenamein, size_t filesize)
{
  FILE* fpi = fopen(filenamein, "r");
  char buf[1024];
  std::string filedata_in;
  std::string infohash_out;
  std::string filedata_out;
  unsigned char hash_raw[20];
  char hash_hex[40];

  if (fpi == NULL)
  {
    std::cerr << "Couldn't open " << filenamein << std::endl;
    return;
  }

  while (size_t r = fread(buf, 1, sizeof(buf), fpi))
  {
    filedata_in.append(buf, r);
  }

  fclose(fpi);

  ben::variant data;
  if (ben::decoder::decode_all(&data, filedata_in))
  {
    std::cout << "Couldn't decode " << filenamein << std::endl;
    return;
  }

  ben::variant_map torrent;
  if (data.get(&torrent))
  {
    std::cout << "Couldn't get root for " << filenamein << std::endl;
    return;
  }

  ben::variant_map info;
  if (torrent.find("info") == torrent.end())
  {
    info = torrent;
  }
  else
  {
    torrent.get("info").get(&info);
  }

  torrent.clear();
  torrent["info"] = info;

  ben::encode(&infohash_out, info);
  ben::encode(&filedata_out, torrent);

  sha1((unsigned char*)infohash_out.c_str(), infohash_out.size(), hash_raw);

  for (int i=0;i<20;++i)
    sprintf(hash_hex+(i*2), "%02x", hash_raw[i]);
  std::string hash(hash_hex, 40);

  dbh.set(hash.c_str(), filedata_out);

  std::cout << "Successfully read " << filenamein << std::endl;
}
