# trap.cpp — Hybrid Encryption Demo

A small C++ program that demonstrates the hybrid-encryption design used by
real ransomware: files are encrypted with a fast symmetric cipher, and the
symmetric key is protected with asymmetric (RSA) encryption. It can encrypt
or decrypt every regular file under a directory. **For learning and
reversing practice only — do not run it on anything you care about.**

## Build

```sh
g++ -std=c++17 -O2 trap.cpp -o trap -lcrypto
```

Requires OpenSSL (headers + libcrypto) and a C++17 compiler.

## Usage

```sh
./trap encrypt <directory>   # encrypt all regular files under <directory>
./trap decrypt <directory>   # restore files previously encrypted
```

Both modes need a working directory (the current directory) containing the
key material files listed below. The binary itself is also encrypted if it
lives inside the target directory, so keep it elsewhere.

## Key files

| File              | Produced by           | Purpose                                        |
|-------------------|-----------------------|------------------------------------------------|
| `trap_private.pem` | first run (`generateKeyPair`, trap.cpp:52) | RSA-2048 private key; needed to decrypt      |
| `trap_public.pem`  | first run             | RSA-2048 public key; used to wrap the AES key  |
| `trap_key.bin`     | first `encrypt` run   | AES-256 key, RSA-OAEP-wrapped with the public key |

On the first run, if the PEM files don't exist, a 2048-bit RSA key pair is
generated and written next to the program (trap.cpp:352-359).

## How the key management works (`getAesKey`, trap.cpp:292)

The program uses a random 256-bit AES key per "victim" rather than encrypting
files with RSA directly:

1. If `trap_key.bin` already exists, it is decrypted (unwrapped) using the
   RSA **private** key via RSA-OAEP (`unwrapAesKey`, trap.cpp:122). This is
   the path taken by `decrypt` mode (and by repeat `encrypt` runs, which
   reuse the same AES key).
2. Otherwise a fresh AES key is generated with `RAND_bytes`, wrapped with the
   RSA **public** key (`wrapAesKey`, trap.cpp:98), and saved to
   `trap_key.bin`.

This is the exact scheme real ransomware uses: the public key can only wrap
new keys, so deleting the private key leaves the victim unable to recover
the AES key. The private key never touches the encrypted files.

## File format

After encryption a file looks like this (all fields written back over the
original file, trap.cpp:249-252):

```
+--------+-----------+-----------+----------------+
| "TRAP" | IV (12 B) | tag (16 B)|  ciphertext    |
+--------+-----------+-----------+----------------+
```

- **magic** `"TRAP"` (4 bytes) — marks the file as encrypted; `decryptFile`
  skips files without it (trap.cpp:266-269).
- **IV** — 12-byte random AES-GCM nonce, generated per file.
- **tag** — 16-byte AES-GCM authentication tag, produced during encryption.
- **ciphertext** — same length as the original plaintext.

## Encryption path

`encryptFile` (trap.cpp:222):

1. Reads the whole file into memory.
2. Generates a fresh random 12-byte IV (`RAND_bytes`).
3. Encrypts with AES-256-GCM (`encryptAES`, trap.cpp:147) and extracts the
   GCM tag.
4. Overwrites the file in place: magic, IV, tag, ciphertext.

## Decryption path

`decryptFile` (trap.cpp:257):

1. Reads the file and checks the `"TRAP"` magic; silently skips anything else.
2. Splits out IV, tag, and ciphertext.
3. Decrypts with AES-256-GCM (`decryptAES`, trap.cpp:182). GCM verifies the
   tag during `EVP_DecryptFinal_ex`; if the key is wrong or the data is
   corrupted, decryption fails and the file is left untouched.
4. Writes the recovered plaintext back over the file.

## Directory walk

`main` (trap.cpp:332) validates the mode and directory, ensures the key
material exists, resolves the AES key, then recursively walks the target
directory with `fs::recursive_directory_iterator`, applying
`encryptFile`/`decryptFile` to every regular file
(trap.cpp:367-384). Files that fail (unreadable, wrong key on decrypt, etc.)
are skipped with a message; a filesystem error terminates the walk but the
program still exits 0.

## Attack model / why this design

- AES-GCM is fast enough to bulk-encrypt many files; RSA can only encrypt a
  few hundred bytes per operation.
- The wrapped key file means only the RSA private key holder can recover the
  AES key — the classic ransomware extortion point (attacker keeps the
  private key; victim gets `trap_key.bin` but can't unwrap it).
- In this demo the private key sits in the working directory, which real
  malware would never do — that makes this copy fully self-decryptable.
