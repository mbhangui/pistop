# pistop

[![pistop C/C++ CI](https://github.com/mbhangui/pistop/actions/workflows/pistop-c-cpp.yml/badge.svg)](https://github.com/mbhangui/pistop/actions/workflows/pistop-c-cpp.yml)

I use a Raspberry PI4 to provide NFSv4 and Samba shares. An external 3TB hard disk is connected to the USB3 port of the PI4 as /MDrive. This disk has the following important directories at the root

1. Music     - This has all the music files
2. data      - This stores all data for [mpdev](https://github.com/mbhangui/mpdev). mpdev is used for storing the play counts, ratings, karma (a value indicating how much a particular song is liked by you).
3. playlists - This stores all playlists
4. cache     - The mpd config uses this directory for storing the mpd database information

This scheme allows me to share my Music Directory to all clients. They all mount <i>/MDrive</i> of PI4 at <i>/var/lib/mpd/MDrive</i> to access the <u>data</u> and <u>Music</u> directories on the drive. These clients are mostly Single Board Computers (SBCs), mix of [Raspbery PIs](https://en.wikipedia.org/wiki/Raspberry_Pi), [Banana PIs](https://en.wikipedia.org/wiki/Banana_Pi), [Allo sparky SBC](https://www.allo.com/sparky/sparky-sbc.html). All of these mount the filesystem using automount. Music playback on clients is done via [Music Player Daemon](https://www.musicpd.org/). These SBCs have been installed in each of my room (Living Room, Music Room, two bedrooms and one device which is connected to a Headphone amp. The power supply of these boards are connected to WiFI switches which can be remotely turned off. The PI4 also runs mpd, but the configuration provides proxy database service for all clients. When you add music, it needs to be added to external hard disk mounted on PI4. mpd automatically updates it's database. Nothing needs to be done on the clients. They all get the updated database instantly.

```
# mpd.conf database configuration entry for PI4
music_directory     "/MDrive/Music"
playlist_directory  "/MDrive/playlists"
database {
  plugin          "simple"
  path            "/MDrive/data/tag_cache.PI4"
  cache_directory "/MDrive/cache"
}

# mpd.conf database configuration entry for client
music_directory     "/var/lib/mpd/MDrive/Music"
playlist_directory  "/var/lib/mpd/MDrive/playlists"
database {
    plugin          "proxy"
    host            "MusicPI"
    port            "6600"
}

The MusicPI above refers to host in /etc/hosts or the IP address the PI4 server. For me it is
192.168.2.101   MusicPI
```

The problem **pistop** solves is a synchronized mounting of the shared Music Directory, starting of mpd and automount service as soon as the PI4 server is powered on and up. This is achieved by having a service known as **fclient** on the clients and a service known as **fserver** on the NFSv4 server (PI4). These services use [supervise](https://en.wikipedia.org/wiki/Daemontools), **tcpclient**, **tcpserver** from the [ucspi-tcp](https://cr.yp.to/ucspi-tcp.html) package. I use the `fclient` service on my laptop too. It automatically starts the mpd daemon, the moment it sees the NFSv4 server (PI4) up running the `fserver` service. In case, I'm done with listening to Music, I shutdown the PI4 server. The `fclient` service detects that and stops the automount service, unmounts the NFS share and stops the mpd daemon. This prevents programs like `ls` from hanging when doing a listing of your home directory.

There are two other features of **pistop**.

* Backing up configuration and important files. You need to create environment variables <b>BACKUP</b> and <b>DEST</b> in <u>/service/fclient/variables</u> or <u>/service/fserver/variables</u> directory. The <b>fserver</b> runs on your file server while the <b>fclient</b> runs on the client devices.
* Automatic update of your device. You need to have packages that need automatic updation in the environment variable <b>UPGRADE</b> in <u>/service/update/variables</u> directory. The at(1) command is used to update packages. You can have any package listed in the file <u>/service/update/variables/UPGRADE</u>.

So when I retire for the night to get a good night sleep, I use an android app known as `ssh button` to shutdown the NFSv4 server. The moment the server shuts down, all my clients shutdown together. The PI4 has a design problem. It doesn't shut off the power to the USB3 ports when shutdown. Due to his, any hard disk connected to PI4 will not spin down when the PI4 is shutdown. I use [uhubctl](https://github.com/mvp/uhubctl) to shutdown power to the usb port. `uhubctl` is utility to control USB power per-port on smart USB hubs. I have also created a systemd service /lib/systemd/system/usb3-poweroff.service to use uhubctl to turn off power.

```
[Unit]
Description=Turns off usb3 power on shutdown/reboot
DefaultDependencies=no
After=umount.target

[Service]
Type=oneshot
ExecStart=/usr/sbin/uhubctl -a 0 -l 2

[Install]
WantedBy=halt.target poweroff.target
```

The above will work if your drive is unmounted. To achieve unmounting of drives, I call my own shutdown script (pishutdown).

```
/home/pi/bin/pishutdown
#!/bin/sh
if [ $# -gt 1 ] ; then
  if [ " $1" = " -h" ] ; then
    # shut down mpdev service
    /usr/bin/svc -d /service/fserver /service/mpdev
    sync
    systemctl stop svscan mpd smbd nmbd nfs-server
    umount /home/pi/MDrive
    sync
  fi
fi
exec /sbin/shutdown $*
```

Using home automation, I then switch off the power output to each and very adaptor thereby saving around 0.05 kwhr of electricity per device every day. The PI4 consumes around 2 units per month if you leave them switched on 24x7. You could say that these SBC consume miniscule power, but this kind of philosophy has led us mankind to plunder this planet. Global Warming is a stark reality of the day and we should try to conserve electricity as much as possible. Please note that if you use wifi switches to switch off devices, these switches by itself will consume 1.5-2 watts even when off, resulting in around 1.5 Units of electricity every month.

You can take a look at the files, configuration for my server and client in the directory [example_config](https://github.com/mbhangui/pistop/tree/master/example_config). Along with pistop, I use [mdev](https://github.com/mbhangui/mpdev) to maintain playcounts and ratings. This is something that mpd(1) doesn't provide out of the box.

## INSTALLATION

On the server (use the IP Address of your own server). This will create fserver supervise service in /service

```
# apt-get update
# apt-get install pistop
# /usr/libexec/pistop/create_service --servicedir=/service --service_name=fserver --host=192.168.1.101 --port=5555 --add-service
```

On each client (use IP Address of your server). This will create fclient supervise service in /service

```
# apt-get update
# apt-get install pistop
# /usr/libexec/pistop/create_service --servicedir=/service --service_name=fclient --host=192.168.1.101 --port=5555 --add-service
```

If you want to have your  clients shutdown itself when the server is powered off, create `POWER_OFF` enviroment variable by doing this
`sudo sh -c "echo 1 > /service/fclient/variables/POWER_OFF"`

**Client AUTOMOUNT SETUP**

```
file /etc/auto.master.d/mpd.autofs
/var/lib/mpd /etc/autofs.mounts timeout=60

file /etc/autofs.mounts
MDrive -fstype=nfs,vers=4,soft,timeo=5,retry=1 MusicPI:/MDrive
```

**Server NFSv4 SETUP**

```
file /etc/exports
/MDrive 192.168.2.0/24(rw,async,no_root_squash,subtree_check,anonuid=1000,anongid=1000)

file /etc/fstab
proc                                      /proc           proc  defaults                                          0 0
PARTUUID=5e3da3da-01                      /boot           vfat  defaults                                          0 2
PARTUUID=5e3da3da-02                      /               ext4  defaults,noatime                                  0 1
UUID=20bcc800-9dce-4734-947d-6d760b36a4de /MDrive         ext4  defaults,noatime,noauto,x-systemd.automount       0 2
tmpfs                                     /tmp            tmpfs defaults,size=250M,noatime,nodev,nosuid,mode=1777 0 0
tmpfs                                     /var/tmp        tmpfs defaults,size=200M,noatime,nodev,nosuid,mode=1777 0 0
# a swapfile is not a swap partition, no line here
#   use  dphys-swapfile swap[on|off]  for that
```

NOTE: You only need to add the line having `UUID=` in your /etc/fstab. Your UUID for your external drive will be different. Use blkid(1) to find that out.

**Server Samba SETUP**

```
[global]

   workgroup = WORKGROUP
   mangled names = no
   fruit:encoding = native
   unix charset = UTF-8
   catia:mappings = 0x22:0xa8,0x2a:0xa4,0x2f:0xf8,0x3a:0xf7,0x3c:0xab,0x3e:0xbb,0x3f:0xbf,0x5c:0xff,0x7c:0xa6

[NAS]
    comment = Music Folder Share
    path = /home/pi/MDrive/Music
    browseable = yes
    create mask = 0755
    directory mask = 0755
    valid users = pi, root
    admin users = pi
    writeable = yes
    vfs objects = catia
;   locking = no
;   oplocks = no
;   kernel oplocks = no
;   posix locking = no
    max connections = 8
```

**Update Service**

You can have your SBC automatically updated. You can have list of packages that the service will automatically update by creating the environment variable UPGRADE. The default value for this environment variable is `ucspi-tcp mpdev libqmail indimail-mini daemontools pistop mpd`. You can edit the file /service/update/variables/UPGRADE to update the environment variable.

```
$ sudo /usr/libexec/pistop/create_service --service_name=update --add-service
```

**RSYNC Service**

You can have a rsync service running on your SBC. rsync service is useful if you want to transfer music files from any computer to the SBC. This can be created by running

```
$ sudo /usr/libexec/pistop/create_service --service_name=rsync --add-service
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


## NOTE for binary builds on debian

debian/ubuntu repositories already has daemontools and ucspi-tcp which are far behind in terms of feature list that the indimail-mta repo provides. When you install indimail-mta, apt-get may pull the wrong version with limited features. Also `apt-get install indimail` or `apt-get install indimail-mta` will get installed with errors, leading to an incomplete setup. You need to ensure that the two packages get installed from the indimail-mta repository instead of the debian repository.

All you need to do is set a higher preference for the indimail-mta repository by creating /etc/apt/preferences.d/preferences with the following conents

```
$ sudo /bin/bash
# cat > /etc/apt/preferences.d/preferences <<EOF
Package: *
Pin: origin download.opensuse.org
Pin-Priority: 1001
EOF
```

You can verify this by doing

```
$ apt policy daemontools ucspi-tcp
daemontools:
  Installed: 2.11-1.1+1.1
  Candidate: 2.11-1.1+1.1
  Version table:
     1:0.76-7 500
        500 http://raspbian.raspberrypi.org/raspbian buster/main armhf Packages
 *** 2.11-1.1+1.1 1001
       1001 http://download.opensuse.org/repositories/home:/indimail/Debian_10  Packages
        100 /var/lib/dpkg/status
ucspi-tcp:
  Installed: 2.11-1.1+1.1
  Candidate: 2.11-1.1+1.1
  Version table:
     1:0.88-6 500
        500 http://raspbian.raspberrypi.org/raspbian buster/main armhf Packages
 *** 2.11-1.1+1.1 1001
       1001 http://download.opensuse.org/repositories/home:/indimail/Debian_10/ Packages
        100 /var/lib/dpkg/status
```
