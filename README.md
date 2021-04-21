# pistop

[![pistop C/C++ CI](https://github.com/mbhangui/pistop/actions/workflows/pistop-c-cpp.yml/badge.svg)](https://github.com/mbhangui/pistop/actions/workflows/pistop-c-cpp.yml)

I use a Raspberry PI4 to provide NFSv4 and Samba shares. This allows me to share my Music Directory to all clients. These clients are mostly Single Board Computers (SBCs), mix of [Raspbery PIs](https://en.wikipedia.org/wiki/Raspberry_Pi), [Banana PIs](https://en.wikipedia.org/wiki/Banana_Pi), [Allo sparky SBC](https://www.allo.com/sparky/sparky-sbc.html). All of these mount the filesystem using automount. Music playback is done via Music Player Daemon. These SBCs have been installed in each of my room (Living Room, Music Room, two bedrooms and one device which is connected to a Headphone amp. The power supply of these boards are connected to WiFI switches which can be remotely turned off.

The problem **pistop** solves is a synchronized mounting of the shared Music Directory, starting of mpd and automount service as soon as the PI4 server is powered on and up. This is achieved by having a service known as **fclient** on the clients and a service known as **fserver** on the NFSv4 server (PI4). These services use [supervise](https://en.wikipedia.org/wiki/Daemontools), **tcpclient**, **tcpserver** from the [ucspi-tcp](https://cr.yp.to/ucspi-tcp.html) package. I use the `fclient` service on my laptop too. It automatically starts the mpd daemon, the moment it sees the NFSv4 server (PI4) up running the `fserver` service. In case, I'm done with listening to Music, I shutdown the PI4 server. The `fclient` service detects that and stops the automount service, unmounts the NFS share and stops the mpd daemon. This prevents programs like `ls` from hanging when doing a listing of your home directory.

So when I retire for the night to get a good night sleep, I use an android app known as `ssh button` to shutdown the NFSv4 server. The moment the server shuts down, all my clients shutdown together. Using home automation, I then switch off the power output to each and very adaptor thereby saving around 0.05 kwhr of electricity per device every day. You could say that these SBC consume miniscule power, but this kind of philosophy has led us mankind to plunder this planet. Global Warming is a stark reality of the day and we should try to conserve electricity as much as possible.

You can take a look at the files, configuration for my server and client in the directory [example_config](https://github.com/mbhangui/pistop/tree/master/example_config).

## INSTALLATION

On the server (use the IP Address of your own server)

```
# apt-get update
# apt-get install pistop
# /usr/libexec/pistop/create_service --servicedir=/service --service_name=fserver --host=192.168.1.1 --port=5555 --add-service
```

On each client (use IP Address of your server)

```
# apt-get update
# apt-get install pistop
# /usr/libexec/pistop/create_service --servicedir=/service --service_name=fclient --host=192.168.1.1 --port=5555 --add-service
```

If you want to have your  clients shutdown itself when the server is powered off, create `POWER_OFF` enviroment variable by doing this
`sudo sh -c "echo 1 > /service/fclient/variables/POWER_OFF"`

**Client AUTOMOUNT SETUP**

```
file /etc/auto.master.d/mpd.autofs
/var/lib/mpd /etc/autofs.mounts timeout=60

file /etc/autofs.mounts
MDrive -fstype=nfs,vers=4,soft,timeo=5,retry=1 pi4:/home/pi/MDrive
```

## Prebuilt Binaries

Prebuilt binaries using openSUSE Build Service are available for pistop for

* Debian (including ARM images for Debian 10 which work (and tested) for RaspberryPI (model 2,3 & 4) and Allo Sparky)
* openSUSE Tumbleweed
* Ubuntu
* Arch Linux
* Fedora

Use the below url for installation

https://software.opensuse.org//download.html?project=home%3Ambhangui%3Adietpi&package=pistop
