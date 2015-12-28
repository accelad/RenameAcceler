#include <vector>
#include <filesystem>
#include <iostream>
#include <algorithm>
#include <Shlwapi.h>
#pragma comment(lib, "Shlwapi.lib")


struct SrcTarPathPair {
  std::experimental::filesystem::path src_path;
  std::experimental::filesystem::path tar_path;
};

struct SrcTarExtPair {
  std::string src_ext = ".ass";
  std::string tar_ext = ".mkv";
};

static std::size_t rename_all(
    const std::vector<SrcTarPathPair> &path_pairs);

static std::size_t backup_all(
    const std::vector<SrcTarPathPair> &path_pairs,
    const std::experimental::filesystem::path &directory);

static void set_directory(
    std::experimental::filesystem::path &directory);

static void set_src_tar_path_pairs(
    std::vector<SrcTarPathPair> &path_pairs,
    const SrcTarExtPair &ext_pair,
    const std::experimental::filesystem::path &directory);

static void set_src_tar_ext_pair(
    SrcTarExtPair &pair);

int main()
{
  namespace fs = std::experimental::filesystem;
  fs::path directory;
  std::vector<SrcTarPathPair> path_pairs;
  SrcTarExtPair ext_pair;
  set_directory(directory);
  auto backup_path = directory / "backup";
  auto cmd = ' ';
  std::cout << "Would you want to set file's extensions manully? (Y/N)\n";
  std::cin >> cmd;
  if (std::tolower(cmd, std::locale()) == 'y') {
    set_src_tar_ext_pair(ext_pair);
  }
  set_src_tar_path_pairs(path_pairs, ext_pair, directory);
  if (fs::exists(directory)) {
    backup_all(path_pairs, backup_path);
    rename_all(path_pairs);
  }
  return 0;
}

static std::size_t rename_all(
    const std::vector<SrcTarPathPair> &path_pairs)
{
  namespace fs = std::experimental::filesystem;
  std::size_t count = 0;
  for (const auto &val : path_pairs) {
    fs::rename(val.src_path, val.tar_path);
    ++count;
  }
  return count;
}

static std::size_t backup_all(
    const std::vector<SrcTarPathPair> &path_pairs,
    const std::experimental::filesystem::path &directory) 
{
  namespace fs = std::experimental::filesystem;
  size_t count = 0;
  if (fs::exists(directory) == false) { fs::create_directory(directory); }
  for (const auto &val : path_pairs) {
    fs::copy_file(val.src_path, directory / val.src_path.filename(), 
                  fs::copy_options::overwrite_existing);
    ++count;
  }
  return count;
}

static void set_directory(
    std::experimental::filesystem::path &directory)
{
  std::cout << "Please input a direcotry path:\n";
  std::string tmp;
  std::getline(std::cin, tmp);
  directory = tmp;
}

static void set_src_tar_path_pairs(
    std::vector<SrcTarPathPair> &path_pairs,
    const SrcTarExtPair &ext_pair,
    const std::experimental::filesystem::path &directory)
{
  namespace fs = std::experimental::filesystem;
  if (fs::exists(directory) == false) { return; }
  std::vector<fs::path> src_paths;
  std::vector<fs::path> tar_paths;
  auto path = fs::path();
  for (const auto &val : fs::directory_iterator(directory)) {
    path.assign(val);
    if (path.extension() == ext_pair.src_ext) {
      src_paths.push_back(path);
    }
    else if (path.extension() == ext_pair.tar_ext) {
      tar_paths.push_back(path);
    }
  }
  std::sort(src_paths.begin(), src_paths.end(),
    [](const auto &left, const auto &right) {
    return StrCmpLogicalW(left.c_str(), right.c_str()) < 0;
  });
  std::sort(tar_paths.begin(), tar_paths.end(),
    [](const auto &left, const auto &right) {
    return StrCmpLogicalW(left.c_str(), right.c_str()) < 0;
  });

  for (auto idx = 0u; 
       idx != src_paths.size() && idx != tar_paths.size(); ++idx) {
    tar_paths[idx].replace_extension(src_paths[idx].extension());
    path_pairs.push_back(SrcTarPathPair({ src_paths[idx], tar_paths[idx] }));
  }
}

static void set_src_tar_ext_pair(
    SrcTarExtPair &ext_pair)
{
  std::cout << "Please input the extensions of "
               "source file and target file respectively\n";
  std::cin >> ext_pair.src_ext;
  std::cin >> ext_pair.tar_ext;
  if (ext_pair.src_ext.front() != '.') {
    ext_pair.src_ext.insert(0, ".");
  }
  if (ext_pair.tar_ext.front() != '.') {
    ext_pair.tar_ext.insert(0, ".");
  }
}