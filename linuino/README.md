Linuino
=======

This folder contains the Linuino component of LibreXC, which

* creates and updates Log-files accordingly to data sent by the Arduino component,
* manages a Socket-Api used for receiving logging-/administrative-commands.

================
### Dependencies
The following dependencies are needed globally

* Python 2.7 (arduino-yun's default version)

Dev dependencies
* Virtualenv (https://virtualenv.readthedocs.org/en/latest/)


=====================
### Development Setup

Create a new virtualenv with Python 2.7 and activate
```Shell
$  virtualenv -p /usr/bin/python2.7 venv
$  . venv/bin/activate
```

Run 'main.py' inside your virtualenv
```Shell
(venv)[linuino]$  python main.py
```

After bootstrapping you can access the Socket-Api on '127.0.0.1:9001'. For low-level WebSocket communication we recommend [wscat](https://github.com/websockets/wscat) during development. While connecting to the Socket you need to transmit two additional authentication-headers ('user' and 'password'). All important parameters can be changed inside 'config.ini'

```Shell
$  wscat -c ws://127.0.0.1:9001 -H user:admin -H password:admin
```

=================
### Install & Run

Checkout the current project-state from this repository and copy it to your Linuino's sd-card.
```Shell
$  [libreXC] scp config.ini arduino@192.168.240.1:/mnt/sda1     # copy config
$  [libreXC] scp -r ./linuino arduino@192.168.240.1:/mnt/sda1       # copy code
```

Connect to your Linunio via 'ssh' and execute 'main.py'
```Shell
$  ssh ardunio@192.168.240.1 && python /mnt/sda1/linunio/main.py
```

You are now able to connect to the Socket on '127.0.0.1:9001' with the authentication-headers 'user:admin' and 'password:admin' (can be changed inside 'config.ini').

All available Socket-commands can be found in the [Comm Spec Linuino Client](https://github.com/deshi-basara/libreXC/wiki/Comm-Spec--Linuino---Client).

```Shell
> available_pids
> read_pid 10
```
