#include <string>
#include <kcpolydb.h>

class info_hasher
{
protected:
  kyotocabinet::PolyDB dbh;
public:
  void check_dir(char* dirname);
  void process_file(char* filename, size_t filesize);

  info_hasher(const std::string& file)
  {
    dbh.open(file, kyotocabinet::PolyDB::OWRITER | kyotocabinet::PolyDB::OCREATE);
  }

  ~info_hasher()
  {
    dbh.close();
  }
};
