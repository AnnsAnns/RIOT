# Examples

RIOT provides a wide range of examples to demonstrate the capabilities of the OS and its modules. 

The examples are organized in subdirectories of the `examples` directory. Each example directory contains a `README.md` file that provides information about the example and how to run it.

Here is a quick overview of the examples available in the RIOT repository:

### The Essentials

- [hello-world](./hello-world)
  - A simple "Hello World" example that shows the basic structure of a RIOT application.
- [blinky](./blinky)
  - The classic "Hello World" example for embedded systems: blinking an LED (or printing "Blink!" via stdio when none are available).
- [telnet_server](./telnet_server)
  - Simple telnet server that listens on port 23 over IPv6.
- [riot_and_cpp](./riot_and_cpp)
  - Example of using C++ in RIOT applications.
- [timer_periodic_wakeup](./timer_periodic_wakeup)
  - How to set up a periodic wakeup timer using the RIOT operating system.
- [ipc_pingpong](./ipc_pingpong)
  - This example is to illustrate the usage of RIOTs IPC messaging system.

### Networking & BLE Examples

#### Constraint Application Protocol (CoAP)

- [gcoap](./gcoap)
  - This example demonstrates the usage of the `gcoap` module, a high-level API for CoAP (Constrained Application Protocol) messaging.
- [gcoap_block_server](./gcoap_block_server)
  - CoAP server handling for Block requests, build with gcoap using nanocoap block handling functions.

#### Bluetooth Low Energy (BLE)

- [nimble_scanner](./nimble_scanner)
  - This example showcases the usage of the `NimBLE` BLE stack as a scanner.
- [skald_eddystone](./skald_eddystone)
  - This example demonstrates the usage of `Skald` for creating an Google `Eddystone` beacon.

#### Low-Power Wide-Area Network (LoRaWAN)

- [gnrc_lorawan](./gnrc_lorawan)
  - Send and receive LoRaWAN packets and perform basic LoRaWAN commands

#### MQTT

- [emcute_mqttsn](./emcute_mqttsn)
  - This application demonstrates the usage of the emCute (MQTT-SN) module in RIOT.

#### CoRE Resource Directory
- [cord_lc](./cord_lc)
  - Example of RIOT's Resource Directory (RD) lookup module, called `cord_lc`
- [cord_epsim](./cord_epsim)
  - This example shows how a node can register with a CoRE resource directory

#### Misc

- [spectrum-scanner](./spectrum-scanner)
  - This example demonstrates how to monitor energy levels on all available wireless channels
- [dtls-sock](./dtls-sock)
  - This example shows how to use DTLS sock `sock_dtls_t`

### Advanced Examples

- [posix_select](./posix_select)
  - This example is a showcase for RIOT's POSIX select support
- [micropython](./micropython)
  - How to use the (incomplete) MicroPython port for RIOT.
- [psa_crypto](./psa_crypto)
  - Basic functions of the PSA Crypto API

### Specialized/Unique Examples

- [javascript](./javascript)
  - 
- [lua_basic](./lua_basic)
  - 
- [asymcute_mqttsn](./asymcute_mqttsn)
  - 
- [thread_duel](./thread_duel)
  - 
- [suit_update](./suit_update)
  - 
- [tests-with-config](./suit_update/tests-with-config)
  - 
- [posix_sockets](./posix_sockets)
  - 
- [gnrc_border_router](./gnrc_border_router)
  - 
- [opendsme](./opendsme)
  - 
- [pio_blink](./pio_blink)
  - 
- [rpx0xx_pio_blink](./pio_blink/rpx0xx_pio_blink)
  - 
- [arduino_hello-world](./arduino_hello-world)
  - 
- [cord_ep](./cord_ep)
  - 
- [default](./default)
  - 
- [lwm2m](./lwm2m)
  - 
- [openthread](./openthread)
  - 
- [gnrc_networking_subnets](./gnrc_networking_subnets)
  - 
- [paho-mqtt](./paho-mqtt)
  - 
- [twr_aloha](./twr_aloha)
  - 
- [tests-with-config](./twr_aloha/tests-with-config)
  - 
- [gcoap_fileserver](./gcoap_fileserver)
  - 
- [dtls-wolfssl](./dtls-wolfssl)
  - 
- [nimble_heart_rate_sensor](./nimble_heart_rate_sensor)
  - 
- [rust-hello-world](./rust-hello-world)
  - 
- [src](./rust-hello-world/src)
  - 
- [ccn-lite-relay](./ccn-lite-relay)
  - 
- [usbus_minimal](./usbus_minimal)
  - 
- [gcoap_dtls](./gcoap_dtls)
  - 
- [gnrc_minimal](./gnrc_minimal)
  - 
- [bindist](./bindist)
  - 
- [abc](./bindist/abc)
  - 
- [nanocoap_server](./nanocoap_server)
  - 
- [leds_shell](./leds_shell)
  - 
- [gnrc_networking_mac](./gnrc_networking_mac)
  - 
- [rust-gcoap](./rust-gcoap)
  - 
- [src](./rust-gcoap/src)
  - 
- [saul](./saul)
  - 
- [wasm](./wasm)
  - 
- [wasm_sample](./wasm/wasm_sample)
  - 
- [rust-async](./rust-async)
  - 
- [src](./rust-async/src)
  - 
- [lua_REPL](./lua_REPL)
  - 
- [gnrc_networking](./gnrc_networking)
  - 
- [filesystem](./filesystem)
  - 
- [dtls-echo](./dtls-echo)
  - 
- [senml_saul](./senml_saul)
  - 
- [nimble_gatt](./nimble_gatt)
  - 
- [sniffer](./sniffer)
  - 
- [tools](./sniffer/tools)
  - 
- [skald_ibeacon](./skald_ibeacon)
  - 
- [lorawan](./lorawan)
  - 
- [tests-with-config](./lorawan/tests-with-config)
