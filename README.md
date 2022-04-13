# MySQL EC2 metadata plugin

Exposes EC2 instance metadata using INFORMATION_SCHEMA.

## Disclaimer

mysql-ec2-metadata plugin can crash your server, corrupt your data, empty your credit card or burn down your neighbors house, ... I don't take any responsibility for this !

## Description

MySQL EC2 metadata plugin downloads EC2 instance metada from metada service and exposes downloaded information in INFORMATION_SCHEMA.EC2_META table:

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

## Getting Started

### Dependencies

* MySQL

### Installing

mysql> INSTALL PLUGIN EC2_META SONAME 'ec2_meta.so';

### Executing program

mysql> SELECT * FROM EC2_META;

### Uninstalling

mysql> UNINSTALL PLUGIN EC2_META;

## Help

Any advise for common problems or issues.

## Author

Tarmo Kople


## Version History

* 0.1
  * Initial Release

## License

This project is licensed under the [NAME HERE] License - see the LICENSE.md file for details
