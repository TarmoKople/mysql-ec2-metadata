# MySQL EC2 metadata plugin

Exposes EC2 instance metadata using INFORMATION_SCHEMA.

## Disclaimer

mysql-ec2-metadata plugin can crash your server, corrupt your data, empty your credit card or burn down your neighbors house, ... I don't take any responsibility for this !

## Description

MySQL EC2 metadata plugin downloads EC2 instance metadata from metadata service and exposes downloaded information in INFORMATION_SCHEMA.EC2_META table:

```mysql> select * from EC2_META;
+--------------------------------+-----------------------------------------------+
| METAOPT                        | VALUE                                         |
+--------------------------------+-----------------------------------------------+
| placement/availability-zone    | eu-central-1b                                 |
| placement/availability-zone-id | euc1-az3                                      |
| placement/region               | eu-central-1                                  |
| ami-id                         | ami-0a95c78892a8d8b50                         |
| ami-launch-index               | 0                                             |
| ami-manifest-path              | (unknown)                                     |
| hostname                       | ip-172-29-55-67.eu-central-1.compute.internal |
| instance-action                | none                                          |
| instance-id                    | i-0482dce76cd95cf93                           |
| instance-life-cycle            | on-demand                                     |
| instance-type                  | m6i.xlarge                                    |
| local-hostname                 | ip-172-29-55-67.eu-central-1.compute.internal |
| local-ipv4                     | 172.29.55.67                                  |
| mac                            | 06:ac:da:ec:90:00                             |
| profile                        | default-hvm                                   |
| reservation-id                 | r-09985cb3c47c494f3                           |
| security-groups                | default                                       |
+--------------------------------+-----------------------------------------------+
17 rows in set (0.00 sec)
```
EC2 metadata is only downloaded once when the EC2_META table is first read, usually this information is not changing while the instance is running. To reload the data please use ```uninstall plugin``` and ```install plugin``` command or restart MySQL.

## Why ? Use cases
mysql-ec2-metadata plugin makes MySQL AWS EC2 aware. This helps to build smarter automation or tools needed for your MySQL database infrastructure.  Information exposed in the EC2_META table can be used in many different ways.

#### Integrating MySQL Orchestrator and mysql-ec2-metadata plugin:

This can be done in to simple steps:
* install mysql-ec2-metadata
* edit MySQL Orchestrator config, add:
```"DetectDataCenterQuery": "select VALUE from information_schema.ec2_meta where metaopt='placement/availability-zone';",```

![Orchestrator](https://github.com/TarmoKople/mysql-ec2-metadata/blob/main/images/orchestrator.png)

## Getting Started

### Dependencies

* MySQL

### Installing

mysql> INSTALL PLUGIN EC2_META SONAME 'ec2_meta.so';

### Executing program

mysql> SELECT * FROM INFORMATION_SCHEMA.EC2_META;

### Uninstalling

mysql> UNINSTALL PLUGIN EC2_META;

## Building
### Preparing the environment
These instructions are for Ubuntu (20.04)
Setup build environment.
```
apt install build-essential
apt install pkg-config
```
Enable source repositories for your system. (edit /etc/apt/sources.list, uncomment lines starting with ```#deb-src``` and then run ```apt update```) 
Install dependencies needed to build MySQL:
```
apt build-dep mysql-server
```
Download and unpack MySQL source code:
```
apt source mysql-server
```
### Compiling the Plugin
This step is not Linux distribution specific. The same method can be used for any Linux distribution if build environment is properly set up.
Enter to MySQL source directory
```
cd mysql-<version>
```

Download the plugin source code and store it under MySQL plugins directory. The easiest way is to use git. In MySQL source directory do:
```
git clone https://github.com/TarmoKople/mysql-ec2-metadata.git plugin/ec2-metadata
```
This downloads source code to plugins/ec2-metadata direcotry.
* **NB!** if you need source code for MySQL 5.7 please check out **5.7** branch

Create a build directory (builddir) inside MySQL source director, cd to builddir and configure source
```
mkdir builddir
cd builddir
cmake -DDOWNLOAD_BOOST=1 -DWITH_BOOST=. ..
```

To build the plugin and MySQL type ```make``` in builddir, to build just the plugin ```make ec2_meta```

Compiled plugin can be found under: builddir/plugin_output_directory/ec2_meta.so

#### Installing compiled plugin
Copy compiled plugin to MySQL plugin directory. To find out where is the plugin directory is located execute:
```
mysql> SHOW GLOBAL VARIABLES LIKE "%plugin_dir%";
+---------------+-------------------------+
| Variable_name | Value                   |
+---------------+-------------------------+
| plugin_dir    | /usr/lib/mysql/plugin/  |
+---------------+-------------------------+
1 row in set (0.01 sec)
```

For Ubuntu / Debian it is by default /usr/lib/mysql/plugin
```
cp plugin_output_directory/ec2_meta.so /usr/lib/mysql/plugin/
```

## Author

Tarmo Kople


## Version History

* 0.1
  * Initial Release

## License

This project is licensed under the [NAME HERE] License - see the LICENSE.md file for details
