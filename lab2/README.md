# Лабораторная работа 2

**Название:** "Разработка драйверов блочных устройств"

**Цель работы:** получить знания и навыки разработки драйверов блочных 
устройств для операционной системы Linux.

## Описание функциональности драйвера

Один первичный раздел размером 30Мбайт и один расширенный раздел, содержащий два логических раздела размером 10Мбайт каждый

## Инструкция по сборке

Сборка модуля:

```
make
```

Сборка с последующей установкой:

```
make install
```

Отключение модуля:

```
make uninstall
```

Очистка артефактов сборки:

```
make clean
```

## Инструкция пользователя

Для начала проверим, что диск дейтвительно существет, выполнив команду:

```
fdisk -l /dev/vramdisk
```

Получаем результат:

```
Disk /dev/vramdisk: 50 MiB, 52428800 bytes, 102400 sectors
Units: sectors of 1 * 512 = 512 bytes
Sector size (logical/physical): 512 bytes / 512 bytes
I/O size (minimum/optimal): 512 bytes / 512 bytes
Disklabel type: dos
Disk identifier: 0x36e5756d

Device         Boot Start    End Sectors Size Id Type
/dev/vramdisk1          1  61439   61439  30M 83 Linux
/dev/vramdisk2      61440 102399   40960  20M  5 Extended
/dev/vramdisk5      61441  81919   20479  10M 83 Linux
/dev/vramdisk6      81921 102399   20479  10M 83 Linux
```

Создадим в разделах файловую систему:

```
mkfs.vfat /dev/vramdisk1
mkfs.vfat /dev/vramdisk5
mkfs.vfat /dev/vramdisk6
```

Далее её можно смонтировать:

```
mount -t vfat /dev/vramdisk1 /mnt/vram1
mount -t vfat /dev/vramdisk5 /mnt/vram5
mount -t vfat /dev/vramdisk6 /mnt/vram6
```

Проверим, что монтирование произошло:

```
df 
```

Результат:

```
Filesystem       1K-blocks      Used Available Use% Mounted on
tmpfs               202324      1212    201112   1% /run
/dev/sda3         25106692  13695532  10110476  58% /
tmpfs              1011608         0   1011608   0% /dev/shm
tmpfs                 5120         4      5116   1% /run/lock
/dev/sda2           524252      5364    518888   2% /boot/efi
VirtualBoxFolder 437981180 437888232     92948 100% /media/sf_VirtualBoxFolder
tmpfs               202320       116    202204   1% /run/user/1000
/dev/vramdisk1       30626         0     30626   0% /mnt/vram1
/dev/vramdisk5       10186         0     10186   0% /mnt/vram5
/dev/vramdisk6       10186         0     10186   0% /mnt/vram6
```

## Примеры использования

Для проверки создадим достаточно большой файл:

```
fallocate -l 7M /mnt/vram1/file
```

Затем оценим скорость передачи между разделами одного диска:

```
pv /mnt/vram1/file > /mnt/vram5/file
```

Результат:

```
7,00MiB 0:00:00 [ 256MiB/s] [==============================================================================================================================================================================================>] 100% 
```

Аналогично для копирования из 5 в 6 разделы:

```
pv /mnt/vram5/file > /mnt/vram6/file
```

```
7,00MiB 0:00:00 [ 335MiB/s] [==============================================================================================================================================================================================>] 100%
```

Теперь проверим запись с виртуального жёсткого диска в реальный:

```
pv /mnt/vram1/file > /home/file
```

```
7,00MiB 0:00:00 [ 544MiB/s] [==============================================================================================================================================================================================>] 100%
```

```
pv /mnt/vram5/file > /home/file
```

```
7,00MiB 0:00:00 [ 443MiB/s] [==============================================================================================================================================================================================>] 100% 
```

```
pv /mnt/vram6/file > /home/file
```

```
7,00MiB 0:00:00 [ 482MiB/s] [==============================================================================================================================================================================================>] 100%
```
