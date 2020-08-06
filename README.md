# Post View Aplication

[![License](https://img.shields.io/github/license/LincolnUehara/qt-JSON-PostViewer)](https://github.com/LincolnUehara/qt-JSON-PostViewer/blob/master/LICENSE)
![Maintenance](https://img.shields.io/maintenance/no/2020)

This is a simple aplication made in Qt framework that check users' posts, using [JSONPlaceholder](https://jsonplaceholder.typicode.com/) for testing and prototyping.

<p align="center">
<img src="https://github.com/LincolnUehara/qt-JSON-PostViewer/blob/master/media/app.gif" width="400">
</p>

### How to compile it

* It is needed to install Qt **5.15.0** because of functions like `setTransferTimeout()` that were implemented in this version.

The simplest way to compile it is using Qt Creator. Commands below sumarizes the IDE's installation, and follow the wizard to complete it.
```
sudo apt-get update
sudo apt-get install build-essential libgl1-mesa-dev -y
wget http://download.qt.io/official_releases/online_installers/qt-unified-linux-x64-online.run
chmod a+x qt-unified-linux-x64-online.run
./qt-unified-linux-x64-online.run
```

* As written before, choose to use Qt **5.15.0**.
![Kit Selection](https://github.com/LincolnUehara/qt-JSON-PostViewer/blob/master/media/kit_select.png)

* Check if there is `QT += network` at `.pro` archive.

### Author

Lincoln Uehara
