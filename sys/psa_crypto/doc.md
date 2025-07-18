@defgroup    sys_psa_crypto    PSA Cryptographic API
@ingroup     sys
@brief       Implements the PSA Crypto API specification.
@see         https://armmbed.github.io/mbed-crypto/html/

@warning This implementation is not complete and not yet thoroughly tested.
         Please do not use this module in production, as it may introduce security issues.

@note    This implementation is not complete and will be successively expanded.

About {#About}
=====
This module implements the PSA Cryptography API Version 1.1 as specified
[here](https://armmbed.github.io/mbed-crypto/html/) and the PSA Status code API Version 1.0
as specified [here](https://arm-software.github.io/psa-api/status-code/1.0/).
It provides an OS level access to cryptographic operations and supports software and hardware
backends as well as the use of secure elements.
The API automatically builds a hardware backend for an operation, if there's one available,
otherwise it falls back to software. Specific backends can be configured, if needed.
For configuration options see [Configuration](#configuration).

PSA Crypto has an integrated key management module, which stores keys internally
without exposing them to applications. To learn how to use keys with PSA,
read [Using Keys](#using-keys).

A basic usage and configuration example can be found in `examples/advanced/psa_crypto`.
For more usage instructions, please read the documentation.

If you want to add your own crypto backend, see [Porting Guide](#porting-guide).

Basic Usage
===
To use PSA Crypto, add `psa/crypto.h` to your includes. This will make all
operations and macros available.

The library is automatically initialized on startup if @ref sys_auto_init is used
(enabled by default). Otherwise, @ref psa_crypto_init needs to be called before
any other operation.

## Structure Initialization
Whenever you declare a PSA Crypto structure (e.g. operation contexts or key attributes),
it needs to be initialized with zeroes. A structure that is not initialized will be interpreted
by PSA as *active* and can not be used for a new operation.
The example function and macro shown below result in the same thing: A new, inactive structure.

```c
// Choose one of these options
psa_hash_operation_t hash_op = psa_hash_operation_init();
psa_hash_operation_t hash_op = PSA_HASH_OPERATION_INIT;
```

An already active operation can be set to zero by reinitializing it. It then becomes *inactive*
again and can be used for a new operation.

When errors occur during execution, PSA resets the operation contexts and makes them
*inactive*, to prevent unauthorized access to an operation's state.
Users can also call `psa_<operation>_abort()` anytime in between function calls to do the same.

Using Keys {#using-keys}
===
PSA can only operate on keys, that are registered with and stored within the internal
key storage module. This means you need to either generate keys with PSA or
import an existing key.
For this purpose there are a number of
[key management functions](https://armmbed.github.io/mbed-crypto/html/api/keys/management.html)
(external link).

## Key Attributes
When creating a key for PSA, the implementation needs to know what kind of key it is
dealing with, what it can be used for, where it's supposed to be stored, etc.
That information needs to be specified in a set of
[Key Attributes](https://armmbed.github.io/mbed-crypto/html/api/keys/attributes.html)
(external link).

The example below defines attributes for an AES-128 key, which can be used for CBC encryption
and decryption and will be stored in local volatile memory.
```c
// Initializes empty attributes structure
psa_key_attributes_t attributes = psa_key_attributes_init();

// Set all necessary attributes
psa_set_key_lifetime(&attributes, PSA_KEY_LIFETIME_VOLATILE);
psa_set_key_type(&attributes, PSA_KEY_TYPE_AES);
psa_set_key_bits(&attributes, 128);
psa_set_key_algorithm(&attributes, PSA_ALG_CBC_NO_PADDING);
psa_set_key_usage_flags(&attributes, (PSA_KEY_USAGE_ENCRYPT | PSA_KEY_USAGE_DECRYPT));
```

After setting the attributes, an exiting key can be imported:
```c
uint8_t aes_key[] = { ... };
psa_key_id_t key_id = 0;     // Will be set by PSA Crypto
psa_status_t status = psa_import_key(&attributes, aes_key, sizeof(aes_key), &key_id);
```
The PSA Crypto implementation will assign an identifier to the key and return it
via the `key_id` parameter. This identifier can then be used for operations with this
specific key.
```c
uint8_t PLAINTEXT[] = { ... };
// Buffer sizes can be calculated with macros
size_t output_buf_size = PSA_CIPHER_ENCRYPT_OUTPUT_SIZE(PSA_KEY_TYPE_AES, PSA_ALG_CBC_NO_PADDING,sizeof(PLAINTEXT));
uint8_t output_buffer[output_buf_size];

status = psa_cipher_encrypt(key_id, PSA_ALG_CBC_NO_PADDING, PLAINTEXT, sizeof(PLAINTEXT), output_buffer, sizeof(output_buffer), &output_length);
```

All the supported key types, algorithms and usage flags can be found in the documentation.

### Key Lifetime {#key-lifetime}
#### Volatile vs. Persistent
The PSA API specifies two ways of storing keys: volatile and persistent. Volatile
keys will be stored only in RAM, which means they will be destroyed after application
termination or a device reset.
Persistent keys will also be written into flash memory for later access. To destroy
them they must be explicitly deleted with the `psa_destroy_key()` function.

@note    Persistent key storage can be optionally enabled on `native` and on the `nRF52840dk`.
         For this, add `USEMODULE += psa_persistent_storage` to your application makefile
         or `CONFIG_MODULE_PSA_PERSISTENT_STORAGE=y` to your `app.config.test` file.
         Example: `tests/sys/psa_crypto_persistent_storage`

@warning Be aware that the current implementation writes keys in plain text to flash memory.
         Anyone with hardware access can read them.

#### Lifetime Encoding
When creating a key, the user needs to specify a lifetime value, which actually consists
of two values: persistence and location. The location defines the actual memory location
of the key (e.g. whether the key will be stored in RAM, in a hardware protected memory slot
or on an external device like a secure element).

The persistence value defines whether the key will be stored in RAM (volatile)
in flash (persistent).
Some default values that exist are:
- @ref PSA_KEY_LIFETIME_VOLATILE (stored in local, volatile memory)
- @ref PSA_KEY_LIFETIME_PERSISTENT (stored in local, persistent memory)

Other lifetime values can be constructed with the macro
`PSA_KEY_LIFETIME_FROM_PERSISTENCE_AND_LOCATION(persistence, location)`.
All supported `PSA_KEY_PERSISTENCE_*` and `PSA_KEY_LOCATION_*` values can be combined.

In addition to the location values defined by the specification, this implementation also
supports values for [Secure Elements](#secure-elements).

Configuration {#configuration}
===
Currently there are two ways to configure PSA Crypto: Kconfig and Makefiles. An example for both
can be found in `RIOT/examples/advanced/psa_crypto`.

## Kconfig
We recommend using Kconfig and choosing your features in `menuconfig`.
You can access the GUI by calling

```sh
TEST_KCONFIG=1 BOARD=<your board> make menuconfig
```

from your application directory.
There you can find the available PSA features and options under `System->PSA Crypto`.
If you only select the operations you want to use (e.g. `PSA Ciphers->AES-128 CBC`), Kconfig
will automatically select the best backend for you depending on the board (e.g. a hardware
accelerator if it is available). Optionally you can force a custom backend.

Further you can specify the exact number of keys you need to store (section `PSA Key Management
Configuration` in `menuconfig`), or choose your [Secure Element](#secure-elements)
configurations.

Alternatively you can create an `app.config.test` file in your application folder
and choose your symbols there (see `examples/advanced/psa_crypto`).

In the `app.config.test` file, modules can be chosen with the following syntax:
`CONFIG_MODULE_<MODULENAME>=y`, as shown below.
```kconfig
CONFIG_MODULE_PSA_CRYPTO=y
CONFIG_MODULE_PSA_CIPHER=y
CONFIG_MODULE_PSA_CIPHER_AES_128_CBC=y
```

## Makefiles
If you don't want to use Kconfig, you can use the traditional way in RIOT of selecting
modules in your application Makefile.

Here you need to set the base module and individual modules for each operation you need.
The example below also chooses a default backend depending on your board.
```makefile
// Base module: this is required!
USEMODULE += psa_crypto

USEMODULE += psa_cipher
USEMODULE += psa_cipher_aes_128_cbc
```

If desired, you can choose a specific backend at compile time. For this you need to specify
that you want to set a custom backend and then explicitly choose the one you want (see below).
```makefile
USEMODULE += psa_cipher_aes_128_cbc_custom_backend
USEMODULE += psa_cipher_aes_128_cbc_backend_riot
```

The currently available modules, are listed [below](#available-modules).

## Key Slot Types {#configuration-keys}
The key management of PSA keeps track of keys by storing them in virtual key slot
representations, along with their attributes. Since keys can come in various sizes,
it would be inefficient to allocate the same amount of memory for all keys.
To reduce the amount of memory used for key storage, PSA internally differentiates between
three types of key slots (see below). Depending on the operations your application uses, PSA will
automatically detect the key sizes needed and will allocate the required memory.
The number of key slots allocated of each type is set to five per default, but can be changed by
the user depending on their requirements.

| Single Key Slot | Asymmetric Key Slot | Protected Key Slot |
|----------------|---------------------|--------------------|
| Single keys or unstructured data,<br>e.g. AES keys or asymmetric<br>public keys in local memory | Asymmetric key pairs<br>(private and public parts) <br>in local memory | Any keys stored on a secure<br>element or on-chip in<br>hardware protected memory |

If you want to change the default number of allocated key slots you can do so by
updating the number in `menuconfig`, or adding them to the `app.config.test` file like so:
```kconfig
CONFIG_PSA_SINGLE_KEY_COUNT=3
CONFIG_PSA_ASYMMETRIC_KEYPAIR_COUNT=1
CONFIG_PSA_PROTECTED_KEY_COUNT=2
```

When using Makefiles, you can pass CFLAGS as shown below.
```makefile
CFLAGS += -DCONFIG_PSA_SINGLE_KEY_COUNT=3
CFLAGS += -DCONFIG_PSA_ASYMMETRIC_KEYPAIR_COUNT=1
CFLAGS += -DCONFIG_PSA_PROTECTED_KEY_COUNT=2
```

@note    The key slot count defines the maximum number of keys that can be cached in
         RAM at runtime. It does not limit the number of persistent keys that can be stored
         in flash memory. It is the user's responsibility to keep track of the number of
         persistently stored keys.

## Available Modules {#available-modules}
Below are the currently available modules.
No matter which operation you need, you always have to choose the base module.
If you want to specify a backend other than the default, you need to select
`psa_<operation>_custom_backend` in addition to the actual backend module.

The names listed are are the version used in makefiles with the
`USEMODULE += <modulename>` syntax.
In Kconfig you don't need to know the exact names, you can simply choose the features in
`menuconfig`.
When using `app.config.test` files in your application directory, you need to write the
names in uppercase and add the prefix `CONFIG_MODULE_` to all of them.

### Key Storage
- Persistent Key Storage: psa_persistent_storage

### Asymmetric Crypto
- Base: psa_asymmetric

#### NIST ECC P192
- psa_asymmetric_ecc_p192r1
- psa_asymmetric_ecc_p192r1_backend_periph
- psa_asymmetric_ecc_p192r1_custom_backend
- psa_asymmetric_ecc_p192r1_backend_microecc

#### NIST ECC P256
- psa_asymmetric_ecc_p256r1
- psa_asymmetric_ecc_p256r1_backend_periph

@warning Cryptocell 310 does not support public key derivation from a private key.

- psa_asymmetric_ecc_p256r1_custom_backend
- psa_asymmetric_ecc_p256r1_backend_microecc

#### Ed25519
- psa_asymmetric_ecc_ed25519
- psa_asymmetric_ecc_ed25519_backend_periph
- psa_asymmetric_ecc_ed25519_custom_backend
- psa_asymmetric_ecc_ed25519_backend_c25519

### AEAD
- Base: psa_aead

#### AES CCM
- psa_aead_aes_128_ccm
- psa_aead_aes_128_ccm_backend_periph
- psa_aead_aes_128_ccm_backend_cifra
- psa_aead_aes_128_ccm_backend_tinycrypt

@note    Be aware that the tinycrypt only allows a nonce size of 13.

- psa_aead_aes_128_ccm_custom_backend
- psa_aead_aes_192_ccm
- psa_aead_aes_192_ccm_backend_cifra
- psa_aead_aes_192_ccm_custom_backend
- psa_aead_aes_256_ccm
- psa_aead_aes_256_ccm_backend_cifra
- psa_aead_aes_256_ccm_custom_backend

### Ciphers
- Base: psa_cipher

#### AES ECB
- psa_cipher_aes_128_ecb
- psa_cipher_aes_128_ecb_backend_riot

#### AES CBC
- psa_cipher_aes_128_cbc
- psa_cipher_aes_128_cbc_backend_periph
- psa_cipher_aes_128_cbc_custom_backend
- psa_cipher_aes_128_cbc_backend_riot
- psa_cipher_aes_192_cbc
- psa_cipher_aes_192_cbc_custom_backend
- psa_cipher_aes_192_cbc_backend_riot
- psa_cipher_aes_256_cbc
- psa_cipher_aes_256_cbc_custom_backend
- psa_cipher_aes_256_cbc_backend_riot

#### CHACHA20
- psa_cipher_chacha20
- psa_cipher_chacha20_backend_periph
- psa_cipher_chacha20_custom_backend
- psa_cipher_chacha20_backend_riot

### Hashes
- Base: psa_hash

#### MD5
- psa_hash_md5
- psa_hash_md5_custom_backend
- psa_hash_md5_backend_riot

#### SHA 1
- psa_hash_sha_1
- psa_hash_sha_1_backend_periph
- psa_hash_sha_1_custom_backend
- psa_hash_sha_1_backend_riot

#### SHA 224
- psa_hash_sha_224
- psa_hash_sha_224_backend_periph
- psa_hash_sha_224_custom_backend
- psa_hash_sha_224_backend_riot

#### SHA 256
- psa_hash_sha_256
- psa_hash_sha_256_backend_periph
- psa_hash_sha_256_custom_backend
- psa_hash_sha_256_backend_riot

#### SHA 384
- psa_hash_sha_384
- psa_hash_sha_384_backend_periph
- psa_hash_sha_384_custom_backend
- psa_hash_sha_384_backend_riot

#### SHA 512
- psa_hash_sha_512
- psa_hash_sha_512_backend_periph
- psa_hash_sha_512_custom_backend
- psa_hash_sha_512_backend_riot

#### SHA 512/224
- psa_hash_sha_512_224
- psa_hash_sha_512_224_backend_periph
- psa_hash_sha_512_224_custom_backend
- psa_hash_sha_512_224_backend_riot

#### SHA 512/256
- psa_hash_sha_512_256
- psa_hash_sha_512_256_backend_periph
- psa_hash_sha_512_256_custom_backend
- psa_hash_sha_512_256_backend_riot

#### SHA 3/256
- psa_hash_sha3_256
- psa_hash_sha3_256_backend_periph
- psa_hash_sha3_256_custom_backend
- psa_hash_sha3_256_backend_riot

#### SHA 3/384
- psa_hash_sha3_384
- psa_hash_sha3_384_backend_periph
- psa_hash_sha3_384_custom_backend
- psa_hash_sha3_384_backend_riot

#### SHA 3/512
- psa_hash_sha3_512
- psa_hash_sha3_512_backend_periph
- psa_hash_sha3_512_custom_backend
- psa_hash_sha3_512_backend_riot

### MAC
- Base: psa_mac

#### HMAC SHA 256
- psa_mac_hmac_sha_256
- psa_mac_hmac_sha_256_backend_periph
- psa_mac_hmac_sha_256_custom_backend
- psa_mac_hmac_sha_256_backend_riot

### Secure Elements
Base:
- psa_secure_element
- psa_secure_element_multiple

#### SE Types
- psa_secure_element_ateccx08a
- psa_secure_element_ateccx08a_cipher_aes_128
- psa_secure_element_ateccx08a_ecc_p256
- psa_secure_element_ateccx08a_hmac_sha256

Random Number Generation {#rng}
===
Currently uses the [RIOT Random Module](#sys_random) as a backend.
See the documentation for configuration options.

Secure Elements {#secure-elements}
===

An example showing the use of SEs can be found in `examples/advanced/psa_crypto`.

To use secure elements, you first need to assign a static location value to each device,
so PSA can find it. If you only use one device, you can use
`PSA_KEY_LOCATION_PRIMARY_SECURE_ELEMENT`. For additional devices this value must be within
the range of `PSA_KEY_LOCATION_SE_MIN` and `PSA_KEY_LOCATION_SE_MAX`.
When booting the system, the `auto_init` module in RIOT will automatically register the device
with the location with PSA Crypto.

You can now import or create keys on the secure element by constructing a key lifetime containing
a device's location value.

```c
psa_key_lifetime_t lifetime =
     PSA_KEY_LIFETIME_FROM_PERSISTENCE_AND_LOCATION (PSA_KEY_LIFETIME_VOLATILE,
                                                        PSA_KEY_LOCATION_PRIMARY_SECURE_ELEMENT);
```

Some secure elements come with their own key management and device configurations. In this case
the configuration parameters must be passed to PSA Crypto during the registration. For this, you
need to define a `psa_se_config_t` structure containing the configuration.
PSA Crypto will use this structure to keep track of what types of keys are allowed on the device
and how much storage is available.
Where this structure should be placed, how it looks and what parameters are required depends
on the type of your device.

A good place to define that structure and the location values is a drivers `<driver>_params.h`
file, but this may vary depending on how your device is integrated in RIOT.

For detailed, device specific information, please check the device driver documentation or
the example.

## Available Devices and Drivers
- ATECCX08A: [Microchip Cryptoauthlib as a PSA backend](#psa-cryptoauthlib)

## Main SE Configuration
To use SEs, the appropriate modules must be chosen in Kconfig:
```kconfig
CONFIG_PSA_SECURE_ELEMENT=y
CONFIG_PSA_SECURE_ELEMENT_ATECCX08A=y        // device example
CONFIG_PSA_SECURE_ELEMENT_ATECCX08A_ECC_P256=y
```

or added to the Makefile:
```makefile
USEMODULE += psa_secure_element
USEMODULE += psa_secure_element_ateccx08a        // device example
USEMODULE += psa_secure_element_ateccx08a_ecc_p256
```

This implementation supports the use of one or more secure elements (SE) as backends. In this
case the number of used secure elements must be specified (must be at least 2 and at most 255).
When using more than one SE, add
```kconfig
CONFIG_PSA_SECURE_ELEMENT_MULTIPLE=y
CONFIG_PSA_MAX_SE_COUNT=2                    // or any other number between 2 and 255
```

or, respectively,

```makefile
USEMODULE += psa_secure_element_multiple
CFLAGS += -DCONFIG_PSA_MAX_SE_COUNT=2        // or any other number between 2 and 255
```

Porting Guide {#porting-guide}
===
This porting guide focuses on how to add your software library or hardware driver
as a backend to PSA Crypto without actually touching the PSA implementation.
We will provide some [general information](#porting-general) and then some case
examples for different kinds of backends:
- [Software Libraries](#porting-software)
- [Hardware Drivers](#porting-hardware)
- [Secure Elements](#porting-secure-elements)

Some examples to look at are:
- [RIOT hash module](#sys_hashes)
- [RIOT cipher module](#sys_crypto)
- [Micro-ECC](#pkg_micro_ecc)
- [CryptoCell 310 driver](#pkg_driver_cryptocell_310).

An example integrating a secure element can be found in the
[Cryptoauthlib Package](#pkg_cryptoauthlib).

## General Information {#porting-general}
### Error Values
You should always check the status of your function calls and translate your library's or
driver's errors to PSA error values (please be as thorough as possible).
The PSA Crypto specification describes exactly what kind of error values should be returned
by which function. Please read the API documentation and comply with the instructions.
We recommend writing a`<mylibrary>_to_psa_error()` function right in the beginning (see for
example `CRYS_to_psa_error()` in
`pkg/driver_cryptocell_310/psa_cryptocell_310/error_conversion.c`).

### The Build System
As mentioned before, there are two ways of selecting build time configurations in RIOT: Kconfig
and Makefiles.
Kconfig dependency resolution is currently an experimental feature and will at some point
replace Makefiles. Until then, our implementation needs to support both, which means we need
to define features and symbols in multiple places.
Luckily, the modules have the exact same names in both systems, which makes the transfer easier.
The examples below show both ways.

### Modules {#module-names}
In RIOT, module names are generated from path names, so if you create a directory for
your sourcefiles, the module name will be the same as the directory name. It is possible
to change that by declaring a new module name in the Makefile by adding the line
`MODULE := your_module_name`.

If you leave it like this, all sourcefiles in the path corresponding to the module name will be
built (e.g. if you choose the module `hashes`, all files in `sys/hashes` will be included).
For better configurability it is possible to add submodules (see
`sys/hashes/psa_riot_hashes` for example).
In that case the base module name will be the directory name and each file inside the directory
becomes its own submodule that must be explicitly chosen. The module name will then be the
directory name with the file name as a postfix.
For example:
```makefile
USEMODULE += hashes
USEMODULE += psa_riot_hashes
USEMODULE += psa_riot_hashes_sha_256
```

will build the file at `sys/hashes/psa_riot_hashes/sha_256.c`, but none of the other files in
the directory.

To enable submodules for your implementation add the following to the directory makefile:
```makefile
BASE_MODULE := psa_<modulename>
SUBMODULES := 1
```

We also need to create so-called pseudomodules for each available submodule.
Those must follow the scheme `psa_<modulename>_<filename>`.
Where they are declared depends on where your module is located. Pseudomodules in `RIOT/sys` must
be added in `pseudomodules.inc.mk`.
When integrating packages or drivers, the pseudomodules can be added in the `Makefile.include`
file of the individual module's directory (see `pkg/micro-ecc/Makefile.include`).

When adding backends to PSA Crypto, please name your modules in ways that fit within the
current naming scheme: `psa_<library>_<algorithm>`. Also, when adding software libraries and
hardware drivers, use the submodule approach. That makes PSA Crypto more configurable.

The drawback of the submodule approach is, that if one of our sourcefiles depends on
another sourcefile in the same folder, we need to select it explicitly. For example, in
`pkg/driver_cryptocell_310/psa_cryptocell_310` you can see that there are some common source
files that all the others use (e.g. for hashes there is a `hashes_common.c` file).

If that is the case for your driver, you need to make sure the modules are selected in
the Kconfig file as well as the `Makefile.dep` file (see `psa_cryptocell_310/Makefile.dep` or
`psa_cryptocell_310/Kconfig`).

### Adding Glue Code {#glue-code}
We define a number of wrapper APIs, which are called by PSA to invoke crypto backends.
Software libraries and hardware drivers use the same methods, secure elements are handled
in a different way (see [Case Example – Secure Elements](#porting-secure-elements) for details).

The names, parameters and return values for wrapper methods are defined in header files in
`sys/psa_crypto/include/psa_<algorithm>.h`.
The functions declared in those files are the ones that are currently supported by this
PSA implementation. They will be extended in the future.

You need to implement those functions with glue code calling your library or driver code
and converting types and error values between PSA and your backend.
Below is an example of how this might look (it's very reduced, your library may need
much more glue code).
```c
psa_status_t psa_ecc_p256r1_sign_hash(const psa_key_attributes_t *attributes,
                                      psa_algorithm_t alg, const uint8_t *key_buffer,
                                      size_t key_buffer_size, const uint8_t *hash,
                                      size_t hash_length, uint8_t *signature,
                                      size_t signature_size, size_t *signature_length)
{
    int status = <libraryname>_<sign_hash_func>(key_buffer, hash, hash_length,
                                           signature, signature_length, curve);

    if (status != SUCCESS) {
        return <libraryname>_status_to_psa_error(status);
    }

    (void)alg;
    (void)attributes;
    (void)key_buffer_size;
    return PSA_SUCCESS;
}
```

### Operation Contexts
Some cryptographic operations use driver specific context to store the operation state in
between function calls. These must be defined somewhere. Examples can be found in
`pkg/driver_cryptocell_310/include/psa_periph_hashes_ctx.h` and
`sys/include/hashes/psa/riot_hashes.h`.

When defining the contexts for a hardware driver, all you need to do is add a file called
`psa_periph_<algorithm>_ctx.h` to your driver's include folder and define the available types
(see supported [types](#supported-types) below).
Those files are automatically included in `crypto_includes.h` and it is important that they
always have the same name for each algorithm.

When defining the contexts for a software library, the headerfile should be called
`<library>_<algorithm>.h` (e.g. `riot_hashes.h`) and must be added to `crypto_includes.h` as
shown below:
```c
#if IS_USED(MODULE_PSA_<LIBRARY>_<ALGORITHM>)
#include "<library>/<library>_<algorithm>.h"
#endif
```

When defining the context types, those must always depend on the specific algorithm module,
for example
```c
#if IS_USED(MODULE_PSA_<LIBRARY>_HASHES_SHA_256)
#include "path/to/headerfile_containing_the_driver_context_definition"

typedef <library_context_type_t> psa_hashes_sha256_ctx_t;
#endif
```

#### Hashes {#supported-types}
- `psa_hashes_md5_ctx_t`
- `psa_hashes_sha1_ctx_t`
- `psa_hashes_sha224_ctx_t`
- `psa_hashes_sha256_ctx_t`
- `psa_hashes_sha384_ctx_t`
- `psa_hashes_sha512_ctx_t`
- `psa_hashes_sha512_224_ctx_t`
- `psa_hashes_sha512_256_ctx_t`

#### Ciphers
- `psa_cipher_aes_128_ctx_t`
- `psa_cipher_aes_192_ctx_t`
- `psa_cipher_aes_256_ctx_t`

Secure Elements need their own contexts. For this,
see [Case Example – Secure Elements](#porting-secure-elements).

## Adding a Backend
The integration of hardware drivers, software libraries and secure element drivers
differs a bit. Below we describe the necessary steps for each of them.

### Case Example – A Software Library {#porting-software}
Software libraries are the easiest backends, because they are not platform or hardware
specific. They can generally run on all platforms in RIOT and we can
combine different software backends for different operations (we could, for example,
use the Micro-ECC package for ECC NIST curves and the C25519 package for operations with
the Curve25519).

Let's say we have an imaginary software library called `FancyCrypt` and want to use
it as a backend of PSA. We've already added it to RIOT as a third party package in
`pkg/fancycrypt`.
Our library provides hashes and elliptic curve operations and to make it accessible to
PSA Crypto we need to write wrappers for our API calls.

First we create a folder called `psa_fancycrypt` in the package directory. Inside we create
a file with the name of each operation you want to integrate, e.g. `p256.c` and
`hashes_sha_224.c` (when adding operations, remember that the path of the files will also
be the [module name](#module-names), so please comply with the current naming scheme).

In these files we need to implement the methods that are called by PSA as described
[above](#glue-code).

#### Adding Makefiles
We add a Makefile to the `psa_fancycrypt` folder with the following content:
```c
BASE_MODULE := psa_fancycrypt
SUBMODULES := 1

include $(RIOTBASE)/Makefile.base
```

This tells RIOT that the `psa_fancycrypt` module has submodules, which can be selected
individually.

In `pkg/fancycrypt` we now need to declare explicit pseudomodules in `Makefile.include` and add
the `psa_fancycrypt` folder to the source files and the `sys/psa_crypto/include` folder to the
includes.
These should be dependent on the PSA Crypto module as shown below.

```makefile
ifneq (,$(filter psa_fancycrypt_%, $(USEMODULE)))
    PSEUDOMODULES += psa_fancycrypt_hashes_sha_256
    PSEUDOMODULES += psa_fancycrypt_p256
    DIRS += $(RIOTPKG)/fancycrypt/psa_fancycrypt
    INCLUDES += -I$(RIOTBASE)/sys/psa_crypto/include
endif
```

If the implementation has any dependencies, they need to be added in `Makefile.dep`, for example:
```makefile
USEMODULE += psa_fancycrypt
USEMODULE += psa_fancycrypt_error_conversion

ifneq (,$(filter psa_fancycrypt_hashes_sha1,$(USEMODULE)))
  USEMODULE += psa_fancycrypt_hashes_common
endif
```

#### Adding a Kconfig file
We add a file called `Kconfig` to the `psa_fancycrypt` folder. Here we declare
the modules for Kconfig like so:
```kconfig
config MODULE_PSA_FANCYCRYPT_HASHES_SHA_256
    bool
    depends on MODULE_PSA_CRYPTO
    select MODULE_PSA_FANCYCRYPT

config MODULE_PSA_FANCYCRYPT_P256
    bool
    depends on MODULE_PSA_CRYPTO
    select MODULE_PSA_FANCYCRYPT

config MODULE_PSA_FANCYCRYPT
    bool
```

If the implementation has any dependencies, we can select them in this Kconfig file:
```kconfig
config MODULE_PSA_FANCYCRYPT_HASHES_SHA_256
    bool
    depends on MODULE_PSA_CRYPTO
    select MODULE_PSA_FANCYCRYPT
    select MODULE_PSA_FANCYCRYPT_HASHES_COMMON
    select MODULE_PSA_FANCYCRYPT_ERROR_CONVERSION
```

In `pkg/fancycrypt/Kconfig` we need to add the line
```kconfig
rsource "psa_fancycrypt/Kconfig"
```
at the bottom.

#### Telling PSA Crypto about it
To be able to choose `fancycrypt` as a PSA backend, we need to add the option to the Kconfig
and Makefiles of the PSA Crypto Module.

In `sys/psa_crypto/` we need to modify `Kconfig.asymmetric`, `sys/psa_crypto/Kconfig.hashes`,
`Makefile.dep` and `Makefile.include`.

To `Kconfig.asymmetric` we need to add
```kconfig
config MODULE_PSA_ASYMMETRIC_ECC_P256R1_BACKEND_FANCYCRYPT
    bool "FancyCrypt Package"
    select PACKAGE_FANCYCRYPT
    select MODULE_PSA_FANCYCRYPT_P256
```
This will expose FancyCrypt as a backend option in PSA and then enable all the necessary
features, when users select it.
You need to do the same thing for the hash operation in `Kconfig.hashes`.

To achieve the same thing with Makefiles we need to do this in two places:
In `Makefile.include` there are some existing pseudomodules for asymmetric crypto and hashes.
There we need to create the backend modules for FancyCrypt by adding

```makefile
PSEUDOMODULES += psa_asymmetric_ecc_p256r1_backend_fancycrypt
```

and

```makefile
PSEUDOMODULES += psa_hash_sha_256_backend_fancycrypt
```

The automatic module selection happens in `Makefile.dep`. To the place where exiting P256 curves
and hashes are selected we add cases for our backend modules:

```makefile
ifneq (,$(filter psa_asymmetric_ecc_p256r1_backend_fancycrypt,$(USEMODULE)))
    USEPKG += fancycrypt
    USEMODULE += psa_fancycrypt
    USEMODULE += psa_fancycrypt_p256
endif
```

Now you should be able to select your package as a backend for PSA Crypto and use it to perform
operations.

### Case Example – A Hardware Driver {#porting-hardware}
The first steps of porting a hardware driver are the same as for the software library.
Only we skip the last part where we add the modules to the PSA Crypto Kconfig and Makefiles
and do something else instead.

Hardware drivers are treated a little differently, mostly because they are tied to a specific
platform and users can not just choose a different driver for their accelerator.
Therefore we just want PSA Crypto to automatically use this driver whenever it runs on the
corresponding platform, which means that we have to add some additional options and features,
not only to the driver but also to the CPU it belongs to.
A good example for this is the [CryptoCell 310 driver](#pkg_driver_cryptocell_310) for the
accelerator on the [nRF52840 CPU](#cpu_nrf52).

Now, let's say we have a CPU called `myCPU` with an on-chip accelerator called
`speedycrypt`. Let's say that `speedycrypt` provides hashes and ECC curves.
The vendor provides a driver, which we already have included in RIOT as a package.
Also we've followed the steps in the [glue code section](#glue-code) and provide a folder called
`pkg/driver_speedycrypt/psa_speedycrypt` with the required wrapper files.
We have also added the module names in a Kconfig file and in the Makefiles.

#### Telling PSA Crypto about it
This is where we diverge from the software library example. If you take a look at the available
backends in PSA, you'll notice one with the postfix `*_BACKEND_PERIPH` for each available
algorithm. **Periph** here is short for *peripheral hardware accelerator*.
The `*_BACKEND_PERIPH` modules depend on the presence of such an accelerator. They are a generic
module for all crypto hardware accelerators and will automatically resolve to the driver that is
associated with the available accelerator.

Before we're able to use it we need to tell RIOT that those hardware features exist for
our `myCPU` (see `cpu/nrf52/Kconfig` and `cpu/nrf52/Makefile.features` as an example).
In `cpu/myCPU` we add all the provided features as shown below.

Files we need to touch:
- `cpu/myCPU/Makefile.features`
- `cpu/myCPU/Kconfig`
- `cpu/myCPU/periph/Makefile.dep`
- `cpu/myCPU/periph/Kconfig`
- When defining new features: `RIOT/kconfigs/Kconfig.features`

**cpu/myCPU/Makefile.features:**
```makefile
FEATURES_PROVIDED += periph_speedycrypt      // General feature for the accelerator
FEATURES_PROVIDED += periph_hash_sha_256
FEATURES_PROVIDED += periph_ecc_p256r1
```

**cpu/myCPU/Kconfig:**
```kconfig
config CPU_FAM_MYCPU
    bool
    select CPU_SOME_FEATURES
     ...
    select HAS_PERIPH_HASH_SHA_256
    select HAS_PERIPH_ECC_P256R1
    select HAS_PERIPH_SPEEDYCRYPT
```
The `HAS_PERIPH_*` symbols are defined in ``. If your device
provides capabilities that are not yet defined, you can add them to that file.

Next we need to define selectable modules for this in the `cpu/myCPU/periph` folder, which
then automatically enable the driver. An example for this is `cpu/nrf52/periph`.
We add the following to the `cpu/myCPU/periph/Kconfig` file and `cpu/myCPU/periph/Makefile.dep`:

**cpu/myCPU/periph/Makefile.dep:**
```makefile
ifneq (,$(filter periph_hash_sha_256,$(USEMODULE)))
    USEPKG += driver_speedycrypt
    USEMODULE += psa_speedycrypt_hashes_sha256
endif
```

**cpu/myCPU/periph/Kconfig:**
```kconfig
config MODULE_PERIPH_FANCYCRYPT
    bool
    depends on HAS_PERIPH_FANCYCRYPT
    select PACKAGE_DRIVER_FANCYCRYPT

config MODULE_PERIPH_HASH_SHA_256
    bool
    depends on HAS_PERIPH_HASH_SHA_256
    select MODULE_PERIPH_SPEEDYCRYPT
    select MODULE_PSA_SPEEDYCRYPT_HASHES_SHA256
```

Here we basically say "If the user chooses the `periph_hash_sha_256 module`, also select the
`periph_speedycrypt` feature, which will then enable the speedycrypt driver". Of course you need
to do this for all your available features.

Now, if you build PSA Crypto with default configurations, it should automatically detect that
your board has a hardware accelerator for hashes and ECC operations and build the hardware
driver as a backend.

### Case Example – A Secure Element Driver {#porting-secure-elements}
Secure elements (SEs) are handled almost completely separate from the other backends. When we use
software libraries or hardware drivers, we only build one implementation per algorithm.
When it comes to secure elements we want to be able to build them in addition to the other
backends and we may want to connect and use more than one of them at the same time.
Another difference is that when using software libraries and hardware drivers, PSA handles the
storage of key material. When using SEs, keys are stored on the SE, which means, we need
additional functionality for the key management.

An existing example in RIOT is the Microchip ATECCX08A device family, whose driver can be found
in `pkg/cryptoauthlib`.

PSA Crypto has an integrated SE driver registry, which stores all registered drivers in a list.
When an application calls a cryptographic operation that's supposed to be performed by a secure
element, the registry will find the correct driver in the list and PSA will invoke the operation.
Each driver is stored with a context that contains persistent as well as transient driver data.
Transient driver data can be anything the driver needs to function. Persistent data is supposed
to be used to keep track of how many keys are stored on the device and if there is still some
free space available.

@note    Currently PSA does not support persistent storage, so the persistent driver data
         is not really persistent, yet. Once persistent storage is implemented, this data
         will be stored, so the implementation can find already existing keys again after
         a reboot.

For this example we integrate an imaginary SE called `superSE`, which comes with a driver called
`superSE_lib`. Again, we assume that we have already added the driver as a package in RIOT and it
can be found at `pkg/superse_lib`.

#### Adding the Glue Code
Secure element drivers need to implement a different API than the other backends. It is defined
[here](#sys_psa_crypto_se_driver).
In our package folder we now create a new folder called `psa_superse_driver` and add a source
file called `psa_superse_lib_driver.c`. Here we now implement glue code for all the cryptographic
operations our SE supports.

You will notice that the SE interface also provides some key management functions. This is
because keys are stored on the device and PSA can not access the memory and key data itself,
but needs to tell the driver to do it.

### Operation Contexts
Some operations need driver specific contexts. For secure elements these are wrapped in types
defined in `crypto_contexts.h` (currently only `psa_se_cipher_context_t` is supported).
In this header file add operation contexts that belong to your driver to the available SE
context unions as shown in the example below:

```c
typedef struct {
    union driver_context {
        unsigned dummy;
    #if IS_USED(MODULE_PSA_SECURE_ELEMENT_ATECCX08A) || defined(DOXYGEN)
        atca_aes_cbc_ctx_t atca_aes_cbc;
    #endif
    #if IS_USED(MODULE_PSA_SECURE_ELEMENT_SUPERSE) || defined(DOXYGEN)
        superse_cipher_ctx_t superse_aes_cbc;
    #endif
    } drv_ctx;
} psa_se_cipher_context_t;
```

#### Allocation
The first thing PSA will do, when an application creates a key on an SE, is ask the driver to
find a free key slot on the device. This is what the `allocate` function is for. How exactly
the slot is allocated, depends on the driver.
It may be possible to query that information directly from the device. If that is not possible,
we can use the persistent data stored in the driver context. An example for this can be
found in `pkg/cryptoauthlib/psa_atca_driver/psa_atca_se_driver.c`.
This example requires the user to provide information about the configurations for each key slot,
which is then stored in the persistent driver data and used for key management (for a better
description read [Using Cryptoauthlib as a backend for PSA Crypto](#psa-cryptoauthlib)).
At this point you can decide what the best approach for your device is.

The `allocate` function should then return some reference to the slot it has allocated
for the key (possibly a pointer or a slot number). Next PSA Crypto will invoke the `import`
or `generate` function to store a key.

#### Using Persistent Data
When you want to use persistent data to keep track of keys, you should utilize the
`psa_se_config_t` structure, which is declared in `crypto_se_config.h`.
You can define a structure that can hold your device configuration and make sure it is available
then your SE is used.

#### Making the Methods Available
At the bottom of the wrapper code, define structures with pointers to the available methods.
For example if you have implemented a `superse_allocate` and `superse_generate_key` function,
you need to add a `psa_drv_se_key_management_t` structure as shown below. Fill the unimplemented
methods with `NULL` pointers.
The last structure should be a `psa_drv_se_t` struct containing pointers to the other structures.
That one will be stored during driver registration to get access to all the implemented
functions.

```c
static psa_drv_se_key_management_t superse_key_management = {
    .p_allocate = superse_allocate,
    .p_validate_slot_number = NULL,
    .p_import = NULL,
    .p_generate = superse_generate_key,
    .p_destroy = NULL,
    .p_export = NULL,
    .p_export_public = NULL
};

psa_drv_se_t superse_methods = {
    .hal_version = PSA_DRV_SE_HAL_VERSION,
    .persistent_data_size = 0,
    .p_init = NULL,
    .key_management = &superse_key_management,
    .mac = NULL,
    .cipher = NULL,
    .aead = NULL,
    .asymmetric = NULL,
    .derivation = NULL
};
```

You should do this for all available functions. The structures for the functions are
declared in `sys/psa_crypto/include/psa_crypto_se_driver.h`.

#### Driver Registration
At start-up all secure element drivers need to be registered with the PSA SE management module.
This happens by calling `psa_register_secure_element()` during the automatic driver
initialization in RIOT.
When you added support for our device to RIOT, you should have implemented an
`auto_init_<device>` function, which initializes the connected devices.
In this function, after initializing a device, you should call `psa_register_secure_element()`
and pass the device's location value, and pointers to the `psa_drv_se_t` structure,
the persistent data and some device specific context.
An example implementation of this can be seen in `sys/auto_init/security/auto_init_atca.c`.

#### Telling PSA Crypto about it
To be able to choose our `superSE` during configuration, we need to define the corresponding
modules in the Kconfig files and Makefiles.

To `pkg/super_se_lib/Kconfig` we add something like
```kconfig
config MODULE_PSA_SUPERSE_DRIVER
    bool
    depends on PACKAGE_SUPERSE_LIB
    default y if MODULE_PSA_CRYPTO
    select PSA_KEY_MANAGEMENT
```
This tells the build system that whenever this driver and PSA Crypto are used at the same time,
the wrapper and the PSA key management module are needed, too.

To `sys/psa_crypto/psa_se_mgmt/Kconfig` we add a menu for the SE like so:
```kconfig
menuconfig MODULE_PSA_SECURE_ELEMENT_SUPERSE
    bool "Our Vendor's SuperSE"
    select PACKAGE_SUPERSE_LIB
    depends on <whatever protocol is needed for communication, e.g. HAS_PERIPH_I2C>
    help
        <Some helpful information about this module>
```
This makes our driver selectable whenever an application configuration selects the PSA secure
element module.

As described in the [Configuration Section](#configuration-keys), references to keys on secure
elements are stored by PSA in a different type of key slot than other keys.
The slot for protected keys usually only contains a slot number or address and not the actual
key, which requires a lot less memory space.

**BUT:** If your secure element supports asymmetric cryptography and exports a public key part
during key generation, that key part must be stored somewhere. So when you choose an
asymmetric operation, the protected key slots will have the space to store a public
key.

#### Dependencies
Secure Element operations also depend on the PSA modules. E.g. when you want to use an ECC
operation, you need to make sure that you also build the asymmetric PSA functions.

For this we need to add the following to the `superSE` menu:
```kconfig
config MODULE_PSA_SECURE_ELEMENT_SUPERSE_ECC_P256
    bool "Our Vendor's Elliptic Curve P256"
    select PSA_KEY_SIZE_256
    select MODULE_PSA_ASYMMETRIC
    depends on MODULE_PSA_SECURE_ELEMENT_SUPERSE
```
This tells us, what size a key slot should have to store the public key. If your SE supports
other curves, you need to modify this accordingly or add more of them.

Now we need to add the same to the Makefiles. In `Makefile.include` we add the source file path
and the PSA include folders and define the new available pseudomodules:
```makefile
ifneq (,$(filter psa_crypto,$(USEMODULE)))
    DIRS += $(RIOTPKG)/superse_lib/psa_superse_driver
    INCLUDES += -I$(RIOTBASE)/sys/psa_crypto/include
    PSEUDOMODULES += psa_secure_element_superse
    PSEUDOMODULES += psa_secure_element_superse_ecc_p256
endif
```

In `Makefile.dep` we automatically add required modules when PSA Crypto and the ECC curve
module are chosen:
```makefile
ifneq (,$(filter psa_crypto,$(USEMODULE)))
  USEMODULE += psa_superse_driver
endif

ifneq (,$(filter psa_secure_element_superse_ecc_p256, $(USEMODULE)))
  USEMODULE += psa_asymmetric
endif
```
This needs to be done for all other supported operations (e.g. ATECCX08 operations in
`pkg/cryptoauthlib/Makefile.include`, `pkg/cryptoauthlib/Makefile.dep` and
`sys/psa_crypto/psa_se_mgmt/Kconfig`). Now the secure element should be available for use
with PSA Crypto.
