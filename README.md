# 文件备份与恢复软件(backup)

本软件为普通个人用户在PC上使用的非商用的数据备份与恢复软件。
除基本的数据备份功能外，还包含对数据的打包解包、压缩解压、加密解密等功能。

## 开发环境

- 操作系统：Linux (Ubuntu22.04 LTS)
- 开发语言：C++
- 编译环境：gcc 11.3.0
- IDE：Visual Studio Code
- 构建工具：cmake 3.22.1
- 发布工具：docker

## 第三方库
- OpenSSL 1.1.1n：用于辅助实现AES加密算法
- cxxopts：用于辅助实现命令行参数用户界面


## 安装

切换到项目根目录下，执行如下命令构建镜像

```
docker build -t backup_image .
```

启动容器

```
docker run -it --name backup_test backup_image /bin/bash
```


## 具体功能

- 数据备份：将目录树中的文件数据保存到指定位置
- 数据还原：将目录树中的文件数据恢复到指定位置
- 文件类型支持：支持特定文件系统的特殊文件（普通/目录/管道/软链接/硬链接）
- 元数据支持：支持特定文件系统的文件元数据（属主/时间/权限等）
- 自定义备份：允许用户筛选需要备份的文件（路径/类型/名字/时间）
- 压缩解压：通过文件压缩节省备份文件的存储空间
- 打包解包：将所有备份文件拼接为一个大文件保存
- 加密解密：由用户指定密码，将所有备份文件均加密保存
- 用户界面：用户可以通过命令行对软件进行方便的操作
- 帮助信息：用户可以查看软件帮助文档、备份文件的信息、备份与恢复的过程信息等

## 帮助文档

```
A data backup and restore tool for Linux
Usage:
  backup [OPTION...]

  -b, --backup        备份
  -r, --restore       恢复
  -l, --list arg      查看指定备份文件的信息
  -v, --verbose       输出执行过程信息
  -i, --input arg     程序输入文件路径
  -o, --output arg    程序输出文件路径
  -p, --password arg  指定密码
  -h, --help          查看帮助文档

 Backup options:
  -c, --compress     备份时压缩文件
  -e, --encrypt      备份时加密文件
      --path arg     过滤路径：正则表达式
      --type arg     过滤文件类型: n普通文件,d目录文件,l链接文件,p管道文件
      --name arg     过滤文件名：正则表达式
      --atime arg    文件的访问时间区间, 例"2024-1-9 10:00:00 2024-1-9 12:00:00"
      --mtime arg    文件的修改时间区间, 格式同atime
      --ctime arg    文件的改变时间区间, 格式同atime
  -m, --message arg  添加备注信息

 Restore options:
  -a, --metadata  恢复文件的元数据
```

## 测试数据说明
### 切换路径
为了方便测试，请先切换到`test`文件目录
```shell
cd test
```

### 解压文件
在进行测试前先解压文件夹
```shell
tar -zxvf testfolder.tar.gz
```

### 文件目录结构

```shell
testfolder
├── normal_file # 普通文本文件
├── fifofile    # 管道文件
├── linkfolder
│   ├── testfile.tar.gz
├── hard_link   # 硬链接 指向linkfolder/testfile.tar.gz
├── include
│   ├── encryptor.h
│   ├── argparser.h
│   ├── compressor.h
│   ├── cxxopts.hpp
│   ├── filebase.h
│   ├── filter.h
│   ├── packer.h
│   └── task.h
└── soft_link -> linkfolder/testfile.tar.gz # 软链接
```

## 示例

**查看帮助文档**
```
backup -h
```

**备份普通文件**

`-b`表示备份，
`-v`表示输出备份过程信息
`-m`用于给出备份说明信息

```shell
backup -bv -i testfolder -o outputs/testfolder -m "backup_message"
```

**压缩并加密文件**

`-c`表示对打包文件进行压缩，
`-e`表示对文件进行加密，
`-p`用于指定加密密码

```shell
backup -bvce -i testfolder -o outputs/testfolder_ce -p 123123 
```

**查看备份文件信息**
```
backup -l testfolder.pak.cps.ept
```

**自定义备份(文件筛选）**

`--path`指定**路径**的匹配规则(正则表达式)，
下面的指令只备份`testfolder/include`和`testfolder/linkfolder`两个目录树

```
backup -bv -i testfolder -o outputs/testfolder_path --path "^testfolder/(include|linkfolder)"
```

`--name`指定**文件名**的匹配规则(正则表达式)，
下面的指令只备份以`.h`为后缀的文件

```
backup -bv -i testfolder -o outputs/testfolder_name --name "\.(h)$"
```

`--type`指定**文件类型**，`n`普通文件,`d`目录文件,`l`链接文件,`p`管道文件

下面的指令只备份普通文件和链接文件

```
backup -bv -i testfolder -o outputs/testfolder_type --type "nl"
```

`--atime`,`--mtime`, `--ctime`分别用于指定**文件的时间戳范围**，
下面的指令只备份在`2024-1-9 10:00:00`到`2024-1-9 12:00:00`之间修改过的文件
```
backup -bv -i testfolder -o outputs/testfolder_atime --mtime "2024-1-9 10:00:00 2024-1-9 12:00:00"
```

**恢复文件**

`-r`表示恢复文件，
`-a`表示需要恢复文件元数据，
`-i`指定备份文件，
`-o`指定文件恢复到哪个位置
`-p`用于指定解密用的密码

```
backup -rva -i testfolder_ce.pak.cps.ept -o restore_file/ -p 123123
```

```
backup -rv -i testfolder_path.pak -o restore_file/path
```