# Files for mpd server and mpd client

These are actual files on the main mpd server and mpd clients. The mpd server serves the music database for all mpd clients. The music database is on a sata disk connected to the Raspberry PI4 server through a USB3 port. It uses NFSv4 to export /home/pi/MDrive. The server files are in the server directory. The client files are in the client directory. All devices have a user named pi with uid=1000 and gid=1000. The mpd database server is a Raspberry PI4 device. The clients are linux laptops, banana pi, allo sparky and raspberry pi3 boards.

## Server Setup

The music lies on a SATA hard disk. The hard disk has three important directories

1. data - This has the stats.db and sticker.db sqlite databases. It also has the mpd database as tag\_cache.PI4. The database plugin in /etc/mpd.conf provides the mpd database for all mpd clients
2. Music - The directory where your music files are stored
3. playlists - This is where playlists get saved when you save playlists using any of the mpd players

Files|Purpose
-----|-------
/etc/exports|Export /home/pi/MDrive for all clients. You have to install/enable nfs-server service
/etc/fstab|Entry of external SATA disk. It uses systemd to mount the disk whenever accessed as /home/pi/MDrive
/etc/mpd.conf|This has the music_directory entry as /var/lib/mpd/MDrive/Music. /var/lib/mpd/MDrive is a symbolic link to /home/pi/MDrive.
/lib/systemd/system/svscan.service|Starts svscan service to start srvices configured in /service
/lib/systemd/system/mpd.service|The mpd database service
/lib/systemd/system/usb3-poweroff.service|Shuts down the usb3 port when raspberry pi4 is powered off. This helps the connected disk to automatically go into standy state
/etc/samba/smb.conf|Samba configuration to allow windows server to mount the disk
/service/fserver|This is a svscan service. It lets clients know when the music server is up so that the clients can mount the disk to play music
/service/mpdev|This is the [mpd event service](https://github.com/mbhangui/mpdev). This gets used only when you play music on the server. It updates the playcounts and ratings when a song is played

## Client Setup

The clients uses /service/fclient svscan service to start automount and mpd service. The mpd.conf has mpd database proxy service configured to use the server serve the mpd database.

Files|Purpose
-----|-------
/etc/mpd.conf|This has the music_directory entry as /var/lib/mpd/MDrive/Music. /var/lib/mpd/MDrive is a symbolic link to /home/pi/MDrive.
/lib/systemd/system/svscan.service|Starts svscan service to start srvices configured in /service
/lib/systemd/system/mpd.service|The mpd database service
/service/fclient|This is a svscan service. It starts the automount service to mount the disk on server and also starts the mpd service. When the server is shutdown, it unmounts the disk and stops the automount and mpd service
/service/mpdev|This is the [mpd event service](https://github.com/mbhangui/mpdev). This updates the playcounts and ratings when a song is played
