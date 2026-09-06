#include <iostream>
#include <filesystem>
#include <openssl/rand.h>
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

void GenerateAESKey()
{    
      unsigned char aes_key[32];
      
      if (RAND_bytes(aes_key, sizeof(aes_key)) != 1){
          std::cerr << "Failed to generate key\n";
          return;
      }

      for (unsigned char byte : aes_key) {
      std::cout << std::hex
                 << std::setw(2)
                 << std::setfill('0')
                 << static_cast<int>(byte);
      }

      std::cout << '\n';
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

  GenerateAESKey();
  return 0;
}

