#include <iostream>
#include <filesystem>

// initial access 
// Execution
//Privilege access 
//Discovery
//Key management ( private and public key)
//Encryption

namespace fs = std::filesystem;

void Listfiles(const fs::path& directory)
{
  try {
    for (const auto& entry : fs::recursive_directory_iterator(
          directory, fs::directory_options::skip_permission_denied
          ))
    {
      if (entry.is_regular_file()) {
        std::cout << entry.path() << '\n'; 
      }
    }
  }
  catch (const fs::filesystem_error& e ){
    std::cerr << "Filesystem error:" << e.what() << '\n';
  }
}

int main(){
  std::string directory;
  std::cout << "Directory to inspect: ";
  std::getline(std::cin, directory);

  if (!fs::exists(directory) || !fs::is_directory(directory)){
    std::cerr << "Invalide directory \n";
    return 1;
  }
  Listfiles(directory);
  return 0;
}

